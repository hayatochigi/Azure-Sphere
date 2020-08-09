# Azure Sphere MT3620　セットアップ手順

[Azure Sphere の使用開始 | Microsoft Azure](
https://azure.microsoft.com/ja-jp/services/azure-sphere/get-started/)
を参考に、開発環境をセットアップする。

## 1. 環境セットアップ

### 1. Seeed Azure Sphere MT3620開発キットをUSBでPCにつなぐ
[開発キットをセットアップして Azure Sphere SDK for Windows をインストールする](https://docs.microsoft.com/ja-jp/azure-sphere/install/install-sdk?pivots=visual-studio)
に記載ある通り、USBケーブルでPCと開発キットをつなぐだけでドライバが自動でダウンロードされる。デバイスマネジャーを確認すると4つのUSBシリアルコンバーターが確認できる。
<p align="center">
  <img width="600" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Setup/1-1%20%E5%88%9D%E5%9B%9E%E6%8E%A5%E7%B6%9A%E5%BE%8C%E3%81%AE%E3%83%87%E3%83%90%E3%82%A4%E3%82%B9%E3%83%9E%E3%83%8D%E3%82%B8%E3%83%A3%E3%83%BC.png">
</p>

### 2. Azure Sphere SDKをインストールする
_**Azure_Sphere_Win_SDK_20_07.exe**_
を上記リンクからインストールする。この操作により、開発用のライブラリと、_**Azure Sphere Developer Command Prompt**_ がインストールされる。
<p align="center">
  <img width="480" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Setup/1-3%20Azure%20Sphere%20SDK%20License%20Agreement.png">
</p>
<p align="center">
  <img width="480" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Setup/1-4%20Azure%20Sphere%20SEK%20Installation%20Completed.png">
</p>

### 3. Visual Studio をインストールする
[無料の IDE と開発者ツール \| Visual Studio Community](https://visualstudio.microsoft.com/ja/vs/community/)

### 4. Visual Studio用Azure Sphereの拡張機能をインストールする
[Azure Sphere - Visual Studio Marketplace](https://marketplace.visualstudio.com/items?itemName=AzureSphereTeam.AzureSphereSDKforVisualStudio2019)
にアクセスし、 _**Visual Studio Extension for Azure Sphere**_ をインストールする
<p align="center">
  <img width="480" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Setup/1-6%20Azure%20Sphere%E6%8B%A1%E5%BC%B5%E6%A9%9F%E8%83%BD%E3%82%A4%E3%83%B3%E3%82%B9%E3%83%88%E3%83%BC%E3%83%AB.png">
</p>


## 2.デバイスを要求する
[デバイスを要求するクイックスタート-Azure Sphere](https://docs.microsoft.com/ja-jp/azure-sphere/install/claim-device)

デバイスは、Azure Sphereテナントに関連付けられる必要がある。
テナントとサブスクリプションの関係についてはこちらで分かりやすく説明してくれている。

[What is the difference between an Azure tenant and Azure subscription?](https://stackoverflow.com/questions/47307368/what-is-the-difference-between-an-azure-tenant-and-azure-subscription#:~:text=Azure%20tenant%20is%20a%20directory,subscriptions%2C%20but%20not%20vice%20versa.)


### 1. Azure Sphereにサインインする
初めてのログインなので、_**Azure Sphere Developer Command Prompt**_ から以下のように入力する
```
azsphere login --newuser example@az.com
```
> Registration successful. Press any key to log in with your new account.

Azure ADが別ウィンドウで立ち上がるので、先ほど入力したユーザー情報でログイン。アクセス許可を確認されるので、同様に問題なければ承諾する。
<p align="center">
  <img width="480" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Setup/1-10%20Azure%20Active%20Directory.png">
</p>
<p align="center">
  <img width="480" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Setup/1-11%20Azure%20AD%20Agreement.png">
</p>

完了するとAzure AD画面は閉じられ、cmdに以下のメッセージが表示される。
> Login successful as 'example@az.com'.\
> warn: You don't have access to any Azure Sphere tenants.\
> warn: Type 'azsphere tenant create --name <name>' or, if you have used Azure Sphere before, type 'azsphere tenant migrate'.


### 2. Azureテナントを選択する
次のリンクを参考に、Azure Sphereテナントを作成する。
[Azure Sphere テナントを作成する](https://docs.microsoft.com/ja-jp/azure-sphere/deployment/create-tenant)
デバイスがPCにUSBケーブルで接続済みであることを確認して、以下のコマンドを実行する。
```
azsphere tenant create --force --name mt3620
```
> warn: You have logged in with the following account:\
warn: example@az.com (0ef8954b-1234-5678-abcd-5a8efghjklm)\
warn: Do you want to use this account to create a new Azure Sphere tenant using the attached device?\
warn: You cannot change the tenant name 'mt3620' once it has been created.\
Enter 'yes' to continue. Enter anything else to exit.

「yes」を入力

> Created a new Azure Sphere tenant:\
 --> Tenant Name: mt3620\
 --> Tenant ID:   7c2c0fa6-1234-5678-abcd-efgh90d72555\
Selected Azure Sphere tenant 'mt3620' as the default.\
You may now wish to claim the attached device into this tenant using 'azsphere device claim'.

---
※tenant create時に発生したエラーの対応履歴
> Failed to retrieve device ID from attached device: 'Could not connect to the device; please ensure it is attached.'

[Troubleshoot Azure Sphere issues](https://docs.microsoft.com/en-us/azure-sphere/install/troubleshoot-installation)
接続されているデバイスを確認する
```
azsphere device list-attached
```
> No attached devices

デバイスマネージャーからはデバイスが見えている、MT3620のPower Indicatorは点灯している
```
azsphere device rescan-attached
```
> 1 device attached:\
--> Device ID: 698A60985xxxxxxx\
  --> Is responsive: yes\
  --> IP address: 192.168.35.2\
  --> Connection path: 133
  
  これでデバイスが正しく認識されたので、次のステップへと進む。
  
  ---
### 3. デバイスを要求する
```
azsphere device claim
```

> warn: You are claiming a device to the following tenant:\
warn: 7c2c0fa6-1234-5678-abcd-efgh90d72555 (MT3620)\
warn: Do you want to claim the device ID 698A60985xxxxxxx to this tenant?\
warn: You cannot change the Azure Sphere tenant this device is claimed to once this action has completed.\
Enter 'yes' to continue. Enter anything else to exit.

「yes」と入力

> Claiming device.\
Successfully claimed device ID '698A60985xxxxxxx' into tenant 'MT3620' with ID '7c2c0fa6-1234-5678-abcd-efgh90d72555'.

### 4. ネットワークの構成とデバイスOSの更新
wifiのSSID(ここでは「mySSID」)とキー(ここでは「myKey」)を準備して以下の操作を行う。
```
azsphere device wifi add --ssid mySSID --psk myKey
```

問題なく接続できると、以下のように表示される。

> warn: Your device's Azure Sphere OS version is deprecated. Run 'azsphere device recover' to erase your device and move it to a current OS version. See https://aka.ms/AzureSphereUpgradeGuidance for further advice and support.
Add network succeeded:
ID                  : 0\
SSID                : mySSID\
Configuration state : enabled\
Connection state    : unknown\
Security state      : psk\
Targeted scan       : False

device wifi show-statusでステータスを確認したかったが、
> warn: Your device's Azure Sphere OS version is deprecated. Run 'azsphere device recover' to erase your device and move it to a current OS version. See https://aka.ms/AzureSphereUpgradeGuidance for further advice and support.
error: Device error 15.2

実際以下のコマンドで確認してみると...
```
azsphere device show-deployment-status
```

> warn: Your device's Azure Sphere OS version is deprecated. Run 'azsphere device recover' to erase your device and move it to a current OS version. See https://aka.ms/AzureSphereUpgradeGuidance for further advice and support.
Your device is running Azure Sphere OS version TP4.2.1.\
The Azure Sphere Security Service is targeting this device with Azure Sphere OS version 20.07.\
warn: Your device is running Azure Sphere OS version TP4.2.1. The latest release version is 20.07.\
warn: Your device's Wi-Fi status could not be checked: Device error 15.2\
Go to https://aka.ms/AzureSphereUpgradeGuidance for further advice and support.

この時点での最新OSは20.07なので、かなり古いOSが動作している。こちらを参考にボードを更新する。
[初期の開発キットで OS を更新する](https://docs.microsoft.com/ja-jp/azure-sphere/resources/update-old-seeed-os)

```
azsphere device recover
```

> Downloading recovery images...
Download complete.\
Starting device recovery. Please note that this may take up to 10 minutes.\
Board found. Sending recovery bootloader.\
Erasing flash.\
Sending 17 images. (5438976 bytes to send)\
Sent 1 of 17 images. (5436600 of 5438976 bytes remaining)\
Sent 2 of 17 images. (5410588 of 5438976 bytes remaining)\
Sent 3 of 17 images. (5307636 of 5438976 bytes remaining)\
Sent 4 of 17 images. (5307244 of 5438976 bytes remaining)\
Sent 5 of 17 images. (5037264 of 5438976 bytes remaining)\
Sent 6 of 17 images. (5020780 of 5438976 bytes remaining)\
Sent 7 of 17 images. (4990728 of 5438976 bytes remaining)\
Sent 8 of 17 images. (2397456 of 5438976 bytes remaining)\
Sent 9 of 17 images. (885796 of 5438976 bytes remaining)\
Sent 10 of 17 images. (861220 of 5438976 bytes remaining)\
Sent 11 of 17 images. (758608 of 5438976 bytes remaining)\
Sent 12 of 17 images. (135796 of 5438976 bytes remaining)\
Sent 13 of 17 images. (70048 of 5438976 bytes remaining)\
Sent 14 of 17 images. (41164 of 5438976 bytes remaining)\
Sent 15 of 17 images. (32768 of 5438976 bytes remaining)\
Sent 16 of 17 images. (16384 of 5438976 bytes remaining)\
Sent 17 of 17 images. (0 of 5438976 bytes remaining)\
Finished writing images; rebooting board.\
Device ID: 698A60985D6C44C68C8F986AC5222529E5751A8793C47189DB61F62CF66FF3A8FBD1A2BAFC1646BB954EEA3CFBB385C8E29B72F7C31E67B689EF47FF1119BDC0\
Device recovered successfully.

---
以上でセットアップ完了。
