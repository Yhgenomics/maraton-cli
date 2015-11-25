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
       size_t ProcessData( int threadNum, string refGene, string reads );
       size_t PrepareSpace();
       size_t CheckEnviroment();
       size_t UploadResult();
    private:
       const string input_dir       = "/home/ubuntu/GeneData/dockerin/";
       const string output_dir      = "/home/ubuntu/GeneData/dockerout/";
       const string ref_dir         = "/home/ubuntu/GeneData/";
       const string docker_daemon   = "127.0.0.1:4243";
       const string process_image   = "10.0.0.15:5000/bwamem";
    };
}
#endif // !ANALYSIS_HELPER_H_
