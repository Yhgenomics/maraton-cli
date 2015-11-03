#ifndef MESSAGE_TASK_DELIVER_HANDLER_HPP_
#define MESSAGE_TASK_DELIVER_HANDLER_HPP_

#include "MessageTaskDeliver.hpp"
#include "MessageTaskDeliverACK.hpp"
#include "PostOffice.h"

namespace Protocol
{
    static int MessageTaskDeliverHandler( MessageTaskDeliver msg )
    {
        // UserDefineHandler Begin
        // Your Codes here!
		MessageTaskDeliverACK messageout;

		messageout.task_id( msg.task_id() );
		messageout.result( "OK begin to run, wait for a Status Report!" );

		PostOffice::instance()->SendMail( &messageout );

        return 0;
        // UserDefineHandler End 
    }
    
} // End of namespace Protocol
#endif // !Message_Task_Deliver_HANDLER_HPP_
