#include "bilibili_api.h"

#include <obs-module.h>
#include <curl/curl.h>

#include <string>
#include <sstream>

/* =============================
   curl write callback
============================= */
static size_t write_cb(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    auto* stream = static_cast<std::string*>(userdata);
    stream->append(ptr, size * nmemb);
    return size * nmemb;
}

/* =============================
   获取房间信息
   GET https://api.live.bilibili.com/room/v1/Room/get_info
============================= */
std::string bilibili_api_get_room_info(const std::string& room_id)
{
    CURL* curl = curl_easy_init();
    if (!curl) {
        blog(LOG_ERROR, "[bilibili-api] curl_easy_init failed");
        return {};
    }

    std::ostringstream url;
    url << "https://api.live.bilibili.com/room/v1/Room/get_info"
        << "?room_id=" << room_id;

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "OBS-Bilibili-Plugin/1.0");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        blog(LOG_ERROR,
             "[bilibili-api] curl error: %s",
             curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return {};
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    if (http_code != 200) {
        blog(LOG_ERROR,
             "[bilibili-api] http error: %ld",
             http_code);
        return {};
    }

    blog(LOG_DEBUG,
         "[bilibili-api] room %s info ok (%zu bytes)",
         room_id.c_str(), response.size());

    return response;
}
