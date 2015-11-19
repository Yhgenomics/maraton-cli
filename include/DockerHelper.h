// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: To use docker  by REST APIs
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/11/18

#ifndef DOCKERHELPER_H_
#define DOCKERHELPER_H_

#include "stdio.h"
#include <string>
#include "maraton.h"
using namespace std;

namespace MaratonCommon
{
	// interface to create and start an docker container
	class DockerHelper:public Singleton<DockerHelper>
	{
	public:
        virtual size_t Search() { return 0; };
        virtual size_t Pull()   { return 0; };
		virtual size_t Run()    { return 0; };
        virtual size_t Create( const string &dest, const string &image, const vector< string > &environment );
        virtual size_t Start()  { return 0; };
        virtual size_t Stop()   { return 0; };

    private:
        const string kDockerHeader      = "Content-Type: application/json";
        const string kListContianers    = "/containers/json";
        const string kCreateContianer   = "/containers/create";
        const string kStartContianer    = "/containers/(id)/start";
        friend Singleton<DockerHelper>;
    };
}
#endif // !I_DOCKERHELPER_H_