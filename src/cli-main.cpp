
#include "SessionManager.hpp"
#include "MasterSession.h"
#include "Session.h"

#include "UVSockService.h"
#include "MessagesHandler.hpp"
#include <iostream>
#include "json.hpp"

class SingleMaster:
	public Singleton<SingleMaster>
{
public:
	MasterSession* MasterPointer;
private:
	friend Singleton<SingleMaster>;
};

void timer_callback( uv_timer_t * timer )
{
	static int last_tick = 0;
	if ( nullptr == SingleMaster::instance()->MasterPointer )
		return;

	int delta = Timer::tick() - last_tick;

	if ( delta > 1000 )
	{
		Protocol::MessageHeartBeat msg;
		SingleMaster::instance()->MasterPointer->send( &msg );
	}
}


int main( int argc , char** argv )
{
	SessionManager<MasterSession>::instance()->on_create
		(
		[] ( MasterSession* session )
		{
			std::cout << "On Connected : " << session->id() << std::endl;
			session->on_message
				(
				[] ( Message* message )
			{
				std::cout << "On Message : " << message->command() << std::endl;
				if ( Protocol::MessagesHandler::process( message ) < 0 )
				{
					message->owner()->close();
				}

			}
			);

			session->on_close( [] (ClusterSession* session)
			{
				std::cout << "On Close : " << session->id() << std::endl;
				SingleMaster::instance()->MasterPointer = nullptr;
			} );

			SingleMaster::instance()->MasterPointer = session;
		}
	);

	int last_tick = 0;
	uv_timer_t timer;
	uv_timer_init( uv_default_loop() , &timer );
	uv_timer_start( &timer ,timer_callback , 0 , 1 );


	UVSockService srv;

	srv.connect( "10.0.0.234" , 90 );

	while ( true )
	{
		
		srv.run();
	}

}