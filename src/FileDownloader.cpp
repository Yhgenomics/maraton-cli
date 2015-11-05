#include "FileDownloader.h"
#include <iostream>
#include <fstream>
#include <string>

FileDownloader::FileDownloader( bool* cancel )
{
	cancel_ = cancel;
}

FileDownloader::~FileDownloader()
{
}

size_t FileDownloader::DownloadCallBack( void* pBuffer , size_t nSize , size_t nMemByte , void* pParam )
{
	FILE* fp = ( FILE* )pParam;
	size_t nWrite = fwrite( pBuffer , nSize , nMemByte , fp );

	return nWrite;
}

size_t FileDownloader::DownloadCallBackSTL( void * pBuffer , size_t nSize , size_t nMemByte , void * pParam )
{
	std::ofstream* pFout = static_cast< std::ofstream* > ( pParam );
	size_t realSize = nSize*nMemByte;
	char* byteBuffer = new char[ realSize + 1 ];
	if ( nullptr != byteBuffer )
	{
		memcpy( byteBuffer , pBuffer , realSize );
	}
	pFout->write( byteBuffer , realSize );
	pFout->flush();
	delete[] byteBuffer;
	byteBuffer = nullptr;
	return realSize;
}

bool FileDownloader::DownloadViaHTTP( std::string path,std::string uri  )
{	
	CURL *curl = curl_easy_init();
	curl_easy_setopt( curl , CURLOPT_URL , uri.c_str() );

	std::ofstream fout;
	fout.open( path.c_str() , std::ios::app | std::ios::binary );
	curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , DownloadCallBackSTL );
	curl_easy_setopt( curl , CURLOPT_WRITEDATA , &fout );
	curl_easy_setopt( curl , CURLOPT_VERBOSE , 1L );

	CURLcode retcCode = curl_easy_perform( curl );

	if ( retcCode != CURLE_OK )
	{
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror( retcCode ) << std::endl;
		system("pause");
	}
	
	fout.close();
	
	curl_easy_cleanup( curl );

	return !retcCode;
}

