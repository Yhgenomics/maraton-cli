#ifndef MESSAGE_TASK_PROGRESS_HPP_
#define MESSAGE_TASK_PROGRESS_HPP_

#include "Message.h"
#include "json.hpp"
#include <string>
using namespace std;

namespace Protocol
{
    class MessageTaskProgress : public Message
    {
    public:
    
        // Getter of task_id_
        string task_id()
        {
            return task_id_;
        }
        
        // Setter of task_id_
        void task_id( string value )
        {
            task_id_ = value;
            raw_data_[ "data" ][ "task_id" ] = value;
        }
        
        // Getter of progress_
        size_t progress()
        {
            return progress_;
        }
        
        // Setter of progress_
        void progress( size_t value )
        {
            progress_ = value;
            raw_data_[ "data" ][ "progress" ] = value;
        }
        
        // Serilize Constructor
        MessageTaskProgress()
            : Message( PROTOCOL_VERSION , 134 , 0 )
        {
            task_id( "" );
            progress( 0 );
        }
        
        // Deserilize Constructor
        MessageTaskProgress( Message* message )
            : Message( *message )
        {
            this->task_id_ = raw_data_[ "data" ][ "task_id" ].get<string>();
            this->progress_ = raw_data_[ "data" ][ "progress" ].get<size_t>();
        }
    
    private:
    
        string task_id_;
        size_t progress_;
    
    }; // End of class define of MessageTaskProgress

} // End of namespace Protocol
#endif // !Message_Task_Progress_HPP_
