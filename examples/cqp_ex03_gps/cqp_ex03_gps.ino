/*******************************************************************************
Example 03: 起動したときのGPS位置と、現在のGPS位置の距離を送信する

GPSモジュール：u-blox NEO-6M NEO-6M-0-001
                                               Copyright (c) 2018 Wataru KUNINO
*******************************************************************************/

#include "TinyGPS.h"
#include <lorawan_client.h>                 // LoRaWAN用ライブラリの組み込み
#define PIN_LED 13                          // Digital 13にLEDを接続
#define PIN_SLP 7                           // スリープ解除ピン(Digital 7固定)
#define INTERVAL_MS 25000                   // 送信待ち時間(ms)
#define INTERVAL_WDT 1000                   // スリープ間隔(ms)
#define TEMP_OFFSET 0.0                     // 温度補正値（℃）
#define SLEEP_MODE 0                        // 0:ノーマルスリープ、1:ディープ
LoRaWANClient client;                       // LoRaWAN Client
volatile long wdt_counter = 0;              // 残スリープ時間(ms)
TinyGPS gps;                                // GPSライブラリのインスタンスを定義
float flat0, flon0;                         // 緯度・経度の初期値

void setup(){                               // 起動時に一度だけ実行する関数
    pinMode(PIN_LED,OUTPUT);                // LEDを接続したポートを出力に
    pinMode(PIN_SLP,OUTPUT);                // スリープ解除ピン(Digital 7固定)
    digitalWrite(PIN_LED,HIGH);             // LEDの点灯
    digitalWrite(PIN_SLP,LOW);              // スリープ解除ピンのリセット
    Serial.begin(9600);                     // 動作確認のためのシリアル出力開始
    Serial.println("Example 03 GPS");       // 「Example 03」をシリアル出力表示
    setupGps();                             // GPS初期化
    while(!getGpsPos(gps,&flat0,&flon0)){   // GPSデータの初期値入力待ち
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
    float flat, flon;                       // 緯度・経度の初期値
    if(getGpsPos(gps,&flat,&flon)){
        float d=gps.distance_between(flat,flon,flat0,flon0);
        Serial.print("distance = ");
        Serial.print(d);
        Serial.print("m ");
        if( client.connect(false) ){        // 再接続
            char val[6],data[12];           // 文字配列型変数valとdataを定義
            dtostrf(d,-1,1,val);            // 測定値を文字配列型に変換
            snprintf(data,11,"{\"d\":%s}",val); // JSON形式の送信データに変換
            Serial.println(data);           // 送信データをシリアル出力表示
            client.sendData(data);          // データを送信(最大11文字まで)
            delay(13);                      // シリアル出力の完了待ち
        }else Serial.println("failed");     // 送信の失敗
    }
    wdt_counter=INTERVAL_MS;                // 残スリープ時間のリセット
    digitalWrite(PIN_SLP,LOW);              // スリープ解除ピンのリセット
    delay(1);                               // スリープ信号出力の遷移待ち
    if(SLEEP_MODE) client.deep_sleep();     // ディープスリープの実行
    else client.sleep();                    // ノーマルスリープの実行
}
