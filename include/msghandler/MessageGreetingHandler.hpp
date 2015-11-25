#ifndef MESSAGE_GREETING_HANDLER_HPP_
#define MESSAGE_GREETING_HANDLER_HPP_

#include "MessageGreeting.hpp"
#include "MessageIntro.hpp"
#include <iostream>
#include "PostOffice.h"
#include "ResourceDescriptor.h"
#include "MessageStatusReport.hpp"
namespace Protocol
{
    static int MessageGreetingHandler( MessageGreeting msg )
    {
        // UserDefineHandler Begin
        // Your Codes here!
        cout << "Excutor Receive the msg of Greeting" << endl;
        cout << "Preparing the SelfIntro" << endl;

        MessageIntro messageout;

        MaratonCommon::ResourceDescriptor resourceDescriptor;

        messageout.uuid( "Executor2Pro" );
        //messageout.free_disk(resourceDescriptor.GetFreeDiskSize( "MB" ));
        //messageout.free_memory(resourceDescriptor.GetFreeMemorySize( "MB" ));
        //messageout.process_100m ( 2000 );
        messageout.free_disk( 50000 );
        messageout.free_memory( 5000 );
        messageout.process_100m( 4000 );

        PostOffice::instance()->SendMail( &messageout );

        MessageStatusReport report;
        report.reports( 3 );
        PostOffice::instance()->SendMail( &report );

        return 0;
        // UserDefineHandler End
    }

} // End of namespace Protocol
#endif // !Message_Greeting_HANDLER_HPP_
