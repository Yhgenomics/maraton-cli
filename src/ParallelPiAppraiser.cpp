// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Use OpenMP for the parallel processing 
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/10/28

#include "ParallelPiAppraiser.h"
#include <iostream>
#include <string>
#include <time.h>
//#include <omp>
#define N 10000000000

// Parallel cacutlate the pi for N times
size_t MaratonCommon::ParallelPiAppraiser::GetScore()
{
    double  local;
    double  pi      = 0.0;
    double  w       = 1.0 / N;
    clock_t t1      = clock();

    //#pragma omp parallel for private(local) reduction(+:pi)
    for ( long i = 0; i < N; i++ )
    {
        local   = ( i + 0.5 )*w;
        pi      = pi + 4.0 / ( 1.0 + local*local );
    }
    clock_t t2      = clock();

    return size_t( t2 - t1 );
}


