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
       size_t ProcessData();
    };
}
#endif // !ANALYSIS_HELPER_H_
