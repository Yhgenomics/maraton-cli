// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description:	Factory to create a certain IAppraiser instance
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/10/28

#ifndef APPRAISER_FACTORY_H_
#define	APPRAISER_FACTORY_H_

#include "IAppraiser.hpp"
#include "AppraiserList.h"
#include < memory >
#include < string >

using namespace std;

namespace MaratonCommon
{
	// Create an IAppraiser instance based on the input name.
	// An unique_ptr<IAppraiser> is used as the return value
	// instead of a raw (naked) pointer.
	class AppraiserFactory
	{
	public:
		unique_ptr<IAppraiser> CreatAppraiser( const string& name );

	};
}

#endif // !APPRAISER_FACTORY_H_

