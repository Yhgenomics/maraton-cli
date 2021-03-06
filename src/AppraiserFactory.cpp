// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Appraiser Factory parse the input name here.
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/10/27

#include "AppraiserFactory.h"
#include <memory>
#include <memory.h>

using namespace MaratonCommon;

unique_ptr<IAppraiser> MaratonCommon::AppraiserFactory::CreateAppraiser( const string & name )
{
    unique_ptr<IAppraiser> outAppraiser;
    if ( name == "ParallelPi" )
    {
        unique_ptr<IAppraiser> out( new ParallelPiAppraiser() );
        outAppraiser= std::move( out );
    }
    return outAppraiser;
}
