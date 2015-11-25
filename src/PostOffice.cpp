#include "PostOffice.h"
#include "MasterFetcher.h"
#include "MessageHeartBeat.hpp"
#include "MessageStatusReport.hpp"

void HeartbeatCallback( uv_timer_t * timer )
{
    if ( nullptr == MasterFetcher::instance()->master_pointer() )
        return;

    Protocol::MessageHeartBeat msg;
    MasterFetcher::instance()->master_pointer()->send( &msg );
}

void ReconnetCallBack( uv_timer_t* timer )
{
    if ( nullptr == MasterFetcher::instance()->master_pointer() )
    {
        PostOffice::instance()->ReconnectService();
    }
    return;
}

int PostOffice::SendMail( Message * msg )
{
    if ( nullptr == MasterFetcher::instance()->master_pointer() )
        return -1;
    MasterFetcher::instance()->master_pointer()->send( msg );
    return 0;
}

int PostOffice::SendSelfStatus()
{
    Protocol::MessageStatusReport messageout;
    messageout.reports( static_cast< size_t >( self_status ) );
    SendMail( &messageout );
    return 0;
}

int PostOffice::Init()
{
    PostOffice::instance()->StartReconnectKeeper();
    PostOffice::instance()->StartHeartBeating();
    return 0;
}

int PostOffice::StartHeartBeating()
{
    memset( &heart_beat_timer , 0 , sizeof( uv_timer_t ) );
    uv_timer_init ( uv_default_loop() , &heart_beat_timer            );
    uv_timer_start( &heart_beat_timer , HeartbeatCallback , 0 , 1000 );
    return 0;
}

int PostOffice::StartReconnectKeeper()
{
    memset( &reconnect_timer , 0 , sizeof( uv_timer_t ) );
    uv_timer_init ( uv_default_loop() , &reconnect_timer            );
    uv_timer_start( &reconnect_timer  , ReconnetCallBack , 0 , 1000 );
    return 0;
}

int PostOffice::ReconnectService()
{
    if ( master_ip != "" )
    {
        sock_service.connect( master_ip , master_port );
        printf( "Try connect to server... \r\n" );
    }
    return 0;
}

void PostOffice::Run()
{
    while ( true )
    {
        sock_service.run();
    }
}
