/*******************************************************************************
Example 05: 現在のGPS位置情報を送信する

GPSモジュール：u-blox NEO-6M NEO-6M-0-001
                                               Copyright (c) 2018 Wataru KUNINO
********************************************************************************
ご注意：本ソフトを実行すると現在の位置情報がLoRaWANへ送信されます。
********************************************************************************
バイナリパーサー設定

SORACOMコンソールを起動し、[LoRaグループ]内の[バイナリパーサー設定]のフォーマット
欄へ下記を入力してください。

head::uint:8
lat::int:32:little-endian:/1000000
lon::int:32:little-endian:/1000000
alt::int:16:little-endian

※改行は[スペース]に変更してください(コピーペーストすると自動的に変換されます)
*******************************************************************************/

#include "TinyGPS.h"
#include <lorawan_client.h>                 // LoRaWAN用ライブラリの組み込み
#define PIN_LED 13                          // Digital 13にLEDを接続
#define PIN_SLP 7                           // スリープ解除ピン(Digital 7固定)
#define INTERVAL_MS 23000                   // 送信待ち時間(ms)
#define INTERVAL_WDT 1000                   // スリープ間隔(ms)
#define TEMP_OFFSET 0.0                     // 温度補正値（℃）
#define SLEEP_MODE 0                        // 0:ノーマルスリープ、1:ディープ
LoRaWANClient client;                       // LoRaWAN Client
volatile long wdt_counter = 0;              // 残スリープ時間(ms)
TinyGPS gps;                                // GPSライブラリのインスタンスを定義
float lat, lon, alt;                        // 緯度・経度・標高

struct LoRaPayload{                         // LoRaWAN送信用の変数(11バイト以下)
  uint8_t gpsHeader=0x21;                   // GPS_HEADER(uint8型 1バイト)
  int32_t lat;                              // 緯度(int32型 4バイト) 1,000,000倍
  int32_t lon;                              // 経度(int32型 4バイト) 1,000,000倍
  int16_t alt;                              // 標高(int16型 2バイト) 単位＝ｍ
};


void setup(){                               // 起動時に一度だけ実行する関数
    pinMode(PIN_LED,OUTPUT);                // LEDを接続したポートを出力に
    pinMode(PIN_SLP,OUTPUT);                // スリープ解除ピン(Digital 7固定)
    digitalWrite(PIN_LED,HIGH);             // LEDの点灯
    digitalWrite(PIN_SLP,LOW);              // スリープ解除ピンのリセット
    Serial.begin(9600);                     // 動作確認のためのシリアル出力開始
    Serial.println("Example 05 GPS");       // 「Example 05」をシリアル出力表示
    setupGps();                             // GPS初期化
    while(!getGpsPos(gps,&lat,&lon,&alt)){  // GPSデータの初期値入力待ち
        digitalWrite(PIN_LED,!digitalRead(PIN_LED));
    }
    client.connect(true);                   // LoRaWANのフラッシュメモリの初期化
    digitalWrite(PIN_LED,HIGH);
}

void loop(){                                // 繰り返し実行する関数
    if(wdt_counter > 0){                    // 残スリープ時間があるとき
        Serial.print(".");                  // 「.」をシリアル出力表示
        delay(3);                           // シリアル出力の完了待ち
        wdt_counter-=INTERVAL_WDT;          // 残スリープ時間を減算
        digitalWrite(PIN_LED,LOW);          // LEDの消灯
        wdtSleep(INTERVAL_WDT);             // スリープの実行
        digitalWrite(PIN_LED,HIGH);         // LEDの点灯
        return;                             // 繰り返し処理に戻る
    }
    Serial.println();                       // 改行をシリアル出力表示
    digitalWrite(PIN_SLP,HIGH);             // LoRaWANモジュールのスリープ解除
    delay(2200 * SLEEP_MODE);               // ディープスリープ復帰時間待ち
    if(getGpsPos(gps,&lat,&lon,&alt)){      // GPSから位置情報を取得
        if( client.connect(false) ){        // 再接続
            LoRaPayload data;               // 送信用の構造体変数dataを定義
            data.lat=(int32_t)(lat*1.e6);   // 緯度を構造体変数dataへ保存
            data.lon=(int32_t)(lon*1.e6);   // 経度を構造体変数dataへ保存
            data.alt=(int16_t)(alt);        // 標高を構造体変数dataへ保存
            Serial.print("lat = ");         // 緯度をシリアル出力表示
            Serial.print(data.lat);         // ±180 000 000
            Serial.print(", lon = ");       // 経度をシリアル出力表示
            Serial.print(data.lon);         // ±180 000 000
            Serial.print(", alt = ");       // 標高をシリアル出力表示
            Serial.println(data.alt);       // 単位＝ m メートル
            client.sendBinary((byte *)&data,sizeof(data));  // データ送信
            delay(100);                     // シリアル出力の完了待ち
        }else Serial.println("failed");     // 送信の失敗
    }
    wdt_counter=INTERVAL_MS;                // 残スリープ時間のリセット
    digitalWrite(PIN_SLP,LOW);              // スリープ解除ピンのリセット
    delay(1);                               // スリープ信号出力の遷移待ち
    if(SLEEP_MODE) client.deep_sleep();     // ディープスリープの実行
    else client.sleep();                    // ノーマルスリープの実行
}
