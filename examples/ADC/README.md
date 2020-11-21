# Azure Sphere Simple ADC Example
サーミスタによって温度を測定するアプリケーション。開発キットのハードウェアサポートに関しては、複数のドキュメントがあるがADCが使えるのか結局よく分からなかった。[Azure Sphere MT3620 開発キット](https://seeedjp.github.io/Wiki/MT3620/)ではサポートしていない旨の記述があり、[MT3620 reference development board (RDB) user guide](https://docs.microsoft.com/ja-jp/azure-sphere/hardware/mt3620-user-guide)ではサポートしている記述がある。

このサンプルで、MT3620開発キットでADCによる電圧測定ができることを確認した。

[高度なアプリケーションで ADC を使用する | Microsoft Azure](https://docs.microsoft.com/ja-jp/azure-sphere/app-development/adc)

注意点として、 _開発キットのJ1はショートしておく必要がある。_ オープンでは、Pin1に外部から1.8Vのリファレンス電圧を入れなくては電圧が正常に測定できない。ショートすることで、内部の2.5Vリファレンスを使用し、測定値は期待したものになる。

## 1. 環境構築

### 1. ハードウェア定義ヘッダーファイルを作成する
1. プロジェクトに"Hardware"フォルダを作成する
2. Hardwareフォルダ内に、"hardware_definition.json"ファイルを作成する
3. [ハードウェア定義](htt\/ｐｓps://docs.microsoft.com/ja-jp/azure-sphere/hardware/hardware-abstraction)を参考に、JSONファイルを編集する
4. Azure Sphere Developer Command Prompt から以下のコマンドを実行する
```
azsphere hardware-definition generate-header --input hardware_definition.json
```
5. Hardware > inc > hw フォルダに、ハードウェア定義ヘッダーファイルが生成されたことを確認する


### 2. アプリケーションマニフェストを記述する
1. app_manifest.jsonを開く
2. "Capabilities"項目にADC定義を追加する。Adcの名前は、ハードウェア定義ヘッダーファイルに定義したものを使用する。
```
{
  "SchemaVersion": 1,
  "Name": "AzureSphere_Simple_ADC",
  "ComponentId": "xxx4a18b-e6e6-44a2-91cd-730e1210adce",
  "EntryPoint": "/bin/app",
  "CmdArgs": [],
  "Capabilities": {
    "Adc": [ "$ADC" ],
    "AllowedApplicationConnections": []
  },
  "ApplicationType": "Default"
}
```


### 3. CMakeLists.txtを編集する
CMakeLists.txtに以下の一文を追記する。
※ これがないと、main.cからハードウェア定義ヘッダーファイルが参照できない...
```
azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "Hardware" TARGET_DEFINITION "hardware_definition.json")
```


## 2. コーディングする
例えば以下のように記述して、エラーなく測定値が出力されることを確認する。
```
#include <applibs/log.h>
#include <applibs/adc.h>
#include "hw/hardware_definition.h"


int main(void)
{
    // Please see the extensible samples at: https://github.com/Azure/azure-sphere-samples
    // for examples of Azure Sphere platform features
    Log_Debug("High Level Application\n");

    int adcfd = ADC_Open(ADC);
    int BitDepth = ADC_GetSampleBitCount(adcfd, AI0);
    
    uint32_t value = 0;
    int result = ADC_Poll(adcfd, 0, &value);

    float voltage = ((float)value * 2.5) / (float)((1 << BitDepth) - 1);

    Log_Debug("Voltage is %.6f\n", voltage);
    return 0;
}
```

<p align="center">
  <img width="800" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Example/Azure%20Sphere%20-%20ADC%20Result.png">
</p>

