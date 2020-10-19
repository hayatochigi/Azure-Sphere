# Device Twins
このサンプルは、IoT Hubのデバイスツイン機能をAzure Sphereに実装する。デバイスツインは以下の図がわかりやすい。

<p align="center">
  <img width="600" src="https://docs.microsoft.com/ja-jp/azure/iot-hub/media/iot-hub-devguide-device-twins/twin.png">
</p>

[IoT Hub のデバイス ツインの理解と使用](https://docs.microsoft.com/ja-jp/azure/iot-hub/iot-hub-devguide-device-twins)

デバイスツインは、クラウド上のプロパティとデバイス上のプロパティが同期するように設計されている。
デバイスがクラウドから切断されていても使用できて、次に接続したとき自動的にクラウド上の最新値を取得する

>接続されている場合や、最初に再接続されたときに、変更があるとデバイス アプリに直ちに通知されます。(上記ドキュメントより)

C2D/D2Cやダイレクトメソッドと違って、クラウドとの接続を前提にしていないので、間欠接続になってしまう環境でも使いやすいのではと思う。
ここで使用するデバイスツインの種類は主に3つ (もう一つは上記ドキュメントを参照)

|          | クラウド      |デバイス         |
|:--------:|:------------:|:------------:|
| Tag      | 読み書き       | アクセス不可      |
| desired  | 読み書き       | 読みonly       |
| reported | 読みonly       | 読み書き   |


## デバイスツイン実装の流れ
1. desiredプロパティ, reportedプロパティのコールバック関数を定義する  
desiredはIoT Hubでプロパティが更新されたら呼び出される。reportedは送信後に呼び出されて送信結果を確認できる。
```
**Desired**
static void twinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payLoad,
	size_t payLoadSize, void* userContextCallback)
  
**Reported**
static void reportStatusCallback(int status, void* userContextCallback)
```


2. desiredプロパティ更新時のコールバック関数を登録
```
IoTHubDeviceClient_LL_SetDeviceTwinCallback(iothubClient, twinCallback, NULL);
```

3. reportedプロパティを送信  
"reportstr"は該当するプロパティと値のみをJSON形式で送信する。送信時にreportedのコールバック関数も併せて実行される。
プロパティが存在しない場合は新たに追加されるので、事前追加などの操作は不要。
```
IoTHubDeviceClient_LL_SendReportedState(iothubClient, (const unsigned char*)reportstr, strlen(reportstr), reportStatusCallback, NULL);
```


4. IoT Hub上でdesiredプロパティを更新  
このサンプルを実行すると、デバイスツインの全体としては以下のようになる。タグの"connectionState"が、接続に応じてconnected/disconnectedと変化する。
"myval"の値を変更してIoT Hub上で保存すると、デバイスに最新値と共にノーティフィケーションが送られる。以下は長いので一部省略している。

```
{
  "deviceId": "AzMT3620",
  "etag": "AAAAAAAAACY=",
  "deviceEtag": "MTkwMDg3Nzgz",
  "status": "enabled",
  "statusUpdateTime": "0001-01-01T00:00:00Z",
  "connectionState": "Connected",
　.. (省略) ..
  "version": 41,
  "properties": {
    "desired": {
      "telemetryConfig": {},
      "test": "15",
      "myval": 123,
      "$metadata": {
        "$lastUpdated": "2020-10-18T08:06:30.3299654Z",
        "$lastUpdatedVersion": 38, 
        "myval": {
          "$lastUpdated": "2020-10-18T08:06:30.3299654Z",
          "$lastUpdatedVersion": 38
        }
      },
      "$version": 38
    },
    "reported": {
      "Prefecture": "Shimane",
      "$metadata": {
        "$lastUpdated": "2020-10-18T08:06:27.6069013Z",
        "Prefecture": {
          "$lastUpdated": "2020-10-18T08:06:27.6069013Z"
        }
      },
      "$version": 3
    }
  },
  "capabilities": {
    "iotEdge": false
  }
}
```

IoT Hub上でdesiredプロパティを更新すると、デバイス上で最新のプロパティ値を確認することができる。

<p align="center">
  <img width="240" src="https://github.com/hayatochigi/images/blob/master/IoT%20Hub%20Example/DeviceTwins%20-%20Portal.PNG">
</p>
<p align="center">
  <img width="600" src="https://github.com/hayatochigi/images/blob/master/IoT%20Hub%20Example/DeviceTwins%20-%20VStudio.png">
</p>
