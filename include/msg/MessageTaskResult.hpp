#ifndef MESSAGE_TASK_RESULT_HPP_
#define MESSAGE_TASK_RESULT_HPP_

#include "Message.h"
#include "json.hpp"
#include <string>
using namespace std;

namespace Protocol
{
    class MessageTaskResult : public Message
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
        
        // Getter of error_
        size_t error()
        {
            return error_;
        }
        
        // Setter of error_
        void error( size_t value )
        {
            error_ = value;
            raw_data_[ "data" ][ "error" ] = value;
        }
        
        // Getter of stage_
        size_t stage()
        {
            return stage_;
        }
        
        // Setter of stage_
        void stage( size_t value )
        {
            stage_ = value;
            raw_data_[ "data" ][ "stage" ] = value;
        }
        
        // Getter of result_
        string result()
        {
            return result_;
        }
        
        // Setter of result_
        void result( string value )
        {
            result_ = value;
            raw_data_[ "data" ][ "result" ] = value;
        }
        
        // Serilize Constructor
        MessageTaskResult()
            : Message( PROTOCOL_VERSION , 133 , 0 )
        {
            task_id( "" );
            error( 0 );
            stage( 0 );
            result( "TaskNoError" );
        }
        
        // Deserilize Constructor
        MessageTaskResult( Message* message )
            : Message( *message )
        {
            this->task_id_ = raw_data_[ "data" ][ "task_id" ].get<string>();
            this->error_ = raw_data_[ "data" ][ "error" ].get<size_t>();
            this->stage_ = raw_data_[ "data" ][ "stage" ].get<size_t>();
            this->result_ = raw_data_[ "data" ][ "result" ].get<string>();
        }
    
    private:
    
        string task_id_;
        size_t error_;
        size_t stage_;
        string result_;
    
    }; // End of class define of MessageTaskResult

} // End of namespace Protocol
#endif // !Message_Task_Result_HPP_
