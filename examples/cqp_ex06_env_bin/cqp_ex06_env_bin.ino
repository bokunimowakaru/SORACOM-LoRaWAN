/*******************************************************************************
Example 06: 温度・湿度・気圧センサBME280の取得値を送信する [バイナリ転送例]

                                               Copyright (c) 2018 Wataru KUNINO
********************************************************************************
I2C初期化エラー時（I2C ERROR表示時）はI2Cアドレスを変更してください
    通常時：bme280_init();
    変更時：bme280_init(0x77);
********************************************************************************
バイナリパーサー設定

SORACOMコンソールを起動し、[LoRaグループ]内の[バイナリパーサー設定]のフォーマット
欄へ下記を入力してください。

temp::float:32:little-endian
hum::uint:8:little-endian
press::float:32:little-endian

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
float temp;                                 // 温度値(℃)保持用変数
float hum;                                  // 湿度値(％)保持用変数
float press;                                // 気圧値(Pa)保持用変数

struct LoRaPayload{                         // LoRaWAN送信用の変数(11バイト以下)
  float temp;                               // 温度(float型 4バイト)
  uint8_t hum;                              // 電圧温度(uint8型 1バイト)
  float press;                              // 動作時間(float型 4バイト)
};


void setup(){                               // 起動時に一度だけ実行する関数
    pinMode(PIN_LED,OUTPUT);                // LEDを接続したポートを出力に
    pinMode(PIN_SLP,OUTPUT);                // スリープ解除ピン(Digital 7固定)
    digitalWrite(PIN_LED,HIGH);             // LEDの点灯
    digitalWrite(PIN_SLP,LOW);              // スリープ解除ピンのリセット
    Serial.begin(9600);                     // 動作確認のためのシリアル出力開始
    Serial.println("Example 06 ENV");       // 「Example 06」をシリアル出力表示
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
    if(!bme280_init()){                     // I2Cセンサの初期化成功時
        temp =bme280_getTemp();             // 温度値を取得
        hum  =bme280_getHum();              // 湿度値を取得
        press=bme280_getPress();            // 気圧値を取得
        bme280_stop();                      // 温湿度・気圧センサの動作停止
        if( client.connect(false) ){        // 再接続
            LoRaPayload payload;            // 送信用の構造体変数dataを定義
            payload.temp = temp;            // 温度を構造体変数dataへ保存
            payload.hum  =(uint8_t)hum;     // 湿度を構造体変数dataへ保存
            payload.press= press;           // 気圧を構造体変数dataへ保存
            Serial.print("{\"temp\":");     // (シリアルモニタ用)温度表示
            Serial.print(payload.temp,2);
            Serial.print(",\"hum\":");      // (シリアルモニタ用)湿度表示
            Serial.print(payload.hum);
            Serial.print(",\"press\":");    // (シリアルモニタ用)気圧表示
            Serial.print(payload.press,2);
            Serial.println("}");
            client.sendBinary((byte*)&payload,sizeof(payload)); // データ送信
            delay(45);                      // シリアル出力の完了待ち
        }
    }else Serial.println("I2C ERROR");      // I2C通信失敗
    wdt_counter=INTERVAL_MS;                // 残スリープ時間のリセット
    digitalWrite(PIN_SLP,LOW);              // スリープ解除ピンのリセット
    delay(1);                               // スリープ信号出力の遷移待ち
    if(SLEEP_MODE) client.deep_sleep();     // ディープスリープの実行
    else client.sleep();                    // ノーマルスリープの実行
}
