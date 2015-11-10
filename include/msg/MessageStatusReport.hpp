#ifndef MESSAGE_STATUS_REPORT_HPP_
#define MESSAGE_STATUS_REPORT_HPP_

#include "Message.h"
#include "json.hpp"
#include <string>
using namespace std;

namespace Protocol
{
    class MessageStatusReport : public Message
    {
    public:
    
        // Getter of reports_
        size_t reports()
        {
            return reports_;
        }
        
        // Setter of reports_
        void reports( size_t value )
        {
            reports_ = value;
            raw_data_[ "data" ][ "reports" ] = value;
        }
        
        // Serilize Constructor
        MessageStatusReport()
            : Message( PROTOCOL_VERSION , 141 , 0 )
        {
            reports( 0 );
        }
        
        // Deserilize Constructor
        MessageStatusReport( Message* message )
            : Message( *message )
        {
            this->reports_ = raw_data_[ "data" ][ "reports" ].get<size_t>();
        }
    
    private:
    
        size_t reports_;
    
    }; // End of class define of MessageStatusReport

} // End of namespace Protocol
#endif // !Message_Status_Report_HPP_
