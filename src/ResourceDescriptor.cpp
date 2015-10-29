// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Resource Descriptor 
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/10/28

#include "ResourceDescriptor.h"

#include < atlbase.h >
#include "winperf.h" 
#include < iostream >

using namespace std;
MaratonCommon::ResourceDescriptor::ResourceDescriptor()
{
}

MaratonCommon::ResourceDescriptor::~ResourceDescriptor()
{
}

size_t MaratonCommon::ResourceDescriptor::GetFreeMemorySize( const string & unit )
{
	size_t offset = _GetSpaceOffset( unit );

	if ( offset >= 0 )
	{
		MEMORYSTATUSEX memoryStatus;

		memoryStatus.dwLength = sizeof( memoryStatus );
		GlobalMemoryStatusEx( &memoryStatus );
		return memoryStatus.ullAvailPhys >> offset;
	}
	else
	{
		return -1;
	}
}

size_t MaratonCommon::ResourceDescriptor::GetFreeDiskSize( const string & unit )
{
	size_t offset = _GetSpaceOffset( unit );

	if ( offset >= 0 )
	{
		ULARGE_INTEGER freeBytesAvailable;
		GetDiskFreeSpaceEx( NULL , &freeBytesAvailable , NULL , NULL );
		return  freeBytesAvailable.QuadPart >> offset;
	}
	else
	{
		return -1;
	}
}

size_t MaratonCommon::ResourceDescriptor::GetTestScore( const string & testName )
{
	return appraiser_factory_.CreateAppraiser( testName )->GetScore();
}

size_t MaratonCommon::ResourceDescriptor::_GetSpaceOffset( const string & unit )
{
	size_t offset;
	if ( unit == "MB" )
	{
		offset = 20;
	}
	else if ( unit == "GB" )
	{
		offset = 30;
	}
	else if ( unit == "KB" )
	{
		offset = 10;
	}
	else if ( unit == "B" || unit == "Byte" )
	{
		offset = 0;
	}
	else
	{
		offset = -1;
	}
	return offset;
}