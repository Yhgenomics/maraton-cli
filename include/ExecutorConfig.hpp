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
    size_t  threadNum    = 5;

    // dir and address
    string  inputDir     = "/data/input/";
    string  outputDir    = "/data/output/";
    string  refDir       = "/data/ref/";
    string  shmDir       = "/dev/shm/";
    string  postDest     = "http://10.0.0.20/file/upload_result";
    string  dockerDaemon = "127.0.0.1:4243";
    string  processor    = "10.0.0.15:5000/bwamem";
    string  sorter       = "10.0.0.15:5000/smsort";
    string  merger       = "10.0.0.15:5000/smmerge";
    string  pullFastq    = "http://10.0.0.20/file/download";
    string  postBam      = "http://10.0.0.20/file/upload_bam";
    string  pullBam      = "http://10.0.0.20/file/download_bam";
    string  postVcf      = "http://10.0.0.20/file/upload_vcf";

    // suffix
    string  NoTail       = "";
    string  inputTail    = ".in";
    string  bwaTail      = ".sam";
    string  sortedTail   = ".bam";
    string  outputTail   = ".bam";
    string  unmergeTail  = ".unmerged";

    // sleep time
    size_t  transWait    = 100;  // in milliseconds
    size_t  processWait  = 100;  // in milliseconds
    size_t  sortWait     = 100;  // in milliseconds

    // suffix strategy
    enum SuffixType
    {
        kNoSuffix,
        kInputFile,
        kBWAFile,
        kSortedFile,
        kOutPutFile,
        kUnMergeFile
    };

    enum TaskStage
    {
        kUnknown = 0,
        kAligned = 1,
        kMerged  = 2
    };

    bool    GetFileName ( const string &uri, string &fileName, SuffixType suffixType )
    {
        int pos = uri.find_last_of( '/' );
        bool result = false;
        if ( pos != string::npos )
        {
            fileName = uri.substr( pos + 1, uri.length() - pos - 1 );
            result = GetFileName( fileName, suffixType );
        }
        return result;
    }

    bool    GetFileName ( string &fileName, SuffixType suffixType )
    {
        bool result = false;
        switch ( suffixType )
        {
            case kInputFile:
                fileName += inputTail;
                result    = true;
                break;
            case kBWAFile:
                fileName += bwaTail;
                result    = true;
                break;
            case kSortedFile:
                fileName += sortedTail;
                result    = true;
                break;
            case kOutPutFile:
                fileName += outputTail;
                result    = true;
                break;
            case kNoSuffix:
                fileName += NoTail;
                result    = true;
                break;
            case kUnMergeFile:
                fileName += unmergeTail;
                result    = true;
                break;
            default :
                break;
        }
        return result;
    }

    bool    GetUri      ( string &uri, const string &suffix,const string &uuid, const string &id )
    {
        bool result = false;
        if( "" != id)
        {
            uri = suffix + "/"+ id + "/" + uuid;
            result = true;
        }
        else
        {
            uri = suffix + "/" + uuid;
            result = true;
        }
        return result;
    }

private:
    friend Singleton< ExecutorConfig >;

};

#endif // !EXECUTOR_CONFIG_HPP_
