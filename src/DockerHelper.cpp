#include "DockerHelper.h"
#include "NetHelper.h"
#include "json.hpp"
#include "curl.h"
#include <string>
#include <iostream>
#include <vector>
#include <unistd.h>

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

    size_t DockerHelper::Create( const string &dest, const string &image, const vector< string > &binds, const vector< string > &environment, vector< string > * const response )
    {
        json postJson;
        json hostConfig;
        postJson  [ "Image"      ] = image;
        postJson  [ "Env"        ] = environment;
        hostConfig[ "Binds"      ] = binds;
        postJson  [ "HostConfig" ] = hostConfig;

        // std::cout << "!@#$!@#$!@#$!@#$!@#$!@#$!test only json like this" << std::endl;
        // std::cout << postJson.dump(4) << std::endl;
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

        NetHelper::instance()->PostViaHTTP( startParams, response );
        return 0;
    }

    size_t DockerHelper::Wait( const string &dest, const string &containerID, vector< string > * const response )
    {
        NetHelperParams waitParams;
        waitParams.url      = dest + "/containers/" + containerID + "/wait";
        waitParams.option   = NetHelperParams::XMark::POST;

        NetHelper::instance()->PostViaHTTP( waitParams, response );
        return 0;
    }

    size_t DockerHelper::Watch( const string &dest, const string &containerID, vector< string> * const response )
    {
        NetHelperParams watchParams;

        watchParams.url     = dest +  "/containers/" + containerID + "/attach?stderr=1&stdout=1&stream=1";
        watchParams.option  = NetHelperParams::XMark::POST;

        NetHelper::instance()->PostViaHTTP( watchParams, response );
    }

    size_t DockerHelper::Run( const string &dest, const string &image, const vector< string > &binds, const vector< string  > &environment )
    {
        vector< string > response;
        Create( dest, image, binds, environment, &response );

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

                    response.clear();
                    Wait ( dest, ContainerID, &response );

                    for( auto result : response )
                    {
                        auto oneResult = json::parse( result );
                        return oneResult["StatusCode"].get<int>();
                    }
                    break;
                }
            }
        }
        return 1;
    }
}
