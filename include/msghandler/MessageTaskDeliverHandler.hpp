#ifndef MESSAGE_TASK_DELIVER_HANDLER_HPP_
#define MESSAGE_TASK_DELIVER_HANDLER_HPP_

#include "MessageTaskDeliver.hpp"
#include "MessageTaskDeliverACK.hpp"
#include "PostOffice.h"
#include <vector>
#include <string>
#include "FileDownloader.h"
#include "SysProcess.h"

namespace Protocol
{
    static int MessageTaskDeliverHandler( MessageTaskDeliver msg )
    {
        // UserDefineHandler Begin
        // Your Codes here!	
		string workdir = "E:\\GeneData\\";
		if ( PostOffice::instance()->self_status == "3" )
		{
			PostOffice::instance()->self_status = "4";
			PostOffice::instance()->SendSelfStatus();
			bool cancel;
			for ( auto item : msg.uri_list() )
			{
				cancel = false;
				FileDownloader fileDownloader( &cancel );
				fileDownloader.DownloadViaHTTP( workdir +msg.task_id()+".fastq" , item );
				cout << "File append to " << workdir + msg.task_id() + ".fastq" << endl;
			}

			PostOffice::instance()->self_status = "5";
			PostOffice::instance()->SendSelfStatus();
			
			
			system( ( workdir + "BWA.exe aln -t 8 -f " + workdir + msg.task_id() + ".sai " + workdir + "hg19.fa " + workdir + msg.task_id() + ".fastq" ).c_str() );
			system( ( workdir + "BWA.exe samse -f " + workdir + msg.task_id() + ".sam " + workdir + "hg19.fa " + workdir + msg.task_id() + ".sai " + workdir + msg.task_id() + ".fastq" ).c_str() );

			//This Part is still need for the refactor of SysProcess at the framework
			//printf( "BWAPhaseTwo:%s\r\n" , ("aln -t 8 -f " + workdir + msg.task_id() + ".sai " + workdir + "hg19.fa " + workdir + msg.task_id() + ".fastq" ).c_str() );
			//SysProcess BWAPhaseOne( workdir + "BWA.exe" , "aln -t 8 -f " + workdir + msg.task_id() + ".sai " + workdir + "hg19.fa " + workdir + msg.task_id() + ".fastq" , workdir ,
			//						[] ( size_t  i ) 
			//{
			//	//cout << "BWA Phase One return " << i << endl;
			//} );
			//BWAPhaseOne.wait_for_exit();

			//printf( "BWAPhaseTwo:%s\r\n" , ("samse -f " + workdir + msg.task_id() + ".sam " + workdir + "hg19.fa " + workdir + msg.task_id() + ".sai " + workdir + msg.task_id() + ".fastq").c_str() );
			//SysProcess BWAPhaseTwo( workdir + "BWA.exe" ,"samse -f " + workdir + msg.task_id() + ".sam " + workdir + "hg19.fa " + workdir + msg.task_id() + ".sai " + workdir + msg.task_id() + ".fastq" , workdir , 
			//						[] ( size_t i )
			//{
			//	//cout << "BWA Phase Two return " << i << endl;
			//} );
			//BWAPhaseTwo.wait_for_exit();

			PostOffice::instance()->self_status = "3";
			PostOffice::instance()->SendSelfStatus();
			cout << "Task "<< msg.task_id() << " done" << endl;
		}

        return 0;
        // UserDefineHandler End 
    }
    
} // End of namespace Protocol
#endif // !Message_Task_Deliver_HANDLER_HPP_
