#ifndef MESSAGE_MERGE_PROCESS_HPP_
#define MESSAGE_MERGE_PROCESS_HPP_

#include "Message.h"
#include "json.hpp"
#include <string>
using namespace std;

namespace Protocol
{
    class MessageMergeProcess : public Message
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
        
        // Getter of merger_
        string merger()
        {
            return merger_;
        }
        
        // Setter of merger_
        void merger( string value )
        {
            merger_ = value;
            raw_data_[ "data" ][ "merger" ] = value;
        }
        
        // Serilize Constructor
        MessageMergeProcess()
            : Message( PROTOCOL_VERSION , 171 , 0 )
        {
            task_id( "" );
            uri_list(  );
            merger( "" );
        }
        
        // Deserilize Constructor
        MessageMergeProcess( Message* message )
            : Message( *message )
        {
            this->task_id_ = raw_data_[ "data" ][ "task_id" ].get<string>();
            this->uri_list_ = raw_data_[ "data" ][ "uri_list" ].get<vector<std::string>>();
            this->merger_ = raw_data_[ "data" ][ "merger" ].get<string>();
        }
    
    private:
    
        string task_id_;
        vector<std::string> uri_list_;
        string merger_;
    
    }; // End of class define of MessageMergeProcess

} // End of namespace Protocol
#endif // !Message_Merge_Process_HPP_
