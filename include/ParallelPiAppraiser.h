// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Appraise by a Parallel computing version of Pi caculation 
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/10/28

#ifndef PARALLEL_PI_APPRAISER_H_
#define PARALLEL_PI_APPRAISER_H_

#include "IAppraiser.hpp"

namespace MaratonCommon
{
	class ParallelPiAppraiser :public IAppraiser
	{
	public:
		virtual size_t GetScore();
	};

}

#endif // !PARALLEL_PI_APPRAISER_H_
