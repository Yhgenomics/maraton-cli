#ifndef MESSAGE_GREETING_HANDLER_HPP_
#define MESSAGE_GREETING_HANDLER_HPP_

#include "MessageGreeting.hpp"
#include "MessageIntro.hpp"
#include <iostream>

namespace Protocol
{
    static int MessageGreetingHandler( MessageGreeting& msg )
    {
        // UserDefineHandler Begin
        // Your Codes here!
		cout << "Excutor Receive the msg of Greeting" << endl;
		cout << "Preparing the SelfIntro" << endl;

		MessageIntro messageout;
		messageout.cpu( "very powerful awesome i7" );
		messageout.cpufrequency( "6*3.5GHz" );
		messageout.ram( "16GB" );
		msg.owner()->send( &messageout );
		return 0;
		// UserDefineHandler End 
    }
    
} // End of namespace Protocol
#endif // !Message_Greeting_HANDLER_HPP_
