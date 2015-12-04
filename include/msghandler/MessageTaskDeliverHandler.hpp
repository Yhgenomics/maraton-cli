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
    static  void    ProcessBegin( AsyncWorker* asyncWorker );
    static  void    ProcessEnd  ( AsyncWorker* asyncWorker );

    static  void    PullBegin   ( AsyncWorker* asyncWorker );
    static  void    PullEnd     ( AsyncWorker* asyncWorker );
    static  void    ProcessBegin( AsyncWorker* asyncWorker );
    static  void    ProcessEnd  ( AsyncWorker* asyncWorker );
    static  void    PushBegin   ( AsyncWorker* asyncWorker );
    static  void    PushEnd     ( AsyncWorker* asyncWorker );
    static  void    InspectOn   ( AsyncWorker* asyncWorker );
    static  void    InspectOff  ( AsyncWorker* asyncWorker );
    static  bool    GetFileName ( const string &uri, string &fileName );
    static  uv_mutex_t taskStatusMutex;
    static  uv_mutex_t outputMutex;

    static  int     MessageTaskDeliverHandler( MessageTaskDeliver msg )
    {
        // UserDefineHandler Begin
        // Your Codes here!
        if ( PostOffice::instance()->self_status == 3 )
        {
            PostOffice::instance()->self_status = PostOffice::ExcutorSates::kTaskDataPreparing;
            PostOffice::instance()->SendSelfStatus();

            bool cancel;
            // OrderMakerParams OrderParams( msg.task_id() );

            PostOffice::instance()->current_task                                = msg.task_id();
            PostOffice::instance()->task_board[ msg.task_id() ].task_id         = msg.task_id();
            PostOffice::instance()->task_board[ msg.task_id() ].ref_gen_name    = msg.reference();

            for ( auto uri : msg.uri_list() )
            {
                PostOffice::instance()->task_board[ msg.task_id() ].block_map[ uri ] = TaskStatus::BlockStatus::kWait;
            }

            auto pullLooper     = AsyncWorker::create( PullBegin    , PullEnd    , nullptr );
            auto processLoooper = AsyncWorker::create( ProcessBegin , ProcessEnd , nullptr );
            auto inspecter      = AsyncWorker::create( InspectOn    , InspectOff , nullptr );
        }
        return 0;
        // UserDefineHandler End
    }

    static  void    PullBegin   ( AsyncWorker* asyncWorker )
    {
        cout << "Pull Begin" << endl;
        bool allPulled = false;

        for ( auto& item : PostOffice::instance()->GetCurrentTaskStatus().block_map)
        {
            uv_mutex_lock  ( &taskStatusMutex );
            item.second = MaratonCommon::TaskStatus::BlockStatus::kPulling;
            uv_mutex_unlock( &taskStatusMutex );

            string fileName ;
            ExecutorConfig::instance()->GetFileName( item.first, fileName, ExecutorConfig::SuffixType::kInputFile );
            cout<<"fileName "<< fileName   << endl;
            cout<< "from "   << item.first << endl;

            bool cancel = false;
            FileDownloader fileDownloader( &cancel );
            fileDownloader.DownloadViaHTTP( ExecutorConfig::instance()->inputDir + fileName  , item.first );
            std::cout << "File download to" << ExecutorConfig::instance()->inputDir << fileName << endl;

            uv_mutex_lock  ( &taskStatusMutex );
            item.second = MaratonCommon::TaskStatus::BlockStatus::kPulled;
            uv_mutex_unlock( &taskStatusMutex );
        }

    }

    static  void    PullEnd     ( AsyncWorker* asyncWorker )
    {
        cout << "pull all down!"<< endl;
    }

    static  void    ProcessBegin( AsyncWorker* asyncWorker )
    {
        while ( !PostOffice::instance()->GetCurrentTaskStatus().IsAllProcessed() )
        {
            bool NoFileToProcess = true;
            string fileName;

            for ( auto& item : PostOffice::instance()->GetCurrentTaskStatus().block_map )
            {
                if( MaratonCommon::TaskStatus::BlockStatus::kPulled == item.second )
                {
                    ExecutorConfig::instance()->GetFileName( item.first, fileName, ExecutorConfig::SuffixType::kInputFile );
                    NoFileToProcess == false;

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

            if( NoFileToProcess && !PostOffice::instance()->GetCurrentTaskStatus().IsAllProcessed())
            {
                cout<<"no file to process yet"<<endl;
                std::this_thread::sleep_for(std::chrono::milliseconds( ExecutorConfig::instance()->transWait ));
            }

        }

    }

    static  void    ProcessEnd  ( AsyncWorker* asyncWorker )
    {
        cout << "process All Down "<< endl;
        cout << "any exception ? " << PostOffice::instance()->GetCurrentTaskStatus().IsAnyException();
    }

    static  void    PushBegin   ( AsyncWorker* asyncWorker )
    {

    }

    static  void    PushEnd     ( AsyncWorker* asyncWorker )
    {

    }

    static  void    InspectOn   ( AsyncWorker* asyncWorker )
    {
        while ( !PostOffice::instance()->GetCurrentTaskStatus().IsAllFinished() )
        {
            if ( PostOffice::instance()->GetCurrentTaskStatus().IsAnyException() )
            {
                // TODO REPORT ERROR
                break;
            }

            bool NoFileToPush = true;
            string fileName;

            for ( auto& item : PostOffice::instance()->GetCurrentTaskStatus().block_map )
            {
                if( MaratonCommon::TaskStatus::BlockStatus::kProcessed == item.second )
                {
                    ExecutorConfig::instance()->GetFileName( item.first, fileName, ExecutorConfig::SuffixType::kOutPutFile );
                    NoFileToPush == false;

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

                    //TODO push exception handle
                    uv_mutex_lock  ( &taskStatusMutex );
                    item.second = MaratonCommon::TaskStatus::BlockStatus::kFinished;
                    uv_mutex_unlock( &taskStatusMutex );

                    cout << "file pushed " << fileName;
                    break;
                }
            }

            if( NoFileToPush )
            {
                cout<<"###################no file to push yet"<<endl;
                std::this_thread::sleep_for(std::chrono::milliseconds( ExecutorConfig::instance()->processWait ));
            }

        }

    }

    static  void    InspectOff  ( AsyncWorker* asyncWorker )
    {
        cout<<"###################task over "<<endl;

        MessageTaskResult msgout;

        PostOffice::instance()->self_status = PostOffice::ExcutorSates::kTaskFinished;
        PostOffice::instance()->SendSelfStatus();

        std::cout << "Task done" << std::endl;

        msgout.task_id( PostOffice::instance()->current_task );
        if ( PostOffice::instance()->GetCurrentTaskStatus().IsAnyException() )
        {
            msgout.error( 1 );
            msgout.result( "At least one block in the list cannot be processed" );

        }
        PostOffice::instance()->SendMail( &msgout );

        PostOffice::instance()->self_status = PostOffice::ExcutorSates::kStandby;
        PostOffice::instance()->SendSelfStatus();
        cout << "Standby" << endl;
    }


    static  bool    GetFileName ( const string &uri, string &fileName )
    {
        int pos = uri.find_last_of( '/' );
        if ( pos == string::npos )
            return false;
        fileName= uri.substr( pos + 1, uri.length() - pos - 1 );
        return true;
    }

    static  bool    debugOnly()
    {
        cout<<" debug only info "<<endl;

        uv_mutex_lock   ( &taskStatusMutex );
        for ( auto task : PostOffice::instance()->task_board )
        {
           for ( auto block : task.second.block_map )
           {
               cout << "block name : "  << block.first   << endl;
               cout << "block statue : "<< block.second  << endl;
           }
        }
        uv_mutex_unlock ( &taskStatusMutex );
    }

    /*
    static  void    ProcessBegin( AsyncWorker* asyncWorker )
    {
        cout << "ProcessBegin" << endl;

        auto originalMsg = static_cast< MessageTaskDeliver* >( asyncWorker->data() );

        MaratonCommon::AnalysisHelper analysisHelper;
        analysisHelper.CheckEnviroment();
        asyncWorker->result( new size_t() );
        auto exitStatus= new size_t( analysisHelper.ProcessData( 3 , originalMsg->reference() , originalMsg->task_id() + ".fastq" ) );
        asyncWorker->result( exitStatus );
        return;
    }

    static  void    ProcessEnd( AsyncWorker* asyncWorker )
    {
        auto exitStatus  = static_cast< size_t* >( asyncWorker->result() );
        if ( 0 != *exitStatus )
        {
            std::cout << "exit code is " << *exitStatus << std::endl;
            return;
        }
        auto originalMsg = static_cast< MessageTaskDeliver* >( asyncWorker->data() );

        MessageTaskProgress taskProgress;
        taskProgress.task_id( originalMsg->task_id() );
        taskProgress.progress( 60 );
        PostOffice::instance()->SendMail( &taskProgress );
        MessageTaskResult msgout;
        PostOffice::instance()->self_status = PostOffice::ExcutorSates::kUploading;
        PostOffice::instance()->SendSelfStatus();
        FileUploader uploader;
        OrderMakerParams OrderParams( originalMsg->task_id() );
        uploader.UploadFileViaHttp( OrderParams.taskid , OrderParams.outputDir + OrderParams.taskid + ".fastq.sam" , OrderParams.postDest );

        taskProgress.progress( 100 );
        PostOffice::instance()->SendMail( &taskProgress );

        PostOffice::instance()->self_status = PostOffice::ExcutorSates::kTaskFinished;
        PostOffice::instance()->SendSelfStatus();

        std::cout << "Task done" << std::endl;

        msgout.task_id( originalMsg->task_id() );
        PostOffice::instance()->SendMail( &msgout );

        PostOffice::instance()->self_status = PostOffice::ExcutorSates::kStandby;
        PostOffice::instance()->SendSelfStatus();

        delete originalMsg;
        originalMsg = nullptr;
        asyncWorker->data( nullptr );
        cout << "Standby" << endl;
    }
*/

} // End of namespace Protocol
#endif // !Message_Task_Deliver_HANDLER_HPP_
