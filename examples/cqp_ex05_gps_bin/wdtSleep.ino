/*******************************************************************************
WDT スリープ

下記のサンプルを改良しました。
国野 亘 2018/4/07
********************************************************************************
はじめての電子工作超入門
第56回 SORACOM Air×3GIMによるArduinoの3G通信
その４～Arduinoのsleepモードで省エネ実装 
2017.01.26 
赤川シホロ

http://deviceplus.jp/hobby/entry_056/
*******************************************************************************/

#include <avr/sleep.h>
#include <avr/wdt.h>
 
/*
 * ウォッチドッグ処理の参考元:2014/11/17 ラジオペンチさん http://radiopench.blog96.fc2.com/
 */
void wdtSleep_setup(unsigned int ii) { // ウォッチドッグタイマーをセット。
	// 引数はWDTCSRにセットするWDP0-WDP3の値。設定値と動作時間は概略下記
	// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
	// 6=1sec, 7=2sec, 8=4sec, 9=8sec
	byte bb;
	bb =ii & 7; 						// 下位3ビットをbbに
	if( ii ==    8) bb=0b100000;
	if( ii ==    9) bb=0b100001;
	if (ii >=   10) bb=0b000;			// 16ms
	if (ii >=   24) bb=0b001;			// 32ms
	if (ii >=   48) bb=0b010;			// 64ms
	if (ii >=   96) bb=0b011;			// 128ms
	if (ii >=  192) bb=0b100;			// 250ms
	if (ii >=  375) bb=0b101;			// 500ms
	if (ii >=  750) bb=0b110;			// 1000ms
	if (ii >= 1500) bb=0b111;			// 2000ms
	if (ii >= 3000) bb=0b100000;		// 4000ms
	if (ii >= 6000) bb=0b100001;		// 8000ms
	bb |= ( 1 << WDCE );

	MCUSR &= ~(1 << WDRF); 				// MCU Status Reg. Watchdog Reset Flag ->0
	// start timed sequence
	WDTCSR |= (1 << WDCE) | (1<<WDE); 	// ウォッチドッグ変更許可（WDCEは4サイクルで自動リセット）
	// set new watchdog timeout value
	WDTCSR = bb; 						// 制御レジスタを設定
	WDTCSR |= _BV(WDIE);
} 
 
ISR(WDT_vect) { 						// WDTがタイムアップした時に実行される処理
 // wdt_cycle++; 						// 必要ならコメントアウトを外す
}

void wdtSleep(unsigned long t) { 		// パワーダウンモードでdelayを実行
//	Serial.println("Goodnight!"); 		//動作中の表示
//	delay(100);

	wdtSleep_setup(t); 					// ウォッチドッグタイマー割り込み条件設定
	ADCSRA &= ~(1 << ADEN); 			// ADENビットをクリアしてADCを停止（120μA節約）
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // パワーダウンモード
	sleep_enable();

	sleep_mode(); 						// ここでスリープに入る

	sleep_disable();					// WDTがタイムアップでここから動作再開 
	ADCSRA |= (1 << ADEN); 				// ADCの電源をON 
} 

/*
int wakePin = 2; //割り込み用のピン番号
int ledPin = 13;
volatile int wdt_cycle = 0; 
volatile int wdt_counter = 0; 

void setup() {
	pinMode(wakePin, INPUT); 
	Serial.begin(9600); 
} 

void loop(){ 
	Serial.println("Awake!"); //動作中の表示 
	delay(50); 
	wdtSleep(9); // 8sec 
	wdt_counter++; 
	if( wdt_counter > 10 ){
		digitalWrite(ledPin,HIGH);
		delay(3000);
		digitalWrite(ledPin,LOW);
		wdt_counter=0;
	}
	Serial.print("WakeUp!:"); //動作中の表示
	Serial.println(wdt_counter);
}
*/
