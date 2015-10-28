
#include "SessionManager.hpp"
#include "MasterSession.h"
#include "Session.h"

#include "UVSockService.h"
#include "MessagesHandler.hpp"
#include <iostream>
#include "json.hpp"

int main ( int argc , char** argv )
{
	SessionManager<MasterSession>::instance ( )->on_create 
		(
			[ ] ( MasterSession* session )
			{
				session->on_message 
				( 
					[ ] ( Message* message )
					{
						std::cout << "On Message : "<< message->command() << std::endl;
						if ( Protocol::MessagesHandler::process ( message ) )
						{
							message->owner ( )->close ( );
						}
					} 
				);
			}	
		);

	UVSockService srv;

	srv.connect ( "10.0.0.234" , 90 );

	while ( true )
	{
		srv.run ( );
	}

}