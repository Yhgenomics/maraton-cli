#include "NetHelper.h"
#include "curl.h"
#include <string>
#include <iostream>

namespace MaratonCommon
{
    using string = std::string;

    size_t NetHelper::Init()
    {
        CURLcode result;
        result = curl_global_init( CURL_GLOBAL_ALL );
        if( CURLE_OK != result )
        {
            std::cout << "curl_golbal_init() failed "<< curl_easy_strerror( result ) << std::endl;
            return 1;
        }
        else
        {
            return 0;
        }
    }

    size_t NetHelper::DeInit()
    {
        curl_global_cleanup();
        return 0;
    }

    size_t NetHelper::GetViaHTTP( const NetHelperParams &params )
    {
        if (NetHelperParams::XMark::GET == params.option)
        {
            CURL     *curl;
            CURLcode result;
            curl = curl_easy_init();
            if ( curl )
            {
                curl_easy_setopt( curl, CURLOPT_URL, params.url.c_str() );
                struct curl_slist *headerList = nullptr;
                for (auto header : params.headers)
                {
                    headerList = curl_slist_append( headerList, header.c_str() );
                }
                curl_easy_setopt( curl , CURLOPT_HTTPHEADER , headerList                );
                curl_easy_setopt( curl , CURLOPT_HTTPGET    , 1L                        );
                curl_easy_setopt( curl , CURLOPT_VERBOSE    , params.verbose ? 1L : 0L  );

                result = curl_easy_perform( curl );
                curl_slist_free_all( headerList );
                curl_easy_cleanup  ( curl );
                if ( CURLE_OK != result )
                {
                    std::cout << "curl_easy_perform failed " << curl_easy_strerror( result ) << std::endl;
                return 2;
                }
            }
            return 0;
        }
        else
        {
            return 1;
        }
    }

    size_t NetHelper::PostViaHTTP( const NetHelperParams &params )
    {
        if ( NetHelperParams::XMark::POST == params.option )
        {
            std::cout << "abqasdfasdf" << std::endl;
            CURL     *curl;
            CURLcode result;
            curl = curl_easy_init();
            if ( curl )
            {
                curl_easy_setopt( curl , CURLOPT_POST       , 1L                         );
                curl_easy_setopt( curl , CURLOPT_VERBOSE    , params.verbose ? 1L : 0L   );
                curl_easy_setopt( curl , CURLOPT_URL        , params.url.c_str()         );
                curl_easy_setopt( curl , CURLOPT_POSTFIELDS , params.post_fields.c_str() );

                struct curl_slist *headerList = nullptr;
                for (auto header : params.headers)
                {
                    headerList = curl_slist_append( headerList, header.c_str() );
                }
                curl_easy_setopt( curl , CURLOPT_HTTPHEADER , headerList                 );
                result = curl_easy_perform( curl );

                curl_slist_free_all( headerList );
                curl_easy_cleanup( curl );
                if ( CURLE_OK != result )
                {
                    std::cout << "curl_easy_perform failed " << curl_easy_strerror( result ) << std::endl;
                    return 2;
                }
            }
            return 0;
        }
        else
        {
            return 1;
        }
    }
}
