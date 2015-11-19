#include "DockerHelper.h"
#include "AnalysisHelper.h"
#include <stdlib.h>
#include <curl.h>
#include <string>
#include <vector>

using namespace MaratonCommon;

size_t AnalysisHelper::ProcessData()
{
    vector< string > environment;
    environment.push_back( "ref=123456" );
    environment.push_back( "idx=testab" );
    DockerHelper::instance()->Create( "127.0.0.1:4243" , "hello-world" , environment );
    return 0;
}
