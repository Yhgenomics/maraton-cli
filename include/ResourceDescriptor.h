// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Dscripe the infomations of free memory diskspace and score for certain test type
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/10/27

#ifndef RESOURCE_DESCRIPTOR_H_
#define RESOURCE_DESCRIPTOR_H_

#include "IResourceDescriptor.hpp"
#include "AppraiserFactory.h"
#include <string>

namespace MaratonCommon
{
	using string = std::string;

	// Free physical memroy size can use unit of "B"("Byte"),"KB","MB","GB"
	// Free disk space size can use unit of "B"("Byte"),"KB","MB","GB"
	// Score for a certain test was given by running some certain phases of tests,
	// which will be created by a factory base on the testName.
	class ResourceDescriptor :public IResourceDescriptor
	{
	public:
		virtual size_t		GetFreeMemorySize( const string& unit );
		virtual size_t		GetFreeDiskSize( const string& unit );
		virtual size_t		GetTestScore( const string& testName );
		ResourceDescriptor();
		~ResourceDescriptor();

	private:
		virtual size_t		_GetSpaceOffset( const string& unit );

		AppraiserFactory	appraiser_factory_;
	};

}

#endif // !RESOURCE_DESCRIPTOR_H_
