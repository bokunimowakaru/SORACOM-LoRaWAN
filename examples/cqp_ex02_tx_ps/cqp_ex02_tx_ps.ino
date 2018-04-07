/*******************************************************************************
Example 02: Arduinoマイコン内部温度センサの取得値を送信する [ディープスリープ]

                                               Copyright (c) 2018 Wataru KUNINO
*******************************************************************************/

#include <lorawan_client.h>                 // LoRaWAN用ライブラリの組み込み
#define PIN_LED 13                          // Digital 13にLEDを接続
#define PIN_SLP 7                           // スリープ解除ピン(Digital 7固定)
#define INTERVAL_MS 51000                   // 送信待ち時間(ms)
#define INTERVAL_WDT 1000                   // スリープ間隔(ms)
#define TEMP_OFFSET 0.0                     // 温度補正値（℃）
#define SLEEP_MODE 1                        // 0:ノーマルスリープ、1:ディープ
LoRaWANClient client;                       // LoRaWAN Client
volatile long wdt_counter = 0;              // 残スリープ時間(ms)

void setup(){                               // 起動時に一度だけ実行する関数
    pinMode(PIN_LED,OUTPUT);                // LEDを接続したポートを出力に
    pinMode(PIN_SLP,OUTPUT);                // スリープ解除ピン(Digital 7固定)
    digitalWrite(PIN_LED,HIGH);             // LEDの点灯
    digitalWrite(PIN_SLP,LOW);              // スリープ解除ピンのリセット
    Serial.begin(9600);                     // 動作確認のためのシリアル出力開始
    Serial.println("Example 02 TX");        // 「Example 01」をシリアル出力表示
    client.connect(true);                   // LoRaWANのフラッシュメモリの初期化
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
    digitalWrite(PIN_SLP,HIGH);             // LoRaWANモジュールのスリープ解除
    delay(2200 * SLEEP_MODE);               // ディープスリープ復帰時間待ち
    if( client.connect(false) ){            // 再接続
        float temp=getTemp()+TEMP_OFFSET;   // 温度を取得し変数tempへ保存
        char val[6],data[12];               // 文字配列型変数valとdataを定義
        dtostrf(temp,-1,1,val);             // 温度値を文字配列型に変換
        snprintf(data,11,"{\"t\":%s}",val); // JSON形式の送信データに変換
        Serial.println();                   // 改行をシリアル出力表示
        Serial.println(data);               // 送信データをシリアル出力表示
        client.sendData(data);              // データを送信(最大11文字まで)
        delay(13);                          // シリアル出力の完了待ち
    }
    wdt_counter=INTERVAL_MS;                // 残スリープ時間のリセット
    digitalWrite(PIN_SLP,LOW);              // スリープ解除ピンのリセット
    delay(1);                               // スリープ信号出力の遷移待ち
    if(SLEEP_MODE) client.deep_sleep();     // ディープスリープの実行
    else client.sleep();                    // ノーマルスリープの実行
}
