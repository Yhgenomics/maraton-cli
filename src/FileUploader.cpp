#include "FileUploader.h"
#include <iostream>

bool    FileUploader::UploadFileViaHttp( std::string taskID , std::string fileFullName , std::string postDest )
{
    if ( Init() )
    {
        PreparePostMark( taskID , fileFullName );
        SetOptions( postDest );
        PostFile();
    }
    Deinit();
    return false;
}

bool    FileUploader::Init()
{
    post_           = NULL;
    last_           = NULL;
    header_list_    = NULL;
    expect_         = "Expect:";
    curl_           = curl_easy_init();
    multi_handle_   = curl_multi_init();
    header_list_    = curl_slist_append( header_list_ , expect_.c_str() );
    if ( curl_ && multi_handle_ )
        return true;
    else
        return false;
}

void    FileUploader::PreparePostMark( std::string taskID , std::string fileFullName )
{
    curl_formadd( &post_ ,
                  &last_ ,
                  CURLFORM_COPYNAME , "sendfile"            ,
                  CURLFORM_FILE     , fileFullName.c_str()  ,
                  CURLFORM_END );

    curl_formadd( &post_ ,
                  &last_ ,
                  CURLFORM_COPYNAME     , "taskid"          ,
                  CURLFORM_COPYCONTENTS , taskID.c_str()    ,
                  CURLFORM_END );

    curl_formadd( &post_ ,
                  &last_ ,
                  CURLFORM_COPYNAME     , "submit"          ,
                  CURLFORM_COPYCONTENTS , "send"            ,
                  CURLFORM_END );
}

void    FileUploader::SetOptions( std::string postDest )
{
    curl_easy_setopt( curl_ , CURLOPT_URL           , postDest.c_str()  );
    curl_easy_setopt( curl_ , CURLOPT_VERBOSE       , 0L                );
    curl_easy_setopt( curl_ , CURLOPT_HTTPHEADER    , header_list_      );
    curl_easy_setopt( curl_ , CURLOPT_HTTPPOST      , post_             );

    curl_multi_add_handle( multi_handle_ , curl_ );
}

void    FileUploader::PostFile()
{
    curl_multi_perform( multi_handle_ , &running_flag );
    do
    {
        fd_set      fdread;     FD_ZERO( &fdread );
        fd_set      fdwrite;    FD_ZERO( &fdwrite );
        fd_set      fdexcep;    FD_ZERO( &fdexcep );

        long timeo = -1L;
        SetTimeOut( &timeo );

        int         maxfd       = -1;
        CURLMcode   curlmCode   = curl_multi_fdset( multi_handle_ , &fdread , &fdwrite , &fdexcep , &maxfd );

        if ( curlmCode != CURLM_OK )
        {
            fprintf( stderr , "curl_multi_fdset() failed, code %d.\n" , curlmCode );
            break;
        }

        int selectResult;

        if ( maxfd == -1 )
        {
#ifdef _WIN32
            Sleep( 100 );
            selectResult = 0;
#else
            timeval wait = { 0, 100 * 1000 };
            selectResult = select( 0 , NULL , NULL , NULL , &wait );
#endif
        }
        else
        {
            selectResult = select( maxfd + 1 , &fdread , &fdwrite , &fdexcep , &timeout_ );
        }

        switch ( selectResult )
        {
            case -1:
                printf( "Select Error\n" );
                break;
            case 0:
            default:
                curl_multi_perform( multi_handle_ , &running_flag );
                break;
        }
    }
    while ( running_flag );
}

void    FileUploader::Deinit()
{
    if ( multi_handle_ ) { curl_multi_cleanup( multi_handle_ ); }
    if ( curl_         ) { curl_easy_cleanup( curl_ );          }
    if ( post_         ) { curl_formfree( post_ );              }
    if ( header_list_  ) { curl_slist_free_all( header_list_ ); }
}

void    FileUploader::SetTimeOut( long *milliseconds )
{
    timeout_ms_		 = *milliseconds;
    timeout_.tv_sec	 = 1;
    timeout_.tv_usec = 0;

    curl_multi_timeout( multi_handle_ , milliseconds );

    if ( *milliseconds >= 0 )
    {
        timeout_.tv_sec = *milliseconds / 1000;
        if ( timeout_.tv_sec > 1 )
            timeout_.tv_sec = 1;
        else
            timeout_.tv_usec = ( *milliseconds % 1000 ) * 1000;
    }
}
