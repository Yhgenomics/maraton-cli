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

namespace Protocol
{
	const string separator	= " ";
	const string refGen		= "hg19.fa";
	const string saiTail	= ".sai";
	const string fqTail		= ".fastq";
	const string samTail	= ".sam";
	const string bamTail	= ".bam";
	const string sortedTail = ".sorted";
	const string postDest	= "http://10.0.0.20/file/upload_result";

#ifdef _WIN32 
	const string workdir		= "E:\\GeneData\\";
	const string phaseOneFlags	= "aln -t 8 -f";
	const string phaseTwoFlags	= "samse -f";
	const string aligner		= "BWA.exe";

#else
	const string workdir		= "/home/ubuntu/GeneData/maratonworkspace/";
	const string aligner		= "bwa";
#endif

#ifdef _WIN32
	static void BWAPhaseOneCallBack( SysProcess* sysProcess , size_t result );
	static void BWAPhaseTwoCallBack( SysProcess* sysProcess , size_t result );
#else
	static void ProcessBegin( AsyncWorker* asyncWorker );
	static void ProcessEnd( AsyncWorker* asyncWorker );
#endif

	static int MessageTaskDeliverHandler( MessageTaskDeliver msg )
    {
        // UserDefineHandler Begin
        // Your Codes here!
		
		if ( PostOffice::instance()->self_status == 3 )
		{
			PostOffice::instance()->self_status = PostOffice::ExcutorSates::kTaskDataPreparing;
			PostOffice::instance()->SendSelfStatus();

            bool cancel;
			for ( auto item : msg.uri_list() )
			{
				cancel = false;
				FileDownloader fileDownloader( &cancel );
				fileDownloader.DownloadViaHTTP( workdir +msg.task_id()+ fqTail , item );
				std::cout << "File append to " << workdir + msg.task_id() + fqTail << endl;
			}

			PostOffice::instance()->self_status = PostOffice::ExcutorSates::kComputing;
			PostOffice::instance()->SendSelfStatus();
#ifdef _WIN32
			auto bwaPhase1 = SysProcess::create(  workdir		+ aligner
												, phaseOneFlags	+ separator
												+ workdir		+ msg.task_id() + saiTail	+ separator
												+ workdir		+ refGen		+ separator
												+ workdir		+ msg.task_id() + fqTail
												, workdir
												, BWAPhaseOneCallBack );
			auto originalMsg = new MessageTaskDeliver( msg );
			bwaPhase1->data( originalMsg );
			bwaPhase1->start();
			
#else
			string *indata = new string( msg.task_id() );
			auto test1 = AsyncWorker::create( ProcessBegin , ProcessEnd , static_cast<void *>( indata ) );
			/*
			string shellCmd;
			OrderMakerParams OrderParams( msg.task_id() );
			shellCmd = OrderMaker::instance()->MakePipeline( OrderParams );

            system(shellCmd.c_str());

			MessageTaskResult msgout;
            PostOffice::instance()->self_status = PostOffice::ExcutorSates::kUploading;
		    PostOffice::instance()->SendSelfStatus();
		    FileUploader uploader;
		    uploader.UploadFileViaHttp( msg.task_id() , workdir + msg.task_id() + sortedTail + bamTail , postDest );

		    PostOffice::instance()->self_status = PostOffice::ExcutorSates::kTaskFinished;
		    PostOffice::instance()->SendSelfStatus();

		    std::cout << "Task done" << std::endl;

		    msgout.task_id( msg.task_id() );
		    PostOffice::instance()->SendMail( &msgout );

		    PostOffice::instance()->self_status = PostOffice::ExcutorSates::kStandby;
		    PostOffice::instance()->SendSelfStatus();
			*/
#endif
		}

        return 0;
        // UserDefineHandler End
    }
#ifdef _WIN32
	static void BWAPhaseOneCallBack( SysProcess* sysProcess , size_t result )
	{
		MessageTaskDeliver* originalMsg = static_cast<MessageTaskDeliver*>( sysProcess->data() );

		auto bwaPhase2 = SysProcess::create(   workdir		+ aligner
											 , phaseTwoFlags+ separator
											 + workdir		+ originalMsg->task_id() + samTail + separator
											 + workdir		+ refGen				 + separator
											 + workdir		+ originalMsg->task_id() + saiTail + separator
											 + workdir		+ originalMsg->task_id() + fqTail
											 , workdir
											 , BWAPhaseTwoCallBack );
		bwaPhase2->data( originalMsg );
		bwaPhase2->start();

		std::cout << "BWA Phase 1 end with result code " << result << std::endl;
	}

	static void BWAPhaseTwoCallBack( SysProcess* sysProcess , size_t result )
	{
		MessageTaskResult msg;
		MessageTaskDeliver* originalMsg = static_cast<MessageTaskDeliver*>( sysProcess->data() );

		std::cout <<  "BWA Phase 2 end with result code " << result << std::endl;

		PostOffice::instance()->self_status = PostOffice::ExcutorSates::kUploading;
		PostOffice::instance()->SendSelfStatus();
		FileUploader uploader;
		uploader.UploadFileViaHttp( originalMsg->task_id() , workdir + originalMsg->task_id() + samTail , postDest );

		PostOffice::instance()->self_status = PostOffice::ExcutorSates::kTaskFinished;
		PostOffice::instance()->SendSelfStatus();

		std::cout << "Task done" << std::endl;

		msg.task_id( originalMsg->task_id() );
		PostOffice::instance()->SendMail( &msg );

		delete originalMsg;
		originalMsg = nullptr;

		PostOffice::instance()->self_status = PostOffice::ExcutorSates::kStandby;
		PostOffice::instance()->SendSelfStatus();
	}
#else
	static void ProcessBegin( AsyncWorker* asyncWorker )
	{
		string* taskid = static_cast< string* >( asyncWorker->data() );
		OrderMakerParams OrderParams( *taskid );
		string shellCmd = OrderMaker::instance()->MakePipeline( OrderParams );
		system( shellCmd.c_str() );
	}

	static void ProcessEnd( AsyncWorker* asyncWorker )
	{
		string* taskid = static_cast< string* >( asyncWorker->data() );

		MessageTaskResult msgout;
		PostOffice::instance()->self_status = PostOffice::ExcutorSates::kUploading;
		PostOffice::instance()->SendSelfStatus();
		FileUploader uploader;
		OrderMakerParams OrderParams( *taskid );
		uploader.UploadFileViaHttp( OrderParams.taskid , OrderParams.workdir + OrderParams.taskid + OrderParams.sortedTail + OrderParams.bamTail , OrderParams.postDest );

		PostOffice::instance()->self_status = PostOffice::ExcutorSates::kTaskFinished;
		PostOffice::instance()->SendSelfStatus();

		std::cout << "Task done" << std::endl;

		msgout.task_id( *taskid );
		PostOffice::instance()->SendMail( &msgout );

		PostOffice::instance()->self_status = PostOffice::ExcutorSates::kStandby;
		PostOffice::instance()->SendSelfStatus();

		delete taskid;
		taskid = nullptr;
		asyncWorker->data( nullptr );
		cout << "Standby" << endl;
	}
#endif

} // End of namespace Protocol
#endif // !Message_Task_Deliver_HANDLER_HPP_
