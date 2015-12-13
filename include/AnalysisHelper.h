// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Helper class for using the BI process docker image.
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/11/18

#ifndef ANALYSIS_HELPER_H_
#define ANALYSIS_HELPER_H_

#include "DockerHelper.h"
#include <string>
#include "curl.h"

namespace MaratonCommon
{
    class AnalysisHelper : public DockerHelper
    {
    public:
       size_t   ProcessData( const int& threadNum, const string& refGene,const string& reads );
       size_t   PrepareSpace();
       size_t   CheckEnviroment();
       size_t   UploadResult();
       size_t   SortData( const string& samFile );
       size_t   MergeProcess( const string& refGene, const string& bams ,const string& vcfName );
    };
}
#endif // !ANALYSIS_HELPER_H_
