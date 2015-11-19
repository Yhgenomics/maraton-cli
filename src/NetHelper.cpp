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
        result = curl_golbal_init( CURL_GLOBAL_ALL );
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

    size_t NetHelper::Get( NetHelperParams params )
    {
        if (NetHelperParams::XMark::GET == params.Option)
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
                curl_easy_setopt( curl , CURLOPT_HTTPHEADER , HeaderList                );
                curl_easy_setopt( curl , CURLOPT_HTTPGET    , 1L                        );
                curl_easy_setopt( curl , CURLOPT_VERBOSE    , params.verbose ? 1L : 0L  );

                result = curl_easy_perform( curl );
                curl_slist_free_all( list )；
                curl_easy_cleanup  ( curl );
                if( CURLE_OK != result )
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

    size_t Post( NetHelperParams params )
    {
        return 0;
    }
}
