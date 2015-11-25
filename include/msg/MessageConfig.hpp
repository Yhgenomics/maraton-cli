#ifndef MESSAGE_CONFIG_HPP_
#define MESSAGE_CONFIG_HPP_

#include "Message.h"
#include "json.hpp"
#include <string>
using namespace std;

namespace Protocol
{
    class MessageConfig : public Message
    {
    public:
    
        // Getter of ref_addr_
        string ref_addr()
        {
            return ref_addr_;
        }
        
        // Setter of ref_addr_
        void ref_addr( string value )
        {
            ref_addr_ = value;
            raw_data_[ "data" ][ "ref_addr" ] = value;
        }
        
        // Getter of docekr_addr_
        string docekr_addr()
        {
            return docekr_addr_;
        }
        
        // Setter of docekr_addr_
        void docekr_addr( string value )
        {
            docekr_addr_ = value;
            raw_data_[ "data" ][ "docekr_addr" ] = value;
        }
        
        // Getter of fastq_addr_
        string fastq_addr()
        {
            return fastq_addr_;
        }
        
        // Setter of fastq_addr_
        void fastq_addr( string value )
        {
            fastq_addr_ = value;
            raw_data_[ "data" ][ "fastq_addr" ] = value;
        }
        
        // Getter of merge_addr_
        string merge_addr()
        {
            return merge_addr_;
        }
        
        // Setter of merge_addr_
        void merge_addr( string value )
        {
            merge_addr_ = value;
            raw_data_[ "data" ][ "merge_addr" ] = value;
        }
        
        // Serilize Constructor
        MessageConfig()
            : Message( PROTOCOL_VERSION , 170 , 0 )
        {
            ref_addr( "" );
            docekr_addr( "" );
            fastq_addr( "" );
            merge_addr( "" );
        }
        
        // Deserilize Constructor
        MessageConfig( Message* message )
            : Message( *message )
        {
            this->ref_addr_ = raw_data_[ "data" ][ "ref_addr" ].get<string>();
            this->docekr_addr_ = raw_data_[ "data" ][ "docekr_addr" ].get<string>();
            this->fastq_addr_ = raw_data_[ "data" ][ "fastq_addr" ].get<string>();
            this->merge_addr_ = raw_data_[ "data" ][ "merge_addr" ].get<string>();
        }
    
    private:
    
        string ref_addr_;
        string docekr_addr_;
        string fastq_addr_;
        string merge_addr_;
    
    }; // End of class define of MessageConfig

} // End of namespace Protocol
#endif // !Message_Config_HPP_
