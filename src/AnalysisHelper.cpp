#include "DockerHelper.h"
#include "AnalysisHelper.h"
#include <stdlib.h>
#include <curl.h>
#include <string>
#include <vector>

using namespace MaratonCommon;

size_t AnalysisHelper::ProcessData()
{
    DockerHelper::instance()->Pull( "127.0.0.1:4243", "10.0.0.15:5000/busybox" );
    vector< string > environment;
    environment.push_back( "ref=123456" );
    environment.push_back( "idx=testab" );
    DockerHelper::instance()->Run( "127.0.0.1:4243" , "10.0.0.15:5000/busybox" , environment );
    return 0;
}
