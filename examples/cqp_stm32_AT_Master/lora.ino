#include "SoftwareSerial.h"
#define PIN_LoRa_RX 11                      // Digital 11にLoRaモデムのTXを接続
#define PIN_LoRa_TX 12                      // Digital 12にLoRaモデムのRXを接続

SoftwareSerial ss = SoftwareSerial(PIN_LoRa_RX,PIN_LoRa_TX);

void lora_tx(const char *tx){
    ss.listen();
    ss.print(tx);
    Serial.print("> ");
    Serial.println(tx);
    delay(1000);
}

int lora_rx(char *rx,int len){
    char c;
    int i=0;
    while(ss.available()){
        c=ss.read();
        if(c == '\n') continue;
        if(c == '\r') break;
        rx[i]=c;
        if(i < len-1 ) i++;
    }
    rx[i]='\0';
    if(!strcmp(rx,"OK")) return 1;
    return 0;
}

void lora_flush(){
    char c;
    while(ss.available()){
        c=ss.read();
    }
}

int lora_init(){
    char rx[8];
    int i=1;
    ss.begin(115200);
    while(1){
        lora_tx("AT\r");
        for(i=0;i<5;i++){
            lora_rx(rx,8);
            if(!strcmp(rx,"OK")) return 1;
            delay(100);
        }
        Serial.println("ERROR:lora_init");
        delay(5000);
    }
    return 0;
}

int lora_connect(){
    char rx[8];
    
    lora_tx("ATZ\r");                   // モデムの初期化
    lora_flush();
//  lora_tx("AT+CFM=1\r");              // ACK要求をONに
//  lora_flush();
    lora_tx("AT+JOIN\r");               // ネットワークへの参加コマンド
    do{
        lora_flush();
        lora_tx("AT+NJS=?\r");          // 参加状態の確認
        lora_rx(rx,8);
        delay(5000);
    }while(strcmp(rx,"1"));             // 参加状態が1のとき
    return 1;
}

int lora_send(int num){
    char s[33];
    snprintf(s,32,"AT+SEND=1:%d\r",num);
    lora_tx(s);
    lora_flush();
    return 0;
}
