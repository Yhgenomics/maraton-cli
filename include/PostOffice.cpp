#include "PostOffice.h"
#include "MasterFetcher.h"
#include "MessageHeartBeat.hpp"

void heartbeat_callback( uv_timer_t * timer )
{
	static int last_tick = 0;
	if ( nullptr == MasterFetcher::instance()->master_pointer() )
		return;

	int temp_tick = Timer::tick();
	int delta = temp_tick - last_tick;

	if ( delta > 1000 )
	{
		Protocol::MessageHeartBeat msg;
		MasterFetcher::instance()->master_pointer()->send( &msg );
		last_tick = temp_tick;
	}
}

int PostOffice::SendMail( Message * msg )
{
	if ( nullptr == MasterFetcher::instance()->master_pointer() )
		return -1;
	MasterFetcher::instance()->master_pointer()->send( msg );
	return 0;
}

int PostOffice::StartHeartBeating()
{	
	uv_timer_init( uv_default_loop() , &heart_beat_timer );
	uv_timer_start( &heart_beat_timer , heartbeat_callback , 0 , 1 );
	return 0;
}
