#ifndef FILE_DOWNLOADER_H_
#define FILE_DOWNLOADER_H_

#include <string>
#include "curl.h"

class FileDownloader
{
public:			
	bool	DownloadViaHTTP( std::string path , std::string uri );
	bool*	cancel_;
	FileDownloader( bool* cancel );
	~FileDownloader();

private:
	static size_t DownloadCallBack( void* pBuffer , size_t nSize , size_t nMemByte , void* pParam );
	static size_t DownloadCallBackSTL( void* pBuffer , size_t nSize , size_t nMemByte , void* pParam );	
};

#endif 
