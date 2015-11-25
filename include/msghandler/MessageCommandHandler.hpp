#ifndef MESSAGE_COMMAND_HANDLER_HPP_
#define MESSAGE_COMMAND_HANDLER_HPP_

#include "MessageCommand.hpp"
#include <iostream>
#include "MessageStatusReport.hpp"
#include "PostOffice.h"

namespace Protocol
{
    static int MessageCommandHandler( MessageCommand msg )
    {
        // UserDefineHandler Begin
        // Your Codes here!
        cout << "cmd in " << msg.command_line() << endl;
        MessageStatusReport messageout;
        messageout.reports( 4 ); //Prepare
        PostOffice::instance()->SendMail( &messageout );
        cout << "it is executing" << endl;

        messageout.reports( 5 ); //Working
        PostOffice::instance()->SendMail( &messageout );
        cout << "job done" << endl;

        messageout.reports( 3 );
        PostOffice::instance()->SendMail( &messageout );

        return 0;
        // UserDefineHandler End 
    }

} // End of namespace Protocol
#endif // !Message_Command_HANDLER_HPP_
