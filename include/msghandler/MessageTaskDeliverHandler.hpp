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

namespace Protocol
{
#ifdef _WIN32
    const string separator		= " ";
	const string workdir		= "E:\\GeneData\\";
	const string phaseOneFlags	= "aln -t 8 -f";
	const string phaseTwoFlags	= "samse -f";
	const string aligner		= "BWA.exe";
	const string refGen			= "hg19.fa";
	const string saiTail		= ".sai";
	const string fqTail			= ".fastq";
	const string samTail		= ".sam";
	const string postDest		= "http://10.0.0.20/file/upload_result";
#else
    const string separator		= " ";
	const string workdir		= "/home/ubuntu/GeneData/";
	const string phaseOneFlags	= "aln -t 3 -f";
	const string phaseTwoFlags	= "samse -f";
	const string aligner		= "BWA.exe";
	const string refGen			= "hg19.fa";
	const string saiTail		= ".sai";
	const string fqTail			= ".fastq";
	const string samTail		= ".sam";
	const string postDest		= "http://10.0.0.20/file/upload_result";

#endif

	static void BWAPhaseOneCallBack( SysProcess* sysProcess , size_t result );
	static void BWAPhaseTwoCallBack( SysProcess* sysProcess , size_t result );

	static int MessageTaskDeliverHandler( MessageTaskDeliver msg )
    {
        // UserDefineHandler Begin
        // Your Codes here!
		if ( PostOffice::instance()->self_status == 3 )
		{
			PostOffice::instance()->self_status = PostOffice::ExcutorSates::kTaskDataPreparing;            PostOffice::instance()->SendSelfStatus();

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
			auto bwaPhase1 = SysProcess::create(   workdir		+ aligner
												 , phaseOneFlags+ separator
												 + workdir		+ msg.task_id() + saiTail	+ separator
												 + workdir		+ refGen		+ separator
												 + workdir		+ msg.task_id() + fqTail
												 , workdir
												 , BWAPhaseOneCallBack );
			auto originalMsg = new MessageTaskDeliver( msg );
			bwaPhase1->data( originalMsg );
			bwaPhase1->start();
#else
            string geneDir      = "/home/ubuntu/GeneData/";
            string bwaFullName  = "/home/ubuntu/DreamLand/bwa-0.7.12/bwa";
            string refFullName  = "/home/ubuntu/GeneData/hg19.fa";
            string bwaCmdFlag   = "mem -t 3";
            string firstcmd     = bwaFullName   + separator
                                + bwaCmdFlag    + separator
                                + refFullName   + separator
                                + geneDir       + msg.task_id() + fqTail    + separator
                                + ">"           + separator
                                + geneDir       + msg.task_id() + samTail;
		    PostOffice::instance()->self_status = PostOffice::ExcutorSates::kComputing;
		    PostOffice::instance()->SendSelfStatus();
            cout<<endl<<endl<<firstcmd<<endl;
	        system(firstcmd.c_str());

            string samtoolsName = "samtools";
            string samViewFlag  = "view -S "    + geneDir       + msg.task_id() + samTail + separator
                                + "-t "         + refFullName   + ".fai"        + " -b > "
                                + geneDir+msg.task_id()         + ".bam";
            string secondcmd    = samtoolsName  + separator
                                + samViewFlag;

            cout<<endl<<endl<<secondcmd<<endl;
            system(secondcmd.c_str());
            string bamsortFlag  = "sort -m 2000000000 "         + geneDir+msg.task_id() + ".bam"    + separator
                                + geneDir       + msg.task_id() + ".sorted";
            string thridcmd     = samtoolsName  + separator     + bamsortFlag;
	        cout <<endl<<endl<<thridcmd<<endl;
            system(thridcmd.c_str());

		    PostOffice::instance()->self_status = PostOffice::ExcutorSates::kUploading;
		    PostOffice::instance()->SendSelfStatus();
            FileUploader uploader;
		    uploader.UploadFileViaHttp( msg.task_id() , geneDir + msg.task_id() + ".sorted.bam" , postDest );

            PostOffice::instance()->self_status = PostOffice::ExcutorSates::kTaskFinished;
		    PostOffice::instance()->SendSelfStatus();

		    cout << "Task done" << endl;

		    MessageTaskResult msgout;
            msgout.task_id( msg.task_id() );
		    PostOffice::instance()->SendMail( &msgout );


            PostOffice::instance()->self_status = PostOffice::ExcutorSates::kStandby;
		    PostOffice::instance()->SendSelfStatus();

#endif
		}

        return 0;
        // UserDefineHandler End
    }

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

		cout << "Task done" << endl;

		msg.task_id( originalMsg->task_id() );
		PostOffice::instance()->SendMail( &msg );

		delete originalMsg;
		originalMsg = nullptr;

		PostOffice::instance()->self_status = PostOffice::ExcutorSates::kStandby;
		PostOffice::instance()->SendSelfStatus();
	}

} // End of namespace Protocol
#endif // !Message_Task_Deliver_HANDLER_HPP_
