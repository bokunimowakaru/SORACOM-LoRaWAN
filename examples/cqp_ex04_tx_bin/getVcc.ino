/*******************************************************************************
VCC電圧取得 for Arduino

本ソースリストおよびソフトウェアは、ライセンスフリーです。
個人での利用は自由に行えます。著作権表示の改変は禁止します。

                                        Copyright (c) 2012-2018 Wataru KUNINO
                                        http://www.geocities.jp/bokunimowakaru/
********************************************************************************
下記を参考にしました

http://www.instructables.com/id/Secret-Arduino-Voltmeter/
*******************************************************************************/

float getVcc(void){
	byte b_ADMUX = ADMUX;
	byte b_ADCSRA = ADCSRA;
	
	#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
		ADMUX = _BV(MUX5) | _BV(MUX0) ;
		ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	#elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined(__AVR_ATmega8__)
	    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	#else
		return( 0 );
	#endif
	
//	ADCSRA |= _BV(ADEN);  // enable the ADC
//	delay(20);            // wait for voltages to become stable.
	delay(2);             // Wait for Vref to settle
	ADCSRA |= _BV(ADSC);  // Start the ADC
	while (bit_is_set(ADCSRA,ADSC));	  // Detect end-of-conversion
	unsigned int wADC = ADCW;
	ADCSRA = b_ADCSRA;
	ADMUX = b_ADMUX;
	if( wADC ) return( 1125.3 / (float)wADC );
	else return(0.);
}

/*
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
     ADMUX = _BV(MUX5) | _BV(MUX0) ;
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}
*/
