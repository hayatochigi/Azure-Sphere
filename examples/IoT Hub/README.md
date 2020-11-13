# Azure Sphere IoT Hub Example
このサンプルは、Device Provisioning Serviceを介してIoT Hubへ接続し、MQTTプロトコルを用いて5秒毎にメッセージをIoT Hubへ送信する。

## 1. Azureセットアップ
Azure IoT HubとDevice Provisioning Service (DPS)をセットアップする。デバイスはまずDPSに問い合わせを行い、DPSは接続先IoT Hubの情報をデバイスへ返す。それを元にIoT Hubへデバイスが接続する。

DPSのメリットについてはこちらの方の説明がわかりやすくて好き。
[IoT Hub Device Provisioning Service とは？超概要](https://qiita.com/mstakaha1113/items/231c859d7427b466d4ad)

DPSを採用することで、デバイスのIoT Hubへの接続をほぼ自動化できる。事前にルート証明書をDPSへ登録しておく。個々のデバイスの認証には、ルート証明書から生成したリーフ証明書を使用する。デバイスはリーフ証明書をDPSへ渡し、DPSはルート証明書を用いてそれを検証する。問題がなければデバイスが登録される。

[デバイス プロビジョニング サービス、x.509 証明書、登録の概要](https://docs.microsoft.com/ja-jp/learn/modules/securely-provision-iot-devices-at-scale-with-device-provisioning-service/3-overview-dps-x509-certificates-enrollments)

##### Azure Sphereの証明書について
Azure Sphereでは、以降で紹介する(Microsoftのドキュメントどおりの)方法でしか、証明書を取り扱えない。  
[DPS zero touch provisioning using X.509 certificate on Azure Sphere](https://docs.microsoft.com/en-us/answers/questions/131979/dps-zero-touch-provisioning-using-x509-certificate.html)  
他のデバイスのように、オリジナルのルート証明書から生成したリーフ証明書は使えないようなので注意...

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

