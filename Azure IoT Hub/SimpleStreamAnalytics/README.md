# Stream Analytics with Azure Sphere
Azure Sphereから送信したテレメトリを[Stream Analytics](https://docs.microsoft.com/ja-jp/azure/stream-analytics/stream-analytics-introduction)のクエリによってフィルタし、
条件にマッチしたテレメトリをBlobストレージへ保存する。
単純なフィルタのみでなく、Stream Analyticsに組み込まれた[Tumbling Window](https://docs.microsoft.com/ja-jp/stream-analytics-query/tumbling-window-azure-stream-analytics)などの組み込み関数を使用して、データの処理も可能となる。

本来はEvent Hubをかましたいんだけど、コストの関係でEvent Hubは未使用。。。

<p align="center">
  <img width="800" src="https://github.com/hayatochigi/images/blob/master/IoT%20Hub%20Example/Simple%20Stream%20Analytics%20Arch.png">
</p>

テレメトリをStream Analyticsのクエリで扱うためには、メッセージはJSON形式になっている必要がある。[Azure-Sphere/Azure IoT Hub/Telemetry Communication at master](https://github.com/hayatochigi/Azure-Sphere/tree/master/Azure%20IoT%20Hub/Telemetry%20Communication)でも検証したように、IoT Hubへのメッセージは自動的にJSON形式になるわけではないので、parson.hなどを使用して明示的にJSON形式へ変換しておく必要がある。

```
    // JSON形式でメッセージを作成
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    (void)json_object_dotset_string(root_object, "Temperature",buf);
    char* json_mssg = json_serialize_to_string(root_value);
    json_value_free(root_value);
```

Stream Analyticsの出力設定で、Blobストレージにどのようなフォルダ階層を作成してテレメトリを保存するか設定できる。例えば今回の場合は、Hotフォルダ下に保存するよう設定したので、Stream Analyticsジョブの開始後は、以下のようにテレメトリが保存される。

<p align="center">
  <img width="800" src="https://github.com/hayatochigi/images/blob/master/IoT%20Hub%20Example/Stream_to_Blob.PNG">
</p>

## IoT Hubのコンシューマーグループ
IoT HubとStream Analyticsは、IoT Hubのコンシューマーグループを介してデータを受け渡す。  
[デバイスからクラウドへのメッセージを組み込みのエンドポイントから読み取る](https://docs.microsoft.com/ja-jp/azure/iot-hub/iot-hub-devguide-messages-read-builtin)

```
コンシューマーグループって2つ作ると、複製されたデータが2つ存在するの？
```
Steram Analyticsで2つのコンシューマーグループ入力を、異なるBlobパスへ保存してみた。同じデータがBlobに保存されている様子が確認できる。
<p align="center">
  <img width="600" src="https://github.com/hayatochigi/images/blob/master/IoT%20Hub%20Example/stream_iothub_cold.PNG">
</p>
<p align="center">
  <img width="600" src="https://github.com/hayatochigi/images/blob/master/IoT%20Hub%20Example/stream_iothub_hot.PNG">
</p>


