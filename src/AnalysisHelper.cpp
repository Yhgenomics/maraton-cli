#include "DockerHelper.h"
#include "AnalysisHelper.h"
#include "ExecutorConfig.hpp"
#include <stdlib.h>
#include <curl.h>
#include <string>
#include <vector>

using namespace MaratonCommon;

size_t AnalysisHelper::ProcessData( const int& threadsNum, const string& refGene, const string& reads )
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

size_t AnalysisHelper::SortData( const string& samFile )
{
    vector< string > environment;
    environment.push_back( "samfile=" + samFile );
    vector< string > binds;
    binds.push_back( ExecutorConfig::instance()->outputDir + ":/output/"  );
    return DockerHelper::instance()->Run( ExecutorConfig::instance()->dockerDaemon
           , ExecutorConfig::instance()->sorter
           , binds
           , environment );
}

size_t AnalysisHelper::MergeProcess( const string& refGene, const string& bams ,const string& vcfName )
{
    vector< string > environment;
    environment.push_back( "refgen=" + refGene  );
    environment.push_back( "bams="   + bams     );
    environment.push_back( "vcf="    + vcfName  );

    vector< string > binds;
    binds.push_back( ExecutorConfig::instance()->inputDir  + ":/input/"   );
    binds.push_back( ExecutorConfig::instance()->outputDir + ":/output/"  );
    binds.push_back( ExecutorConfig::instance()->refDir    + ":/ref/"     );

    return DockerHelper::instance()->Run( ExecutorConfig::instance()->dockerDaemon
            , ExecutorConfig::instance()->merger
            , binds
            , environment );
}

size_t AnalysisHelper::PrepareSpace()
{
    return 0;
}

size_t AnalysisHelper::CheckEnviroment()
{
    DockerHelper::instance()->Pull( ExecutorConfig::instance()->dockerDaemon
            , ExecutorConfig::instance()->processor );
    DockerHelper::instance()->Pull( ExecutorConfig::instance()->dockerDaemon
            , ExecutorConfig::instance()->sorter );
    DockerHelper::instance()->Pull( ExecutorConfig::instance()->dockerDaemon
            , ExecutorConfig::instance()->merger );

return 0;
}

size_t AnalysisHelper::UploadResult()
{
    return 0;
}
