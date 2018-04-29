/*******************************************************************************
Example 07: メッセージを受信する

                                               Copyright (c) 2018 Wataru KUNINO
*******************************************************************************/

#include <lorawan_client.h>                 // LoRaWAN用ライブラリの組み込み
#define PIN_LED 13                          // Digital 13にLEDを接続
#define INTERVAL_MS 9000                    // 送信待ち時間(ms)
LoRaWANClient client;                       // LoRaWAN Client

void lora_rx(String s){
    Serial.print("Rx> ");
    Serial.println(s);
    digitalWrite(PIN_LED,HIGH);             // LEDの点灯
}

void setup(){                               // 起動時に一度だけ実行する関数
    pinMode(PIN_LED,OUTPUT);                // LEDを接続したポートを出力に
    Serial.begin(9600);                     // 動作確認のためのシリアル出力開始
    Serial.println("Example 07 RX");        // 「Example 07」をシリアル出力表示
    client.connect(true);                   // LoRaWANのフラッシュメモリの初期化
}

void loop(){                                // 繰り返し実行する関数
    digitalWrite(PIN_LED,LOW);              // LEDの消灯
    Serial.println("data request");         // データの受信要求表示
    client.sendBinary("",1,1,lora_rx);      // データ確認用の送信とデータ受信
    delay(INTERVAL_MS);                     // 次回の送信までの待ち時間
}
