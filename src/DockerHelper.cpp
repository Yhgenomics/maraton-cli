#include "DockerHelper.h"
#include "NetHelper.h"
#include "json.hpp"
#include "curl.h"
#include <string>
#include <iostream>

namespace MaratonCommon
{
    using std::string;
    using nlohmann::json;
    using std::cout;
    using std::endl;
    size_t DockerHelper::Create( const string &dest, const string &image, const vector< string > &environment )
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
        NetHelper::instance()->PostViaHTTP( createParams );
        return 0;
    }
}
