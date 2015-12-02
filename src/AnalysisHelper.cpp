#include "DockerHelper.h"
#include "AnalysisHelper.h"
#include <stdlib.h>
#include <curl.h>
#include <string>
#include <vector>

using namespace MaratonCommon;

size_t AnalysisHelper::ProcessData( int threadsNum, string refGene, string reads )
{
    vector< string > environment;
    char threadFlagBuffer[20];
    sprintf( threadFlagBuffer, "t=%d",threadsNum );
    string threadFlag( threadFlagBuffer );
    environment.push_back( threadFlag);
    environment.push_back( "refgen="+refGene );
    environment.push_back( "reads="+reads );
    vector< string > binds;
    binds.push_back( "/home/ubuntu/GeneData/dockerin/:/input/" );
    binds.push_back( "/home/ubuntu/GeneData/dockerout/:/output/" );
    binds.push_back( "/home/ubuntu/GeneData/ref/:/ref/" );
    binds.push_back( "/dev/shm/:/dev/shm/" );
    return DockerHelper::instance()->Run( docker_daemon, process_image, binds, environment );
}

size_t AnalysisHelper::PrepareSpace()
{
    return 0;
}

size_t AnalysisHelper::CheckEnviroment()
{
    DockerHelper::instance()->Pull( docker_daemon, process_image );
    return 0;
}

size_t AnalysisHelper::UploadResult()
{
    return 0;
}
