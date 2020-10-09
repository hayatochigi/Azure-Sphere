# Azure Sphere MQTT Example
このサンプルは、Azure Sphereから[ThingSpeak](https://thingspeak.com/)へMQTT v3.1.1を用いてデータをパブリッシュし、ThingSpeak上のデータを更新する。
MQTTのライブラリには、非常にシンプルでドキュメントも充実しているLiamBindle氏の[MQTT-C](https://github.com/LiamBindle/MQTT-C)を使用する。

ThingSpeakへアクセスするためには、アプリケーションマニフェストで以下のように接続を許可する必要がある。

[アプリケーション マニフェスト
](https://docs.microsoft.com/ja-jp/azure-sphere/app-development/app-manifest)
```
"Capabilities": {
    "AllowedApplicationConnections": [],
    "AllowedConnections": [ "mqtt.thingspeak.com" ],
```

<p align="center">
  <img width="800" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Example/ThingSpeak%20-%20Result.PNG">
</p>
