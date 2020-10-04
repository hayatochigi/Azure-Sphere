// このサンプルはlibcurl APIを使用しているので、詳細はこちらを参考のこと
// libcurl - API https://curl.haxx.se/libcurl/c/

// 実行前にappidを"curl_eary_setopt"で定義すること

#include <applibs/log.h>
#include <curl/curl.h>
#include "parson.h"

// curlコマンドでデータ受信後に呼び出されるコールバック関数
size_t do_parse(char*, size_t, size_t, void*);

int main(void)
{
    CURL* curl;
    CURLcode curlCode;
    char returnChar[512];

    // libcurl APIを使用する際には、必ずこの初期化を行う
    curl = curl_easy_init();

    // 通信先のURLを定義
    // アプリケーションマニフェストで、"AllowedConnections"に追加する必要がある
    curlCode = curl_easy_setopt(curl, CURLOPT_URL, "https://api.openweathermap.org/data/2.5/weather?q=London,uk&appid={your id}");
    if (curlCode != CURLE_OK) {
        Log_Debug("Error at curl_easy_setopt\n");
    }

    // SSLの確認はしない
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

    // 実行後に呼び出すコールバック関数を定義
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, do_parse);

    // 実行
    curl_easy_perform(curl);
    if (curlCode != CURLE_OK) {
        Log_Debug("Error at curl_easy_perform\n");
    }
    
    curl_easy_cleanup(curl);

    return 0;
}

/// <summary>
/// curl_easy_performによって操作が実行され、サーバーからの応答があったのち
/// 呼び出されるコールバック関数。ここでJSON文字列の処理を行う
/// </summary>
/// 引数の詳細はこちら: https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
size_t do_parse(char* ptr, size_t size, size_t nmemb, void* stream) {

    JSON_Value* rootInfo;
    
    // ptrに取得した天気情報がJSON形式の文字列で格納されている
    // "json_parser_string"を使用してJSON_Valueへ変換する
    // "json_value_get_type"で確認すると、rootInfoのタイプは4(=JSONObject)になる
    rootInfo = json_parse_string(ptr);
    if (rootInfo == NULL) {
        Log_Debug("Error at json_parse_string\n");
    }

    // JSON_Valueからオブジェクトを取得
    JSON_Object* weatherInfo = json_value_get_object(rootInfo);

    // weather変数は配列オブジェクトのため、名前を指定してJSON_Arrayへ変換
    JSON_Array* weather = json_object_get_array(weatherInfo, "weather");
    // weather変数の0要素目の情報のみが必要なので、index0でオブジェクトを取得
    JSON_Object* weather_0 = json_array_get_object(weather, 0);

    // dotget関数はJSONの各変数にドット演算子を用いてアクセスできる
    char* location = json_object_dotget_string(weatherInfo, "name");
    double temp = json_object_dotget_number(weatherInfo, "main.temp");
    char* currentWeather = json_object_dotget_string(weather_0, "main");

    // 情報を表示
    Log_Debug("\n\nLocation: %s, temp = %0.2f, weather = %s\n\n", location, temp, currentWeather);    
    
    json_value_free(rootInfo);

}