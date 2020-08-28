// MQTT V3.1 プロトコル仕様
// http://public.dhe.ibm.com/software/dw/jp/websphere/wmq/mqtt31_spec/mqtt-v3r1_ja.pdf

// MQTT クライアント・アプリケーションでのコールバックと同期
// https://www.ibm.com/support/knowledgecenter/ja/SSFKSJ_9.1.0/com.ibm.mq.dev.doc/q029020_.htm

// “Internet of Things” モノとモノが繋がるインターネットで広がる”Connected Life”
// http://ngm2m.jp/m2m/files/symp2014_suzuki_pm.pdf


#include <stdio.h>
#include <string.h>
#include <applibs/log.h>
#include <stdbool.h>
#include "wolfmqtt/wolfmqtt/mqtt_client.h"

#define MAX_BUFFER_SIZE         1024
#define DEFAULT_CMD_TIMEOUT_MS  1000
#define BROCKER_TIMEOUT 5000
#define CLIENT_ID "abcdefghiuvwxyz"

// From ThingSpesk Settings
#define HOST "iot.eclipse.org"              // ThingSpeak™のMQTT ブローカーはmqtt.thingspeak.com URL上にある
#define PORT 1883                           // ポートは1883 https://jp.mathworks.com/help/thingspeak/mqtt-basics.html
#define USERNAME "abc"                      // My Profile page
#define PASSWORD "1234"                     // My Profile page
#define WRITE_API_KEY "1234abcd"            // My Channels > User Channel > API Keys 
#define ChannelID "1234567"                 // My Channels > User Channel > Channel Settings

/// <summary>
/// MqttClient関数実行時に呼び出されるMQTTCallback関数
/// </summary>
static int mqttclient_message_cb(MqttClient* client, MqttMessage* msg, byte msg_new, byte msg_done)
{
    return MQTT_CODE_SUCCESS;
}
static int mqttclient_tls_cb(MqttClient* client) {
    return MQTT_CODE_SUCCESS;
}

void ErrorHandler(char func_name[], enum MqttPacketResponseCodes error_code);

int main(void){

    // 関数の実行結果を受け取り、エラー判定を行うための変数
    int result = 0;

    MqttClient myMqttClient;
    MqttNet myMqttNet;
    byte* tx_buf = NULL, * rx_buf = NULL;
 
    // 送受信バッファを確保
    tx_buf = malloc(MAX_BUFFER_SIZE);
    rx_buf = malloc(MAX_BUFFER_SIZE);

    // MqttNetはConnect, Read, Write, Disconnectにおけるネットワークコールバック関数を定義する。
    myMqttNet.connect = mqttclient_message_cb;
    myMqttNet.read = mqttclient_message_cb;
    myMqttNet.write = mqttclient_message_cb;
    myMqttNet.disconnect = mqttclient_message_cb;

#pragma region Initialize
    // ドキュメント手順3: MqttClient_Initを呼び出す
    // MqttClient引数は初期化されていなくてよい
    result = MqttClient_Init(&myMqttClient, &myMqttNet, mqttclient_message_cb,
        tx_buf, MAX_BUFFER_SIZE, rx_buf, MAX_BUFFER_SIZE, DEFAULT_CMD_TIMEOUT_MS);
    ErrorHandler("MqttClient_Init", result);

    // ドキュメント手順4: ネットワーク上のブローカーと接続するためにMqttClient_NetConnectを呼び出す
    /* cannot use TLS unless ENABLE_MQTT_TLS is defined */
    // TLS接続の場合はコールバック関数で証明書を指定する
    result = MqttClient_NetConnect(&myMqttClient, HOST, PORT, BROCKER_TIMEOUT, false, mqttclient_tls_cb);
    ErrorHandler("MqttClient_NetConnect", result);

    // ドキュメント手順5: MqttConnect構造体を作成して、MqttClient_Connectを呼び出す    
    MqttConnect ConnectInfo;
    ConnectInfo.stat = MQTT_MSG_BEGIN;

    // キープアライブタイマーは、クライアントから一定時間のメッセージがない場合、
    // サーバーがそのクライアントとの接続を切断する際の時間です。
    ConnectInfo.keep_alive_sec = 30;
    // クライアントが切断されたときに、サーバーがそのクライアントを覚えているか指定します。
    // true = 覚えていない
    // false = 覚えている
    ConnectInfo.clean_session = true;
    ConnectInfo.client_id = CLIENT_ID;

    // プロトコルバージョンはv3.1.1を使用するため、定義コメントにある通りデフォルトの4を指定
    ConnectInfo.protocol_level = 4;

    // Last Will & Testament (遺言メッセージ)
    // 予期しないクライアントの切断時に、指定したトピックへ自動的にメッセージを送信する
    // クライアントは事前にlwtメッセージをサーバーへ登録しておく
    ConnectInfo.enable_lwt = false;
    //ConnectInfo.lwt_msg;

    // 接続情報
    ConnectInfo.username = USERNAME;
    ConnectInfo.password = PASSWORD;

    ConnectInfo.ack.stat = MQTT_MSG_WAIT;
    ConnectInfo.ack.flags = 0;
    ConnectInfo.ack.return_code = 0;

    result = MqttClient_Connect(&myMqttClient, &ConnectInfo);
    ErrorHandler("MqttClient_Connect", result);

    //result = MqttClient_Ping(&myMqttClient);
    //ErrorHandler("MqttClient_Ping", result);
#pragma endregion
      
#pragma region Publish
    // メッセージをパブリッシュする

    MqttPublish myPublish;
    // retain: あとからサブスクライブしてきた場合にも送れるように、最後のメッセージを保持する
    myPublish.retain = true;
    // qos: Quality Of Service。このサンプルではパブリッシュの確認は不要なのでQos0で定義する
    myPublish.qos = MQTT_QOS_0;
    myPublish.duplicate = 0;

    // topic_name: トピック名。使用するBrokerのマニュアルを参照
    //myPublish.topic_name = "channels/" + ChannelID + "/publish/" + WRITE_API_KEY;

    myPublish.packet_id = 0;
    myPublish.buffer = (byte*)"123";
    myPublish.buffer_len = strlen("123");
    
    result = MqttClient_Publish(&myMqttClient, &myPublish);
    ErrorHandler("MqttClient_Publish", result);

    result = MqttClient_WaitMessage(&myMqttClient, BROCKER_TIMEOUT);
    ErrorHandler("MqttClient_WaitMessage", result);
#pragma endregion

#pragma region Close
    result = MqttClient_Disconnect(&myMqttClient);
    ErrorHandler("MqttClient_Disconnect", result);
    result = MqttClient_NetDisconnect(&myMqttClient);
    ErrorHandler("MqttClient_NetDisconnect", result);

    free(tx_buf);
    free(rx_buf);
#pragma endregion

    return 0;
}

/// <summary>
/// MQTT関数の実行結果を受け取り、成功コード以外の場合はエラーメッセージを返します。
/// </summary>
/// <param name="func_name">対象の関数名</param>
/// <param name="error_code">関数が返した状態コード</param>
void ErrorHandler(char func_name[], enum MqttPacketResponseCodes error_code) {
    if (error_code != MQTT_CODE_SUCCESS) {
        printf("%s: %s (%d)\n", func_name, MqttClient_ReturnCodeToString(error_code), error_code);
        return error_code;
    }
}
