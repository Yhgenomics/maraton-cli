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
            kWait=0,
            kPulling,
            kPulled,
            kProcessing,
            kProcessed,
            kPushing,
            kPushed,
            kFinished,
            kException
        };

        map< string, BlockStatus > block_map;
        string ref_gen_name;
        string task_id;

        bool IsAllFinished()
        {
            return IsAllPassPhase ( BlockStatus::kFinished  );
        }

        bool IsAllPulled()
        {
            return IsAllPassPhase ( BlockStatus::kPulled    );
        }

        bool IsAllProcessed()
        {
            return IsAllPassPhase ( BlockStatus::kProcessed );
        }

        bool IsAllPushed()
        {
            return IsAllPassPhase ( BlockStatus::kPushed    );
        }

        bool IsAnyException()
        {
            bool result = false;
            for ( auto block : block_map )
            {
                if ( BlockStatus::kException == block.second )
                {
                    result = true;
                    break;
                }
            }
            return result;
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
