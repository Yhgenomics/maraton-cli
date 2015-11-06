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
			//SysProcess taskExecutor( "echo" , "123" , [] ( size_t i ) { cout << "finished" << endl; } );
			//taskExecutor.wait_for_exit();

			PostOffice::instance()->self_status = "3";
			PostOffice::instance()->SendSelfStatus();
			cout << "pretent job done" << endl;
		}

        return 0;
        // UserDefineHandler End 
    }
    
} // End of namespace Protocol
#endif // !Message_Task_Deliver_HANDLER_HPP_
