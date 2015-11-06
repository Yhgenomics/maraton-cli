// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Interface of the Appraiser
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/10/28

#ifndef I_APPRAISER_HPP_
#define I_APPRAISER_HPP_

#include "stdio.h"
using namespace std;

namespace MaratonCommon
{
	// A simple interface with one method GetScore to run tests and output the score.
	class IAppraiser
	{
	public:
		virtual size_t  GetScore() { return -1; }
	};
}

#endif // !I_APPRAISER_HPP_