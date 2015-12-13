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

//#difine DEBUG_ONLY

using namespace std;

namespace Protocol
{
    static  void    PullBegin   ( AsyncWorker* asyncWorker );
    static  void    PullEnd     ( AsyncWorker* asyncWorker );
    static  void    ProcessBegin( AsyncWorker* asyncWorker );
    static  void    ProcessEnd  ( AsyncWorker* asyncWorker );
    static  void    InspectOn   ( AsyncWorker* asyncWorker );
    static  void    InspectOff  ( AsyncWorker* asyncWorker );
    static  void    SortBegin   ( AsyncWorker* asyncWorker );
    static  void    SortEnd     ( AsyncWorker* asyncWorker );
    static  void    SortOneBegin( AsyncWorker* asyncWorker );
    static  void    SortOneEnd  ( AsyncWorker* asyncWorker );

    static  uv_mutex_t  taskStatusMutex;

#ifdef DEBUG_ONLY
    static  uv_mutex_t  outputMutex;
    static  uv_mutex_t  clockMutex;

    static  chrono::steady_clock::time_point   start_at;
    static  chrono::steady_clock::time_point   end_at;
    static  chrono::steady_clock::time_point   proceess_start_at;
    static  chrono::steady_clock::time_point   proceess_end_at;
    static  chrono::steady_clock::time_point   one_begin;
    static  chrono::steady_clock::time_point   one_end;

    static  int processNum = 0;
#endif

    static  int     MessageTaskDeliverHandler( MessageTaskDeliver msg )
    {
#ifdef DEBUG_ONLY
        uv_mutex_lock   ( &clockMutex );
        start_at   = chrono::steady_clock::now();
        processNum = 0;
        uv_mutex_unlock ( &clockMutex );
#endif

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
           // AsyncWorker::create( InspectOn    , InspectOff , nullptr );
        }

        return 0;
        // UserDefineHandler End
    }

    static  void    PullBegin   ( AsyncWorker* asyncWorker )
    {
        cout << endl << "############ Pull Begin" << endl;

        for ( auto& item : PostOffice::instance()->GetCurrentTaskStatus().block_map)
        {
            uv_mutex_lock  ( &taskStatusMutex );
            item.second = MaratonCommon::TaskStatus::BlockStatus::kPulling;
            uv_mutex_unlock( &taskStatusMutex );

            string fileName = item.first;
            ExecutorConfig::instance()->GetFileName( fileName, ExecutorConfig::SuffixType::kInputFile );
            string uri      = "";
            ExecutorConfig::instance()->GetUri( uri, ExecutorConfig::instance()->pullFastq, item.first, "");

            bool cancel = false;
            FileDownloader fileDownloader( &cancel );
            fileDownloader.DownloadViaHTTP( ExecutorConfig::instance()->inputDir + fileName  , uri );

            uv_mutex_lock  ( &taskStatusMutex );
            item.second = MaratonCommon::TaskStatus::BlockStatus::kPulled;
            uv_mutex_unlock( &taskStatusMutex );
        }
    }

    static  void    PullEnd     ( AsyncWorker* asyncWorker )
    {
        cout << endl << "############ Pull all done!" << endl;
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
#ifdef DEBUG_ONLY
                    if( 0 == processNum )
                    {
                        uv_mutex_lock   ( &clockMutex );
                        proceess_start_at = chrono::steady_clock::now();
                        ++processNum;
                        uv_mutex_unlock ( &clockMutex );
                    }
#endif
                   //  fileName = item.first;
                   //  ExecutorConfig::instance()->GetFileName( fileName, ExecutorConfig::SuffixType::kInputFile );
                    NoFileToProcess = false;

                    uv_mutex_lock  ( &taskStatusMutex );
                    item.second = MaratonCommon::TaskStatus::BlockStatus::kProcessing;
                    uv_mutex_unlock( &taskStatusMutex );

                    MaratonCommon::AnalysisHelper analysisHelper;

                    analysisHelper.CheckEnviroment();
                    auto exitStatus=  analysisHelper.ProcessData( ExecutorConfig::instance()->threadNum
                            , PostOffice::instance()->GetCurrentTaskStatus().ref_gen_name
                            , item.first ) ;

                    uv_mutex_lock  ( &taskStatusMutex );
                    item.second = ( 0 == exitStatus )? MaratonCommon::TaskStatus::BlockStatus::kProcessed : MaratonCommon::TaskStatus::BlockStatus::kException;
                    uv_mutex_unlock( &taskStatusMutex );

                    break;
                }
            }

            if ( NoFileToProcess && !PostOffice::instance()->GetCurrentTaskStatus().IsAllProcessed() )
            {
                this_thread::sleep_for( chrono::milliseconds( ExecutorConfig::instance()->transWait ) );
            }
        }
    }

    static  void    ProcessEnd  ( AsyncWorker* asyncWorker )
    {
#ifdef DEBUG_ONLY
        uv_mutex_lock   ( &clockMutex );
        proceess_end_at = chrono::steady_clock::now();
        uv_mutex_unlock ( &clockMutex );
#endif

        cout << "############ process All Down "<< endl;

        if( false ==  PostOffice::instance()->GetCurrentTaskStatus().IsAnyException() )
        {
            AsyncWorker::create( SortBegin    , SortEnd , nullptr );
        }
        else
        {
            cout << "###########[WARNING] process meet exception" << endl;
        }
    }

    static  void    SortBegin   ( AsyncWorker* asyncWorker )
    {
        for ( auto& item : PostOffice::instance()->GetCurrentTaskStatus().block_map )
        {
            auto temp = new string( item.first );
            AsyncWorker::create ( SortOneBegin, SortOneEnd, temp );
        }
        while ( !PostOffice::instance()->GetCurrentTaskStatus().IsAllSorted() )
        {
            this_thread::sleep_for( chrono::milliseconds( ExecutorConfig::instance()->sortWait ) );
        }
    }

    static  void    SortEnd     ( AsyncWorker* asyncWorker )
    {
        cout << "############### sort all started" << endl;
        AsyncWorker::create( InspectOn    , InspectOff , nullptr );
    }

    static  void    SortOneBegin( AsyncWorker* asyncWorker )
    {
        auto temp = static_cast< string* > ( asyncWorker->data() );
        MaratonCommon::AnalysisHelper analysisHelper;

        uv_mutex_lock   ( &taskStatusMutex );
        PostOffice::instance()->GetCurrentTaskStatus().SetBlockStatus(*temp ,MaratonCommon::TaskStatus::BlockStatus::kSorting );
        uv_mutex_unlock ( &taskStatusMutex );

        // analysisHelper.CheckEnviroment();
        auto exitStatus=  analysisHelper.SortData( *temp );

        uv_mutex_lock  ( &taskStatusMutex );
        if( 0 == exitStatus )
        {
            PostOffice::instance()->GetCurrentTaskStatus().SetBlockStatus(*temp ,MaratonCommon::TaskStatus::BlockStatus::kSorted );
        }
        else
        {
            PostOffice::instance()->GetCurrentTaskStatus().SetBlockStatus(*temp ,MaratonCommon::TaskStatus::BlockStatus::kException );
        }
        cout << exitStatus <<"#########file Sorted " << *temp << endl;
        uv_mutex_unlock( &taskStatusMutex );

        delete temp;
        temp = nullptr;
    }

    static  void    SortOneEnd  ( AsyncWorker* asyncWorker )
    {

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
                if ( MaratonCommon::TaskStatus::BlockStatus::kSorted == item.second )
                {
                    fileName = item.first;
                    ExecutorConfig::instance()->GetFileName( fileName, ExecutorConfig::SuffixType::kOutPutFile );
                    NoFileToPush = false;

                    uv_mutex_lock  ( &taskStatusMutex );
                    item.second = MaratonCommon::TaskStatus::BlockStatus::kPushing;
                    uv_mutex_unlock( &taskStatusMutex );

                    FileUploader uploader;
                    uploader.UploadFileViaHttp( PostOffice::instance()->GetCurrentTaskStatus().task_id
                            , ExecutorConfig::instance()->outputDir + fileName
                            , ExecutorConfig::instance()->postBam );

                    uv_mutex_lock  ( &taskStatusMutex );
                    item.second = MaratonCommon::TaskStatus::BlockStatus::kPushed;
                    uv_mutex_unlock( &taskStatusMutex );

                    //TODO handle push exception and reliable result deliver
                    uv_mutex_lock  ( &taskStatusMutex );
                    item.second = MaratonCommon::TaskStatus::BlockStatus::kFinished;
                    uv_mutex_unlock( &taskStatusMutex );

                    break;
                }
            }

            if( NoFileToPush )
            {
                this_thread::sleep_for( chrono::milliseconds( ExecutorConfig::instance()->processWait ) );
            }
        }
    }

    static  void    InspectOff  ( AsyncWorker* asyncWorker )
    {
#ifdef DEBUG_ONLY
        uv_mutex_lock   ( &clockMutex );
        end_at  = chrono::steady_clock::now();
        cout
            << endl
            << "############ Task    time : "
            << chrono::duration_cast< chrono::milliseconds >( end_at - start_at ).count()
            << " ms"
            << endl
            << "############ Process time : "
            << chrono::duration_cast< chrono::milliseconds >( proceess_end_at - proceess_start_at ).count()
            << " ms "
            << endl;

        uv_mutex_unlock ( &clockMutex );
#endif
        MessageTaskResult msgout;

        PostOffice::instance()->self_status = PostOffice::ExcutorSates::kTaskFinished;
        PostOffice::instance()->SendSelfStatus();

        cout << endl << "############ Task aligned and sorted" << endl;

        msgout.task_id( PostOffice::instance()->current_task );
        if ( PostOffice::instance()->GetCurrentTaskStatus().IsAnyException() )
        {
            msgout.error( 1 );
            msgout.stage( ExecutorConfig::instance()->TaskStage::kUnknown );
            msgout.result( "[ EXCEPTION ]: At least one block in the list cannot be processed" );
            cout <<endl << "######### but At least one block in the list cannot be processed " << endl;
        }
        else
        {
            msgout.error( 0 );
            msgout.stage ( ExecutorConfig::instance()->TaskStage::kAligned );
            msgout.result( "OK! Aligment finished!" );
        }

        PostOffice::instance()->SendMail( &msgout );

        PostOffice::instance()->self_status = PostOffice::ExcutorSates::kStandby;
        PostOffice::instance()->SendSelfStatus();
        cout << endl << "############ Standby" << endl;
    }

} // End of namespace Protocol
#endif // !Message_Task_Deliver_HANDLER_HPP_
