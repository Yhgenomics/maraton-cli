#ifndef PROCESS_CODE_MAKER_HPP_
#define PROCESS_CODE_MAKER_HPP_

#include "maraton.h"
#include <string>

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
    string	workdir		= "/home/ubuntu/GeneData/maratonworkspace/";
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

#endif // !PROCESS_CODE_MAKER_HPP_
