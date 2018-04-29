/*******************************************************************************
Example 08: 受信メッセージをI2C接続LCDへ表示する

                                               Copyright (c) 2018 Wataru KUNINO
*******************************************************************************/

#include <lorawan_client.h>                 // LoRaWAN用ライブラリの組み込み
#define PIN_LED 13                          // Digital 13にLEDを接続
#define INTERVAL_MS 9000                    // 送信待ち時間(ms)
LoRaWANClient client;                       // LoRaWAN Client

void lora_rx(String s){
    char lcd[17],hex[3];                    // LCD表示用に文字配列変数を定義
    int len=s.length()/2;                   // 表示する文字列長を定義
    if(len>16) len=16;                      // 文字列長は16文字以内
    for(int i=0;i<len;i++){                 // 受信データを文字列へ変換する
        s.substring(2*i,2*i+2).toCharArray(hex,3);  // 2文字ずつ変数hexへ代入
        lcd[i]=(char)strtol(hex,*(hex+2),16);       // 変数hexを文字へ変換
    }
    lcd[len]='\0';                          // 終端文字(NULL)を代入
    Serial.print("Rx> ");                   // シリアル出力表示
    Serial.println(lcd);                    // 文字配列変数lcdをシリアル出力表示
    lcdPrint(lcd);                          // LCDへ表示
    digitalWrite(PIN_LED,HIGH);             // LEDの点灯
}

void setup(){                               // 起動時に一度だけ実行する関数
    pinMode(PIN_LED,OUTPUT);                // LEDを接続したポートを出力に
    Serial.begin(9600);                     // 動作確認のためのシリアル出力開始
    Serial.println("Example 08 LCD");       // 「Example 07」をシリアル出力表示
    lcdSetup(8, 2);                         // 8桁×2行のI2C接続LCDを初期化
    lcdPrint("LoRaWAN Ex08 LCD");           // タイトルをLCDへ表示
    client.connect(true);                   // LoRaWANのフラッシュメモリの初期化
}

void loop(){                                // 繰り返し実行する関数
    digitalWrite(PIN_LED,LOW);              // LEDの消灯
    Serial.println("data request");         // データの受信要求表示
    client.sendBinary("",1,1,lora_rx);      // データ確認用の送信とデータ受信
    delay(INTERVAL_MS);                     // 次回の送信までの待ち時間
}
