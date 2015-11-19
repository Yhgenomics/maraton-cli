// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Helper class for deliver POST and GET requests.
//              Be used by the DockerHelper, to use Docker remote.
//              APIs.
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/11/19

#ifndef NET_HELPER_H_
#define NET_HELPER_H_
#include "maraton.h"
#include <string>

namespace MaratonCommon
{
    using string = std::string;
    class NetHelper:public Singleton<NetHelper>
    {
        public:
            size_t Init();
            size_t DeInit();
            size_t Get ( NetHelperParams params );
            size_t Post( NetHelperParams params );
        private:
            friend Singleton<NetHelper>;
    }
}

#endif
