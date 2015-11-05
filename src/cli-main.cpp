#include "SessionManager.hpp"
#include "MasterSession.h"
#include "Session.h"
#include "MasterFetcher.h"
#include "UVSockService.h"
#include "MessagesHandler.hpp"
#include "PostOffice.h"
#include "json.hpp"
#include <iostream>
#include "FileDownloader.h"
#include <vector>
#include <string>

int main( int argc , char** argv )
{
	PostOffice::instance()->master_ip = "10.0.0.20";
	PostOffice::instance()->master_port = 90;
	PostOffice::instance()->Init();

	SessionManager<MasterSession>::instance()->on_create
		(
			[] ( MasterSession* session )
			{
				MasterFetcher::instance()->master_pointer( session );
				PostOffice::instance()->self_status = "3";

 				std::cout << "On Connected : " << session->id() << std::endl;
				session->on_message([] ( Message* message )
						{
							std::cout << "On Message : " << message->command() << std::endl;
							
							if ( Protocol::MessagesHandler::process( message ) < 0 )
							{
								message->owner()->close();
							}
						}
				); // End of session->on_message

				session->on_close ([] (ClusterSession* session)
						{							
							std::cout << "On Close : " << session->id() << std::endl;
							MasterFetcher::instance()->master_pointer( nullptr ) ;

						}
				); // End of session->on_close
			}
		); // End of  SessionManager<MasterSession>::instance()->on_create

	PostOffice::instance()->Run();
}