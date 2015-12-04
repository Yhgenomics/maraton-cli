#ifndef MESSAGE_TASK_DELIVER_HANDLER_HPP_
#define MESSAGE_TASK_DELIVER_HANDLER_HPP_

#include "MessageTaskDeliver.hpp"
#include "MessageTaskDeliverACK.hpp"
#include "PostOffice.h"
#include <vector>
#include <string>
#include "FileDownloader.h"
#include "SysProcess.h"
#include "MessageTaskResult.hpp"
#include "FileUploader.h"
#include <iostream>
#include "ExecutorConfig.hpp"
#include "AsyncWorker.h"
#include "MessageTaskProgress.hpp"
#include "AnalysisHelper.h"
#include "uv.h"
#include <chrono>
#include <thread>
#include <typeinfo>

using namespace std;

namespace Protocol
{
    static  void    PullBegin   ( AsyncWorker* asyncWorker );
    static  void    PullEnd     ( AsyncWorker* asyncWorker );
    static  void    ProcessBegin( AsyncWorker* asyncWorker );
    static  void    ProcessEnd  ( AsyncWorker* asyncWorker );
    static  void    InspectOn   ( AsyncWorker* asyncWorker );
    static  void    InspectOff  ( AsyncWorker* asyncWorker );

    static  uv_mutex_t taskStatusMutex;
    static  uv_mutex_t outputMutex;

    static  int     MessageTaskDeliverHandler( MessageTaskDeliver msg )
    {
        // UserDefineHandler Begin
        // Your Codes here!
        if ( PostOffice::instance()->self_status == 3 )
        {
            PostOffice::instance()->self_status = PostOffice::ExcutorSates::kComputing;
            PostOffice::instance()->SendSelfStatus();

            PostOffice::instance()->current_task                                = msg.task_id();
            PostOffice::instance()->task_board[ msg.task_id() ].task_id         = msg.task_id();
            PostOffice::instance()->task_board[ msg.task_id() ].ref_gen_name    = msg.reference();

            for ( auto uri : msg.uri_list() )
            {
                PostOffice::instance()->task_board[ msg.task_id() ].block_map[ uri ] = TaskStatus::BlockStatus::kWait;
            }

            AsyncWorker::create( PullBegin    , PullEnd    , nullptr );
            AsyncWorker::create( ProcessBegin , ProcessEnd , nullptr );
            AsyncWorker::create( InspectOn    , InspectOff , nullptr );
        }

        return 0;
        // UserDefineHandler End
    }

    static  void    PullBegin   ( AsyncWorker* asyncWorker )
    {
        cout << "Pull Begin" << endl;

        for ( auto& item : PostOffice::instance()->GetCurrentTaskStatus().block_map)
        {
            uv_mutex_lock  ( &taskStatusMutex );
            item.second = MaratonCommon::TaskStatus::BlockStatus::kPulling;
            uv_mutex_unlock( &taskStatusMutex );

            string fileName ;
            ExecutorConfig::instance()->GetFileName( item.first, fileName, ExecutorConfig::SuffixType::kInputFile );

            bool cancel = false;
            FileDownloader fileDownloader( &cancel );
            fileDownloader.DownloadViaHTTP( ExecutorConfig::instance()->inputDir + fileName  , item.first );
            std::cout << "################ File download to" << ExecutorConfig::instance()->inputDir << fileName << endl;

            uv_mutex_lock  ( &taskStatusMutex );
            item.second = MaratonCommon::TaskStatus::BlockStatus::kPulled;
            uv_mutex_unlock( &taskStatusMutex );
        }
    }

    static  void    PullEnd     ( AsyncWorker* asyncWorker )
    {
        cout << "############## Pull all done!" << endl;
    }

    static  void    ProcessBegin( AsyncWorker* asyncWorker )
    {
        while ( !PostOffice::instance()->GetCurrentTaskStatus().IsAllProcessed() )
        {
            bool NoFileToProcess = true;
            string fileName;

            for ( auto& item : PostOffice::instance()->GetCurrentTaskStatus().block_map )
            {
                if ( MaratonCommon::TaskStatus::BlockStatus::kPulled == item.second )
                {
                    ExecutorConfig::instance()->GetFileName( item.first, fileName, ExecutorConfig::SuffixType::kInputFile );
                    NoFileToProcess = false;

                    uv_mutex_lock  ( &taskStatusMutex );
                    item.second = MaratonCommon::TaskStatus::BlockStatus::kProcessing;
                    uv_mutex_unlock( &taskStatusMutex );

                    MaratonCommon::AnalysisHelper analysisHelper;
                    analysisHelper.CheckEnviroment();
                    auto exitStatus=  analysisHelper.ProcessData( ExecutorConfig::instance()->threadNum
                            , PostOffice::instance()->GetCurrentTaskStatus().ref_gen_name
                            , fileName) ;

                    uv_mutex_lock  ( &taskStatusMutex );
                    item.second = ( 0 == exitStatus )? MaratonCommon::TaskStatus::BlockStatus::kProcessed : MaratonCommon::TaskStatus::BlockStatus::kException;
                    uv_mutex_unlock( &taskStatusMutex );

                    cout << "file processed " << fileName;
                    break;
                }
            }

            if ( NoFileToProcess && !PostOffice::instance()->GetCurrentTaskStatus().IsAllProcessed() )
            {
                cout << "############# No file to process yet" << endl;
                std::this_thread::sleep_for( std::chrono::milliseconds( ExecutorConfig::instance()->transWait ) );
            }
        }
    }

    static  void    ProcessEnd  ( AsyncWorker* asyncWorker )
    {
        cout << "############## process All Down "<< endl;
        cout << "############## process meet any exception ? " << PostOffice::instance()->GetCurrentTaskStatus().IsAnyException() << endl;;
    }

    static  void    InspectOn   ( AsyncWorker* asyncWorker )
    {
        while ( !PostOffice::instance()->GetCurrentTaskStatus().IsAllFinished() )
        {
            if ( PostOffice::instance()->GetCurrentTaskStatus().IsAnyException() )
            { break; }

            bool NoFileToPush = true;
            string fileName;

            for ( auto& item : PostOffice::instance()->GetCurrentTaskStatus().block_map )
            {
                if ( MaratonCommon::TaskStatus::BlockStatus::kProcessed == item.second )
                {
                    ExecutorConfig::instance()->GetFileName( item.first, fileName, ExecutorConfig::SuffixType::kOutPutFile );
                    NoFileToPush = false;

                    uv_mutex_lock  ( &taskStatusMutex );
                    item.second = MaratonCommon::TaskStatus::BlockStatus::kPushing;
                    uv_mutex_unlock( &taskStatusMutex );

                    FileUploader uploader;
                    uploader.UploadFileViaHttp( PostOffice::instance()->GetCurrentTaskStatus().task_id
                            , ExecutorConfig::instance()->outputDir + fileName
                            , ExecutorConfig::instance()->postDest );

                    uv_mutex_lock  ( &taskStatusMutex );
                    item.second = MaratonCommon::TaskStatus::BlockStatus::kPushed;
                    uv_mutex_unlock( &taskStatusMutex );

                    //TODO handle push exception and reliable result deliver
                    uv_mutex_lock  ( &taskStatusMutex );
                    item.second = MaratonCommon::TaskStatus::BlockStatus::kFinished;
                    uv_mutex_unlock( &taskStatusMutex );

                    cout << "file pushed " << fileName;
                    break;
                }
            }

            if( NoFileToPush )
            {
                cout << "################### No file to push yet" << endl;
                std::this_thread::sleep_for(std::chrono::milliseconds( ExecutorConfig::instance()->processWait ));
            }
        }
    }

    static  void    InspectOff  ( AsyncWorker* asyncWorker )
    {
        cout << "################### Task over " << endl;

        MessageTaskResult msgout;

        PostOffice::instance()->self_status = PostOffice::ExcutorSates::kTaskFinished;
        PostOffice::instance()->SendSelfStatus();

        std::cout << "Task done" << std::endl;

        msgout.task_id( PostOffice::instance()->current_task );
        if ( PostOffice::instance()->GetCurrentTaskStatus().IsAnyException() )
        {
            msgout.error( 1 );
            msgout.result( "#############[ EXCEPTION ]: At least one block in the list cannot be processed" );

        }
        PostOffice::instance()->SendMail( &msgout );

        PostOffice::instance()->self_status = PostOffice::ExcutorSates::kStandby;
        PostOffice::instance()->SendSelfStatus();
        cout << "Standby" << endl;
    }

} // End of namespace Protocol
#endif // !Message_Task_Deliver_HANDLER_HPP_ 
