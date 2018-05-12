/*******************************************************************************
Arduinoマイコン内部温度センサの取得値を送信する
                                               Copyright (c) 2018 Wataru KUNINO
********************************************************************************
Digital 11にLoRaモデムのTXを接続
Digital 12にLoRaモデムのRXを接続
*******************************************************************************/

#define PIN_LED 13                          // Digital 13にLEDを接続
#define INTERVAL_MS 30000                   // 送信待ち時間(ms)
#define TEMP_OFFSET 0.0                     // 温度補正値（℃）

void setup(){                               // 起動時に一度だけ実行する関数
    pinMode(PIN_LED,OUTPUT);                // LEDを接続したポートを出力に
    digitalWrite(PIN_LED,HIGH);             // LEDの点灯
    Serial.begin(9600);                     // 動作確認のためのシリアル出力開始
    Serial.println("AT_Master");            // 「AT_Master」をシリアル出力表示
    lora_init();
    lora_connect();                         // LoRaWANへ接続する
}

void loop(){                                // 繰り返し実行する関数
    digitalWrite(PIN_LED,HIGH);             // LoRaWANモジュールのスリープ解除
    int temp=(int)getTemp()+TEMP_OFFSET;    // 温度を取得し変数tempへ保存
    Serial.print("Temp=");
    Serial.println(temp);                   // 送信データをシリアル出力表示
    lora_send(temp);                        // tempの値をLoRa送信
    digitalWrite(PIN_LED,LOW);              // LEDの消灯
    delay(INTERVAL_MS);                     // 次回の送信までの待ち時間
}
