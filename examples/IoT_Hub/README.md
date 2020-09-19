# Azure Sphere IoT Hub Example
このサンプルは、Device Provisioning Serviceを介してIoT Hubへ接続し、MQTTプロトコルを用いて5秒毎にメッセージをIoT Hubへ送信する。

## 1. Azureセットアップ
Azure IoT HubとDevice Provisioning Service (DPS)をセットアップする。
このサンプルでは、将来的な利便性を考えIoT Hubへの直接接続ではなく、DPSを介した接続を行う。

DPSのメリットについてはこちらの方の説明がわかりやすくて好き。
[IoT Hub Device Provisioning Service とは？超概要](https://qiita.com/mstakaha1113/items/231c859d7427b466d4ad)

### 1. IoT HubとDPSの作成
この資料の通り行うだけ。資料わかりやすくて助かります。

[クイック スタート:Azure Portal で IoT Hub Device Provisioning Service を設定する](https://docs.microsoft.com/ja-jp/azure/iot-dps/quick-setup-auto-provision)

コード内で使用するのでIoTHubのホスト名と、DPSのスコープIDを概要ページから確認して控えておく。


### 2. 証明書周りの整備
この資料の通り行うだけ。

[Azure Sphere 用の Azure IoT ハブを設定する](https://docs.microsoft.com/ja-jp/azure-sphere/app-development/setup-iot-hub)


### 3. テナントIDを確認する
```
azsphere tenant list
```
コード内で使用するので、テナントIDを控えておく。


## 2. コードを修正する
- main.c内の"scopeId"定義
- アプリケーションマニフェスト内のIoT Hubホスト名とテナントID

上記2点を修正してコードを実行する

## 3. メッセージを確認する
[Service Bus Explorer](https://github.com/paolosalvatori/ServiceBusExplorer/releases)
を使用することで送信したメッセージを簡単に確認できる。

Actions >> Create IoT Hub Listner

から、IoT Hubの接続文字列を使用してIoT Hubへ接続できる。実行するとこんな感じ

<p align="center">
  <img width="800" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Example/Service%20Bus%20Explorer%20Result.png">
</p>

IoT Hubのビュアーを見ると、きちんとメッセージが消費されていたり接続デバイス数が変化していることが確認できる。

<p align="center">
  <img width="800" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Example/IoT%20Hub%20Viewer.PNG">
</p>

