// Copyright (c) 2015 Yhgenomics. All rights reserved.
// Description: Const values for the executor.
//              All magic values are set here until
//              better place to refactor.
// Creator:		Ke Yang(keyang@yhgenomics.com)
// Date:		2015/12/3

#ifndef EXECUTOR_CONFIG_HPP_
#define EXECUTOR_CONFIG_HPP_

#include "maraton.h"
#include <string>

class ExecutorConfig : public Singleton< ExecutorConfig >
{
public:
    // machine ablity params
    size_t  threadNum   = 5;

    // dir and address
    string  inputDir    = "/data/input/";
    string  outputDir   = "/data/output/";
    string  refDir      = "/data/ref/";
    string  shmDir      = "/dev/shm/";
    string  postDest    = "http://10.0.0.20/file/upload_result";
    string  dockerDaemon= "127.0.0.1:4243";
    string  processor   = "10.0.0.15:5000/bwamem";

    // suffix
    string  NoTail      = "";
    string  inputTail   = ".in";
    string  outputTail  = ".in.bam";

    // sleep time
    size_t  transWait   = 1000;  // in milliseconds
    size_t  processWait = 1000;  // in milliseconds

    // suffix strategy
    enum SuffixType
    {
        kNoSuffix,
        kInputFile,
        kOutPutFile
    };

    bool    GetFileName ( const string &uri, string &fileName, SuffixType suffixType )
    {
        int pos = uri.find_last_of( '/' );
        if ( pos == string::npos )
            return false;

        fileName = uri.substr( pos + 1, uri.length() - pos - 1 );

        switch ( suffixType )
        {
            case kInputFile:
                fileName += inputTail;
                break;
            case kOutPutFile:
                fileName += outputTail;
                break;
            case kNoSuffix:
                fileName += NoTail;
                break;
        }

        return true;
    }

private:
    friend Singleton< ExecutorConfig >;

};

#endif // !EXECUTOR_CONFIG_HPP_
