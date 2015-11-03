#ifndef MESSAGE_INTRO_HPP_
#define MESSAGE_INTRO_HPP_

#include "Message.h"
#include "json.hpp"
#include <string>
using namespace std;

namespace Protocol
{
    class MessageIntro : public Message
    {
    public:
    
        // Getter of uuid_
        string uuid()
        {
            return uuid_;
        }
        
        // Setter of uuid_
        void uuid( string value )
        {
            uuid_ = value;
            raw_data_[ "data" ][ "uuid" ] = value;
        }
        
        // Getter of free_memory_
        size_t free_memory()
        {
            return free_memory_;
        }
        
        // Setter of free_memory_
        void free_memory( size_t value )
        {
            free_memory_ = value;
            raw_data_[ "data" ][ "free_memory" ] = value;
        }
        
        // Getter of free_disk_
        size_t free_disk()
        {
            return free_disk_;
        }
        
        // Setter of free_disk_
        void free_disk( size_t value )
        {
            free_disk_ = value;
            raw_data_[ "data" ][ "free_disk" ] = value;
        }
        
        // Getter of process_100m_
        size_t process_100m()
        {
            return process_100m_;
        }
        
        // Setter of process_100m_
        void process_100m( size_t value )
        {
            process_100m_ = value;
            raw_data_[ "data" ][ "process_100m" ] = value;
        }
        
        // Serilize Constructor
        MessageIntro()
            : Message( PROTOCOL_VERSION , 92 , 0 )
        {
            uuid( "" );
            free_memory( 0 );
            free_disk( 0 );
            process_100m( 0 );
        }
        
        // Deserilize Constructor
        MessageIntro( Message* message )
            : Message( *message )
        {
            this->uuid_ = raw_data_[ "data" ][ "uuid" ].get<string>();
            this->free_memory_ = raw_data_[ "data" ][ "free_memory" ].get<size_t>();
            this->free_disk_ = raw_data_[ "data" ][ "free_disk" ].get<size_t>();
            this->process_100m_ = raw_data_[ "data" ][ "process_100m" ].get<size_t>();
        }
    
    private:
    
        string uuid_;
        size_t free_memory_;
        size_t free_disk_;
        size_t process_100m_;
    
    }; // End of class define of MessageIntro

} // End of namespace Protocol
#endif // !Message_Intro_HPP_
