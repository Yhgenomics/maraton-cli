#ifndef FILE_UPLOADER_H_
#define FILE_UPLOADER_H_

#include <string>
#include "curl.h"

class FileUploader
{
public:
    bool    UploadFileViaHttp( std::string taskID , std::string fileFullName , std::string postDest );

private:
    bool    Init();
    void    PreparePostMark( std::string taskID , std::string fileFullName );
    void    SetOptions( std::string postDest );
    void    PostFile();
    void    Deinit();

    // set -1 for no waiting
    void    SetTimeOut( long *milliseconds );

    CURL            *curl_;
    CURLM           *multi_handle_;
    curl_httppost   *post_;
    curl_httppost   *last_;
    curl_slist      *header_list_;
    std::string     expect_;
    int             running_flag;
    timeval         timeout_;
    long            timeout_ms_;
};

#endif 
