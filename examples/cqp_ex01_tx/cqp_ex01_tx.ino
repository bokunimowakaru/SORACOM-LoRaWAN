/*******************************************************************************
Example 01: Arduinoマイコン内部温度センサの取得値を送信する

                                               Copyright (c) 2018 Wataru KUNINO
*******************************************************************************/

#include <lorawan_client.h>                 // LoRaWAN用ライブラリの組み込み
#define PIN_LED 13                          // Digital 13にLEDを接続
#define INTERVAL_MS 57000                   // 送信待ち時間(ms)
#define TEMP_OFFSET 0.0                     // 温度補正値（℃）
LoRaWANClient client;                       // LoRaWAN Client

void setup(){                               // 起動時に一度だけ実行する関数
    pinMode(PIN_LED,OUTPUT);                // LEDを接続したポートを出力に
    digitalWrite(PIN_LED,HIGH);             // LEDの点灯
    Serial.begin(9600);                     // 動作確認のためのシリアル出力開始
    Serial.println("Example 01 TX");        // 「Example 01」をシリアル出力表示
    client.connect(true);                   // LoRaWANのフラッシュメモリの初期化
}

void loop(){                                // 繰り返し実行する関数
    digitalWrite(PIN_LED,HIGH);             // LoRaWANモジュールのスリープ解除
    float temp=getTemp()+TEMP_OFFSET;       // 温度を取得し変数tempへ保存
    char val[6],data[12];                   // 文字配列型変数valとdataを定義
    dtostrf(temp,-1,1,val);                 // 温度値を文字配列型に変換
    snprintf(data,11,"{\"t\":%s}",val);     // JSON形式の送信データに変換
    Serial.println();                       // 改行をシリアル出力表示
    Serial.println(data);                   // 送信データをシリアル出力表示
    client.sendData(data);                  // データを送信(最大11文字まで)
    digitalWrite(PIN_LED,LOW);              // スリープ解除ピンのリセット
    delay(INTERVAL_MS);                     // 次回の送信までの待ち時間
}
