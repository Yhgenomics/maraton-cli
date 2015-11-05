#ifndef MESSAGE_TASK_DELIVER_HANDLER_HPP_
#define MESSAGE_TASK_DELIVER_HANDLER_HPP_

#include "MessageTaskDeliver.hpp"
#include "MessageTaskDeliverACK.hpp"
#include "PostOffice.h"
#include <vector>
#include <string>
#include "FileDownloader.h"

namespace Protocol
{
    static int MessageTaskDeliverHandler( MessageTaskDeliver msg )
    {
        // UserDefineHandler Begin
        // Your Codes here!
		//MessageTaskDeliverACK messageout;
		//messageout.task_id( msg.task_id() );
		//messageout.result( "OK begin to run, wait for a Status Report!" );
		//PostOffice::instance()->SendMail( &messageout );
		
		if ( PostOffice::instance()->self_status == "3" )
		{
			PostOffice::instance()->self_status = "4";
			PostOffice::instance()->SendSelfStatus();
			bool cancel;
			for ( auto item : msg.uri_list() )
			{
				cancel = false;
				FileDownloader fileDownloader( &cancel );
				fileDownloader.DownloadViaHTTP( "E:\\001somefile" , item );
			}

			PostOffice::instance()->self_status = "5";
			PostOffice::instance()->SendSelfStatus();
			cout << "pretent to do some job" << endl;

			PostOffice::instance()->self_status = "3";
			PostOffice::instance()->SendSelfStatus();
			cout << "pretent job done" << endl;
		}
		
	
		


        return 0;
        // UserDefineHandler End 
    }
    
} // End of namespace Protocol
#endif // !Message_Task_Deliver_HANDLER_HPP_
