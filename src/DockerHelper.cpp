#include "DockerHelper.h"
#include "NetHelper.h"
#include "json.hpp"
#include "curl.h"
#include <string>
#include <iostream>
#include <vector>

namespace MaratonCommon
{
    using std::string;
    using nlohmann::json;
    using std::cout;
    using std::endl;

    size_t DockerHelper::Pull( const string &dest, const string& source )
    {
        NetHelperParams pullParams;

        pullParams.headers.clear();
        pullParams.headers.push_back( kDockerHeader );

        pullParams.url      = dest + kCreateImage + kParamsToken + kFromImage + source;
        pullParams.option   = NetHelperParams::XMark::POST;

        vector< string > response;
        NetHelper::instance()->PostViaHTTP( pullParams ,&response );
        return 0;
    }

    size_t DockerHelper::Create( const string &dest, const string &image, const vector< string > &environment, vector< string > * const response )
    {
        json postJson;
        postJson[ "Image" ] = image;
        postJson[ "Env"   ] = environment;

        NetHelperParams createParams;
        createParams.headers.clear();
        createParams.headers.push_back( kDockerHeader );

        createParams.url            = dest + kCreateContianer;
        createParams.post_fields    = postJson.dump();
        createParams.option         = NetHelperParams::XMark::POST;

        NetHelper::instance()->PostViaHTTP( createParams ,response );

        return 0;
    }

    size_t DockerHelper::Start( const string &dest, const string &containerID, vector< string > * const response )
    {
        NetHelperParams startParams;

        startParams.url     = dest + "/containers/" + containerID + "/start";
        startParams.option  = NetHelperParams::XMark::POST;

        NetHelper::instance()->PostViaHTTP( startParams ,response );
        return 0;
    }

    size_t DockerHelper::Run( const string &dest, const string &image, const vector< string  > &environment )
    {
        vector< string > response;
        Create(dest,image,environment, &response);

        string ContainerID = "";
        for ( auto item : response )
        {
            auto oneResponse = json::parse( item );
            if( oneResponse.find("Id") != oneResponse.end() )
            {
                ContainerID = oneResponse["Id"].get<string>();
                if ( !ContainerID.empty() )
                {
                    response.clear();
                    Start( dest, ContainerID, &response );
                    break;
                }
            }
        }
        return 0;
    }
}
