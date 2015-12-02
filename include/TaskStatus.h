// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Total status and status for each block
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/12/2
#include <map>
#include <string>
using namespace std;

namespace MaratonCommon
{
    class TaskStatus
    {
    public:

        enum BlockStatus
        {
            WAIT=0,
            PULLING,
            PULLED,
            PROCESSING,
            PROCESSED,
            PUSHED,
            FINISHED
        };

        map< string, BlockStatus > block_map;
        bool IsAllFinished()
        {
            return IsAllPassPhase ( BlockStatus::FINISHED );
        }

        bool IsAllPulled()
        {
            return IsAllPassPhase ( BlockStatus::PULLED );
        }

        bool IsAllProcessed()
        {
            return IsAllPassPhase ( BlockStatus::PROCESSED);
        }

        bool IsAllPushed()
        {
            return IsAllPassPhase ( BlockStatus::PUSHED );
        }

        bool IsAllPassPhase ( BlockStatus phase )
        {
            bool result = true;
            for ( auto block : block_map )
            {
                result =  result && ( block.second >= phase );
                if (! result) { break; }
            }
            return result;
        }



     };
}
