// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Helper class for deliver POST and GET requests.
//              Be used by the DockerHelper, to use Docker remote.
//              APIs.
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/11/19

#ifndef NET_HELPER_H_
#define NET_HELPER_H_
#include "NetHelperParams.h"
#include "maraton.h"
#include <string>
#include <vector>

namespace MaratonCommon
{
    using string = std::string;
    class NetHelper:public Singleton<NetHelper>
    {
        public:
            size_t Init();
            size_t DeInit();
            size_t GetViaHTTP ( const NetHelperParams &params ,vector< string >* const response );
            size_t PostViaHTTP( const NetHelperParams &params ,vector< string >* const response );
            static long ResponseParser( void *data, int size, int nmemb, vector< string >* const response );

        private:
            friend Singleton<NetHelper>;
    };
}

#endif
