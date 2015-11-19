// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Helper class for using the BI process docker image.
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/11/18

#ifndef ANALYSIS_HELPER_H_
#define ANALYSIS_HELPER_H_

#include "IDockerHelper.h"
#include <string>
#include "curl.h"

namespace MaratonCommon
{
    class AnalysisHelper: public IDockerHelper
    {
    public:
        size_t Get();
        size_t Post();

	    CURL			*curl_;
	    CURLM			*multi_handle_;
	    curl_httppost	*post_;
	    curl_httppost	*last_;
	    curl_slist		*header_list_;
	    std::string		expect_;
    	int				running_flag;
	    timeval			timeout_;
    	long			timeout_ms_;
    };
}
#endif // !ANALYSIS_HELPER_H_
