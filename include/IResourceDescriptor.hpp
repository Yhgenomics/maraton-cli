// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Interface of the ResourceDescriptor, so can reuse it on varise of 
//              equitpments( plantforms ).(Thanks to shuboyang@yhgenomics.com) 
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/10/28

#ifndef I_REOUSRCE_DESCRIPTOR_HPP_
#define I_REOUSRCE_DESCRIPTOR_HPP_

#include < string >
using namespace std;

namespace MaratonCommon
{
	// By default a -1 is returned as a non-implement of methods.
	class IResourceDescriptor
	{
	public:
		virtual size_t  GetFreeMemorySize( const string& unit )
		{
			return -1;
		}
		virtual size_t GetFreeDiskSize( const string& unit )
		{
			return -1;
		}
		virtual size_t GetTestScore( const string& testName )
		{
			return -1;
		}
	};
}

#endif // !I_REOUSRCE_DESCRIPTOR_HPP_