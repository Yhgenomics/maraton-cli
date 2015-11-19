// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Params for the NET helper to keep the interface
//              stable.
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/11/19

#ifndef NET_HELPER_PARAMS_H_
#define NET_HELPER_PARAMS_H_
#include <string>
#include <vector>
namespace MaratonCommon
{
    using std::string;
    using std::vector;
    class NetHelperParams
    {
    public:
        string              url;
        vector< string >    headers;
        bool                verbose;
        enum XMark
        {
            UNKNOWN = 0,
            GET     = 1,
            POST    = 2
        };
        XMark               option;
        string              post_fields;
    };
}

#endif
