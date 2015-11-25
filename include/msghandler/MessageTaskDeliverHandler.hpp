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

namespace Protocol
{
    static  void    ProcessBegin( AsyncWorker* asyncWorker );
    static  void    ProcessEnd( AsyncWorker* asyncWorker );

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
			for ( auto item : msg.uri_list() )
            {
				cancel = false;
				FileDownloader fileDownloader( &cancel );
				fileDownloader.DownloadViaHTTP( OrderParams.inputDir + OrderParams.taskid+ OrderParams.fqTail , item );
				std::cout << "File append to " << OrderParams.inputDir + OrderParams.taskid + OrderParams.fqTail << endl;
            }

			PostOffice::instance()->self_status = PostOffice::ExcutorSates::kComputing;
			PostOffice::instance()->SendSelfStatus();
#ifdef _WIN32
			auto    originalMsg = new MessageTaskDeliver( msg );
			auto    taskProcess = AsyncWorker::create( ProcessBegin , ProcessEnd , static_cast<void *>( originalMsg ) );
#else
            auto    indata      = new MessageTaskDeliver( msg );
			auto    taskProcess = AsyncWorker::create( ProcessBegin , ProcessEnd , static_cast<void *>( indata ) );
#endif
        }

        return 0;
        // UserDefineHandler End
    }
#ifdef _WIN32

	static  void    ProcessBegin( AsyncWorker* asyncWorker )
    {
		cout << "ProcessBegin" << endl;
		auto originalMsg = static_cast< MessageTaskDeliver* >( asyncWorker->data() );
		string shellCmd;
		OrderMakerParams OrderParams( originalMsg->task_id() );

		MessageTaskProgress taskProgress;
		taskProgress.task_id( originalMsg->task_id() );

		taskProgress.progress( 0 );
		PostOffice::instance()->SendMail( &taskProgress );

		shellCmd = OrderMaker::instance()->MakeSamOrder( OrderParams );
		cout << "cmd is " << shellCmd << endl;;
		system( shellCmd.c_str() );
		taskProgress.progress( 60 );
		PostOffice::instance()->SendMail( &taskProgress );
    }

	static  void    ProcessEnd( AsyncWorker* asyncWorker )
    {
		auto originalMsg = static_cast< MessageTaskDeliver* >( asyncWorker->data() );

		MessageTaskResult msgout;
		PostOffice::instance()->self_status = PostOffice::ExcutorSates::kUploading;
		PostOffice::instance()->SendSelfStatus();
		FileUploader uploader;
		OrderMakerParams OrderParams( originalMsg->task_id() );
		uploader.UploadFileViaHttp( OrderParams.taskid , OrderParams.workdir + OrderParams.taskid + OrderParams.samTail , OrderParams.postDest );

		MessageTaskProgress taskProgress;
		taskProgress.task_id( originalMsg->task_id() );
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
#else
	static  void    ProcessBegin( AsyncWorker* asyncWorker )
    {
		cout<<"ProcessBegin"<<endl;

       	auto originalMsg = static_cast< MessageTaskDeliver* >( asyncWorker->data() );

        MaratonCommon::AnalysisHelper analysisHelper;
        analysisHelper.CheckEnviroment();
        asyncWorker->result(new size_t());
        auto exitStatus= new size_t( analysisHelper.ProcessData( 3, originalMsg->reference(),originalMsg->task_id()+".fastq" ) );
        asyncWorker->result( exitStatus );
        return;
    }

    static  void    ProcessEnd( AsyncWorker* asyncWorker )
    {
    	auto exitStatus  = static_cast< size_t* >( asyncWorker->result() );
        if( 0 != *exitStatus )
        {
            std::cout << "exit code is " << *exitStatus << std::endl;
            return;
        }
        auto originalMsg = static_cast< MessageTaskDeliver* >( asyncWorker->data() );

        MessageTaskProgress taskProgress;
        taskProgress.task_id( originalMsg->task_id() );
        taskProgress.progress(60);
        PostOffice::instance()->SendMail( &taskProgress );
        MessageTaskResult msgout;
		PostOffice::instance()->self_status = PostOffice::ExcutorSates::kUploading;
		PostOffice::instance()->SendSelfStatus();
		FileUploader uploader;
		OrderMakerParams OrderParams( originalMsg->task_id() );
	    uploader.UploadFileViaHttp( OrderParams.taskid , OrderParams.outputDir + OrderParams.taskid +".fastq.sam" , OrderParams.postDest );

        taskProgress.progress(100);
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
#endif

} // End of namespace Protocol
#endif // !Message_Task_Deliver_HANDLER_HPP_
