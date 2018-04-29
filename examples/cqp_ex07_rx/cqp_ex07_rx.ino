/*******************************************************************************
Example 07: データを受信する

                                               Copyright (c) 2018 Wataru KUNINO
********************************************************************************
メッセージ送信方法（16進数・1～11バイトを送信すると、LEDが点灯する）

SORACOMコンソールを起動し、[LoRaデバイス管理]から対象デバイスを選択し、[操作]
メニューから[ダウンリンク通信]を選び、以下を入力してから[送信]ボタンを押します。
	データ	414243
	fPort	2
*******************************************************************************/

#include <lorawan_client.h>                 // LoRaWAN用ライブラリの組み込み
#define PIN_LED 13                          // Digital 13にLEDを接続
#define INTERVAL_MS 27000                   // 送信待ち時間(ms)
LoRaWANClient client;                       // LoRaWAN Client

void lora_rx(String s){                     // sendBinaryで設定したコールバック
    Serial.print("Rx> ");                   // シリアル出力表示
    Serial.println(s);                      // 受信データ(16進数)を表示
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
    client.sendBinary("",1,2,lora_rx);      // データ確認用の送信とデータ受信
    delay(INTERVAL_MS);                     // 次回の送信までの待ち時間
}
