#ifndef MESSAGE_COMMAND_HPP_
#define MESSAGE_COMMAND_HPP_

#include "Message.h"
#include "json.hpp"
#include <string>
using namespace std;

namespace Protocol
{
    class MessageCommand : public Message
    {
    public:
    
        // Getter of command_line_
        string command_line()
        {
            return command_line_;
        }
        
        // Setter of command_line_
        void command_line( string value )
        {
            command_line_ = value;
            raw_data_[ "data" ][ "command_line" ] = value;
        }
        
        // Getter of uri_list_
        vector<std::string> uri_list()
        {
            return uri_list_;
        }
        
        // Setter of uri_list_
        void uri_list( vector<std::string> value )
        {
            uri_list_ = value;
            raw_data_[ "data" ][ "uri_list" ] = value;
        }
        
        // Getter of run_as_
        string run_as()
        {
            return run_as_;
        }
        
        // Setter of run_as_
        void run_as( string value )
        {
            run_as_ = value;
            raw_data_[ "data" ][ "run_as" ] = value;
        }
        
        // Serilize Constructor
        MessageCommand()
            : Message( PROTOCOL_VERSION , 166 , 0 )
        {
            command_line( "" );
            uri_list(  );
            run_as( "" );
        }
        
        // Deserilize Constructor
        MessageCommand( Message* message )
            : Message( *message )
        {
            this->command_line_ = raw_data_[ "data" ][ "command_line" ].get<string>();
            this->uri_list_ = raw_data_[ "data" ][ "uri_list" ].get<vector<std::string>>();
            this->run_as_ = raw_data_[ "data" ][ "run_as" ].get<string>();
        }
    
    private:
    
        string command_line_;
        vector<std::string> uri_list_;
        string run_as_;
    
    }; // End of class define of MessageCommand

} // End of namespace Protocol
#endif // !Message_Command_HPP_
