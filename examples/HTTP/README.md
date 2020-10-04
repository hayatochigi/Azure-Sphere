# Azure Sphere HTTP Example
このサンプルは、Azure Sphere上でcurlコマンドによるHTTPリクエストを行い、[OpenWeather](https://openweathermap.org/)へ天気を問い合わせる。
受け取ったレスポンスはJSON形式のため、[parson](http://kgabis.github.io/parson/)ライブラリを用いて解析を行い、必要な情報を取得する。
OpenWeather APIについてはこちら [How to start](https://openweathermap.org/appid)

Azure SphereでOpenWeatherへアクセスするためには、アプリケーションマニフェストに以下の記述を行う。

[アプリケーション マニフェスト
](https://docs.microsoft.com/ja-jp/azure-sphere/app-development/app-manifest)
```
"Capabilities": {
    AllowedConnections": [ "api.openweathermap.org" ]
    },
```

<p align="center">
  <img width="800" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Example/HTTP-result.png">
</p>

JSON形式のレスポンスは以下のようになる。parsonを用いると、各JSON変数にドット演算子を使用してアクセスすることができる。

```
{
    "coord": {
        "lon": -0.13,
        "lat": 51.51
    },
    "weather": [
        {
            "id": 500,
            "main": "Rain",
            "description": "light rain",
            "icon": "10d"
        }
    ],
    "base": "stations",
    "main": {
        "temp": 283.63,
        "feels_like": 279.44,
        "temp_min": 283.15,
        "temp_max": 284.15,
        "pressure": 983,
        "humidity": 81
    },
    "visibility": 10000,
    "wind": {
        "speed": 5.1,
        "deg": 210
    },
    "rain": {
        "1h": 0.85
    },
    "clouds": {
        "all": 75
    },
    "dt": 1601815392,
    "sys": {
        "type": 1,
        "id": 1414,
        "country": "GB",
        "sunrise": 1601791589,
        "sunset": 1601832694
    },
    "timezone": 3600,
    "id": 2643743,
    "name": "London",
    "cod": 200
}
```
アクセス例
```
double temp = json_object_dotget_number(weatherInfo, "main.temp");
```