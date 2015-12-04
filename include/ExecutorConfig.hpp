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

    // suffix
    string  NoTail      = "";
    string  inputTail   = ".in";
    string  outputTail  = ".in.bam";

    // sleep time
    size_t  transWait   = 1000;  // in milliseconds
    size_t  processWait = 10000; // in milliseconds

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

        fileName= uri.substr( pos + 1, uri.length() - pos - 1 );

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

/*
class OrderMakerParams
{
    using	string		= std::string;
public:
    OrderMakerParams( string id )
    {
        this->taskid = id;
    }
    string	separator	= " ";
    string	taskid		= "";
    string	saiTail		= ".sai";
    string	fqTail		= ".fastq";
    string	samTail		= ".sam";
    string	bamTail		= ".bam";
    string	sortedTail	= ".sorted";
    string	postDest	= "http://10.0.0.20/file/upload_result";
#ifdef _WIN32
    string	refGen		= "E:\\GeneData\\hg19.fa";
    string	workdir		= "E:\\GeneData\\";
    string	p1Flags		= "aln -t 8 -f";
    string	p2Flags		= "samse -f";
    string	aligner		= "BWA.exe";
#else
    string	refGen		= "/home/ubuntu/GeneData/hg19.fa";
    string	workdir		= "/home/ubuntu/GeneData/";
    string  refDir      = "/home/ubuntu/GeneData/ref/";
    string  inputDir    = "/home/ubuntu/GeneData/dockerin/";
    string  outputDir   = "/home/ubuntu/GeneData/dockerout/";
    string	aligner		= "bwa";
    string	samtools	= "samtools";
    string	memOption	= "mem -t 3";
    string	viewOption	= "view -bS";
    string	sortOption	= "sort -m 2000000000";
#endif
};

class OrderMaker :public Singleton<OrderMaker>
{
public:
    using string = std::string;

    string MakeSamOrder( const OrderMakerParams& Params )
    {
        if ( Params.taskid == "" )
        {
            return "";
        }
#ifdef _WIN32
        return Params.workdir   + Params.aligner    + Params.separator
             + Params.p1Flags   + Params.separator
             + Params.workdir   + Params.taskid     + Params.saiTail    + Params.separator
             + Params.refGen    + Params.separator
             + Params.workdir   + Params.taskid     + Params.fqTail
             + " && "
             + Params.workdir   + Params.aligner    + Params.separator
             + Params.p2Flags   + Params.separator
             + Params.workdir   + Params.taskid     + Params.samTail    + Params.separator
             + Params.refGen    + Params.separator
             + Params.workdir   + Params.taskid     + Params.saiTail    + Params.separator
             + Params.workdir   + Params.taskid     + Params.fqTail;

#else
        return Params.aligner   + " "
             + Params.memOption + " "
             + Params.refGen    + " "
             + Params.workdir   + Params.taskid     + Params.fqTail
             + " > "
             + Params.workdir   + Params.taskid     + Params.samTail;

#endif // ! _WIN32
    }

    string MakeBamOrder( const OrderMakerParams& Params )
    {
        if ( Params.taskid == "" )
        {
            return "";
        }
#ifdef _WIN32
        return "";
#else
        return Params.samtools      + " "
             + Params.viewOption    + " "
             + Params.workdir       + Params.taskid + Params.samTail
             + " > "
             + Params.workdir       + Params.taskid + Params.bamTail;

#endif // ! _WIN32
    }

    string SortBamOrder( const OrderMakerParams& Params )
    {
        if ( Params.taskid == "" )
        {
            return "";
        }
#ifdef _WIN32
        return "";
#else
        return Params.samtools      + " "
             + Params.sortOption    + " "
             + Params.workdir       + Params.taskid + Params.bamTail + " "
             + Params.workdir       + Params.taskid + Params.sortedTail;

#endif // ! _WIN32
    }

    string MakePipeline( const OrderMakerParams& Params )
    {

#ifdef _WIN32
        return "";
#else
        return MakeSamOrder( Params ) + " && "
             + MakeBamOrder( Params ) + " && "
             + SortBamOrder( Params );

#endif // ! _WIN32

    }

private:
    friend Singleton<OrderMaker>;
};
*/

#endif // !EXECUTOR_CONFIG_HPP_
