// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Interface to use docker  by REST API
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/11/18

#ifndef I_DOCKERHELPER_H_
#define I_DOCKERHELPER_H_

#include "stdio.h"

using namespace std;

namespace MaratonCommon
{
	// interface to create and start an docker container
	class IDockerHelper
	{
	public:
        virtual size_t Search() { return 0; };
        virtual size_t Pull()   { return 0; };
		virtual size_t Run()    { return 0; };
        virtual size_t Create() { return 0; };
        virtual size_t Start()  { return 0; };
        virtual size_t Stop()   { return 0; };
    };
}
#endif // !I_DOCKERHELPER_H_