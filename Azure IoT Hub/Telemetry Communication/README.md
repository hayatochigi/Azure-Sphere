# Temeletry Communication
Azure IoT HubとAzure Sphere MT3620を接続し、デバイスtoクラウド(D2C)またはクラウトtoデバイス(C2D)によって相互にテレメトリ(メッセージ)のやり取りを行う。
接続にはDPSは使用せず、デバイスを直接IoT Hubへ接続する。DPSを使用する場合はこちら [Azure Sphere IoT Hub Example](https://github.com/hayatochigi/Azure-Sphere/tree/master/examples/IoT%20Hub)

[IoT Hub で device-to-cloud および cloud-to-device メッセージを送信する](https://docs.microsoft.com/ja-jp/azure/iot-hub/iot-hub-devguide-messaging)  
[テレメトリを送信および受信するコードを記述する](https://docs.microsoft.com/ja-jp/learn/modules/remotely-monitor-devices-with-azure-iot-hub/3-write-code-device-telemetry)

MT3620からIoT Hubへはテレメトリをメッセージ文字列としてMQTTプロトコルで送信する。IoT HubからMT3620へは、Portal上のマニュアル操作でテレメトリをメッセージ文字列として、加えてKey-Value形式のプロパティを送信する。

<p align="center">
  <img width="400" src="https://github.com/hayatochigi/images/blob/master/IoT%20Hub%20Example/IoT%20Hub%20TelComm%20-%20C2D%20message.PNG">
</p>

サンプルを実行すると、C2Dメッセージとプロパティを確認できる。D2Cメッセージは[ServiceBusExplorer](https://github.com/paolosalvatori/ServiceBusExplorer/releases)を用いると簡単に確認できる。

<p align="center">
  <img width="800" src="https://github.com/hayatochigi/images/blob/master/IoT%20Hub%20Example/IoT%20Hub%20TelComm%20Result.png">
</p>
