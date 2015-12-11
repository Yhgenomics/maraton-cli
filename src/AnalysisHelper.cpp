#include "DockerHelper.h"
#include "AnalysisHelper.h"
#include "ExecutorConfig.hpp"
#include <stdlib.h>
#include <curl.h>
#include <string>
#include <vector>

using namespace MaratonCommon;

size_t AnalysisHelper::ProcessData( int threadsNum, string refGene, string reads )
{
    char    threadFlagBuffer[20];
    sprintf( threadFlagBuffer, "t=%d", threadsNum );
    string  threadFlag( threadFlagBuffer );

    vector< string > environment;
    environment.push_back( threadFlag           );
    environment.push_back( "refgen=" + refGene  );
    environment.push_back( "reads=" + reads     );

    vector< string > binds;
    binds.push_back( ExecutorConfig::instance()->inputDir  + ":/input/"   );
    binds.push_back( ExecutorConfig::instance()->outputDir + ":/output/"  );
    binds.push_back( ExecutorConfig::instance()->refDir    + ":/ref/"     );
    binds.push_back( ExecutorConfig::instance()->shmDir    + ":/dev/shm/" );

    return DockerHelper::instance()->Run( ExecutorConfig::instance()->dockerDaemon
            , ExecutorConfig::instance()->processor
            , binds
            , environment );
}

//size_t AnalysisHelper::Sort()
//{
//    ;
//}

//size_t AnalysisHelper::MergeProcess()
//{
//    ;
//}

size_t AnalysisHelper::PrepareSpace()
{
    return 0;
}

size_t AnalysisHelper::CheckEnviroment()
{
    DockerHelper::instance()->Pull( ExecutorConfig::instance()->dockerDaemon
            , ExecutorConfig::instance()->processor );
    return 0;
}

size_t AnalysisHelper::UploadResult()
{
    return 0;
}
