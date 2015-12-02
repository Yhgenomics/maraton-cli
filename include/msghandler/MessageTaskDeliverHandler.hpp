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
#include "OrderMaker.hpp"
#include "AsyncWorker.h"
#include "MessageTaskProgress.hpp"
#include "AnalysisHelper.h"
#include "uv.h"
#include <unistd.h>

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
            OrderMakerParams OrderParams( msg.task_id() );
            PostOffice::instance()->current_task = msg.task_id();

            for ( auto uri : msg.uri_list() )
            {
                PostOffice::instance()->task_board[ msg.task_id() ].block_map[ uri ] = TaskStatus::BlockStatus::WAIT;
            }

            // refacotr 2015 12 02 to block sequence process
            // auto pullData       = new vector< std::string >( msg.uri_list() );

            auto pullLooper     = AsyncWorker::create( PullBegin    , PullEnd   , nullptr );

           // auto processLoooper = AsyncWorker::create( ProcessBegin , ProcessEnd, nullptr );

           // auto pushLooper     = AsyncWorker::create( PushBegin    , PushEnd   , nullptr );

            return 0;
            for ( auto item : msg.uri_list() )
            {
                cancel = false;
                FileDownloader fileDownloader( &cancel );
                fileDownloader.DownloadViaHTTP( OrderParams.inputDir + OrderParams.taskid + OrderParams.fqTail , item );
                std::cout << "File append to " << OrderParams.inputDir + OrderParams.taskid + OrderParams.fqTail << endl;
            }

            PostOffice::instance()->self_status = PostOffice::ExcutorSates::kComputing;
            PostOffice::instance()->SendSelfStatus();
          //  auto    indata      = new MessageTaskDeliver( msg );
          //  auto    taskProcess = AsyncWorker::create( ProcessBegin , ProcessEnd , static_cast< void * >( indata ) );
        }

        return 0;
        // UserDefineHandler End
    }

    static  void    PullBegin   ( AsyncWorker* asyncWorker )
    {
        cout << "Pull Begin" << endl;
        bool allPulled = false;

        uv_mutex_lock  ( &taskStatusMutex );
        allPulled = PostOffice::instance()->GetCurrentTaskStatus().IsAllPulled();
        uv_mutex_unlock( &taskStatusMutex );

        for ( auto& item : PostOffice::instance()->GetCurrentTaskStatus().block_map)
        {
            cout<< item.first  << endl;
            cout<< item.second << endl;
            string uri = item.first;
            int pos = uri.find_last_of('/');
            string fileName = uri.substr( pos + 1, uri.length() - pos - 1  );

            bool cancel = false;
            FileDownloader fileDownloader( &cancel );
            fileDownloader.DownloadViaHTTP( "/data/input/"+fileName  , item.first );
            std::cout << "File download to /data/input/ " << fileName << endl;
            uv_mutex_lock  ( &taskStatusMutex );
            item.second = MaratonCommon::TaskStatus::BlockStatus::PULLED;
            uv_mutex_unlock( &taskStatusMutex );

        }

        uv_mutex_lock  ( &taskStatusMutex );
        allPulled = PostOffice::instance()->GetCurrentTaskStatus().IsAllPulled();
        uv_mutex_unlock( &taskStatusMutex );

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

    static  void    PullEnd     ( AsyncWorker* asyncWorker )
    {

    }

    static  void    ProcessBegin( AsyncWorker* asyncWorker )
    {

    }
    static  void    ProcessEnd  ( AsyncWorker* asyncWorker )
    {

    }
    static  void    PushBegin   ( AsyncWorker* asyncWorker )
    {

    }

    static  void    PushEnd     ( AsyncWorker* asyncWorker )
    {

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
