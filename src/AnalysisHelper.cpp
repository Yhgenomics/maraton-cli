#include "AnalysisHelper.h"
#include <stdlib.h>
#include <curl.h>
#include <string>

using namespace MaratonCommon;

size_t AnalysisHelper::Post()
{
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:4243/containers/create");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"Image\": \"hello-world\"}");
        struct curl_slist *list = NULL;
        list = curl_slist_append(list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

size_t AnalysisHelper::Get()
{
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:4243/containers/json?all=1");
        struct curl_slist *list = NULL;
        list = curl_slist_append(list,"Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        res = curl_easy_perform(curl);
        curl_slist_free_all(list);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

}
