/*******************************************************************************
Example 04: Arduinoマイコン内部温度センサの取得値を送信する [バイナリ転送例]

マイコン内蔵温度センサの値と、電源電圧(マイコン部)、CPU動作時間を送信します。

                                               Copyright (c) 2018 Wataru KUNINO
********************************************************************************
バイナリパーサー設定

SORACOMコンソールを起動し、[LoRaグループ]内の[バイナリパーサー設定]のフォーマット
欄へ下記を入力してください。

temp::float:32:little-endian
vcc::uint:16:little-endian:/1000
time::uint:32:little-endian:/1000

※改行は[スペース]に変更してください(コピーペーストすると自動的に変換されます)
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
uint32_t time_prev;                         // 前回の時刻

struct LoRaPayload{                         // LoRaWAN送信用の変数(11バイト以下)
  float temp;                               // 温度(float型 4バイト)
  uint16_t vcc;                             // 電圧温度(uint16型 2バイト)
  uint32_t time;                            // 動作時間(uint32型 4バイト)
} payload;


void setup(){                               // 起動時に一度だけ実行する関数
    pinMode(PIN_LED,OUTPUT);                // LEDを接続したポートを出力に
    pinMode(PIN_SLP,OUTPUT);                // スリープ解除ピン(Digital 7固定)
    digitalWrite(PIN_LED,HIGH);             // LEDの点灯
    digitalWrite(PIN_SLP,LOW);              // スリープ解除ピンのリセット
    Serial.begin(9600);                     // 動作確認のためのシリアル出力開始
    Serial.println("Example 04 BIN");       // 「Example 04」をシリアル出力表示
    time_prev=millis();                     // 起動したときの時刻を記録
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
    Serial.println();                       // 改行をシリアル出力表示
    digitalWrite(PIN_SLP,HIGH);             // LoRaWANモジュールのスリープ解除
    delay(2200 * SLEEP_MODE);               // ディープスリープ復帰時間待ち
    payload.temp = getTemp()+TEMP_OFFSET;   // 温度を取得
    payload.vcc = (uint16_t)(getVcc()*1000);// 電圧を取得し、mVへ変換
    payload.time = millis()-time_prev;      // 動作時間を取得し、前回値を減算
    time_prev += payload.time;              // 今回のCPU時間を保存
    if( client.connect(false) ){            // 再接続
        Serial.print("{\"temp\":");         // (シリアルモニタ用)温度表示
        Serial.print(payload.temp,3);
        Serial.print(",\"vcc\":");          // (シリアルモニタ用)電圧表示
        Serial.print((float)payload.vcc/1000,3);
        Serial.print(",\"time\":");         // (シリアルモニタ用)時間表示
        Serial.print((float)payload.time/1000,3);
        Serial.println("}");
        client.sendBinary((byte *)&payload,sizeof(payload));    // データ送信
        delay(82);                          // シリアル出力の完了待ち
    }
    wdt_counter=INTERVAL_MS;                // 残スリープ時間のリセット
    digitalWrite(PIN_SLP,LOW);              // スリープ解除ピンのリセット
    delay(1);                               // スリープ信号出力の遷移待ち
    if(SLEEP_MODE) client.deep_sleep();     // ディープスリープの実行
    else client.sleep();                    // ノーマルスリープの実行
}
