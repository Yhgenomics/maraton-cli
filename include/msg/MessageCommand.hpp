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
        
        // Serilize Constructor
        MessageCommand()
            : Message( PROTOCOL_VERSION , 166 , 0 )
        {
            command_line( "" );
        }
        
        // Deserilize Constructor
        MessageCommand( Message* message )
            : Message( *message )
        {
            this->command_line_ = raw_data_[ "data" ][ "command_line" ].get<string>();
        }
    
    private:
    
        string command_line_;
    
    }; // End of class define of MessageCommand

} // End of namespace Protocol
#endif // !Message_Command_HPP_
