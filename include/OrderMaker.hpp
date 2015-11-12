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
	string	phase1Flags	= "aln -t 8 -f";
	string	phase2Flags	= "samse -f";
	string	aligner		= "BWA.exe";
#else
	string	refGen		= "/home/ubuntu/GeneData/hg19.fa";
	string	workdir		= "/home/ubuntu/GeneData/maratonworkspace/";
	string	aligner		= "bwa";
	string	samtools	= "samtools"
	string	memOption	= "mem -t 3";
	string	viewOption	= "view -bS";
	string	sortOption	= "sort -m 2000000000"£»
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
#else
		return Params.aligner	+ " "
			 + Params.memOption + " "
			 + Params.refGen	+ " "
			 + workdir			+ Params.taskid	+ Params.fqTail
			 + " > "
			 + workdir			+ Params.taskid	+ Params.samTail;

#endif // ! _WIN32
		
	}

	string MakeBamOrder( const OrderMakerParams& Params )
	{
		if ( Params.taskid == "" )
		{
			return "";
		}
#ifdef _WIN32
#else
		return Params.samtools	+ " "
			+ Params.viewOption + " "
			+ workdir			+ Params.taskid + Params.samTail
			+ " > "
			+ workdir			+ Params.taskid + Params.bamTail;

#endif // ! _WIN32	
	}

	string SortBamOrder( const OrderMakerParams& Params )
	{
		if ( Params.taskid == "" )
		{
			return "";
		}
#ifdef _WIN32
#else
		return Params.samtools + " "
			+ Params.sortOption + " "
			+ workdir + Params.taskid + Params.bamTail
			+ workdir + Params.taskid + Params.sortedTail;

#endif // ! _WIN32
	}

	string MakePipeline( const OrderMakerParams& Params )
	{
		return    MakeSamOrder( Params )
				+ " && " + MakeBamOrder( Params )
				+ " && " + SortBamOrder( Params );
	}
private:
	friend Singleton<OrderMaker>;
};

#endif // !PROCESS_CODE_MAKER_HPP_
