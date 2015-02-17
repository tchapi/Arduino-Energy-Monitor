#define DEBUG 1

/*

thanks to http://hacking.majenko.co.uk/making-accurate-adc-readings-on-arduino
and Jérôme : http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/

*/
// to enable 12-bit ADC resolution on Arduino Due, 
// include the following line in main sketch inside setup() function:
//  analogReadResolution(ADC_BITS);
// otherwise will default to 10 bits, as in regular Arduino-based boards.
#if defined(__arm__)
  #define ADC_BITS    12
#else
  #define ADC_BITS    10
#endif

#define ADC_COUNTS  (1<<ADC_BITS)

long readVcc() {
  long result;
  
  //not used on emonTx V3 - as Vcc is always 3.3V - eliminates bandgap error and need for calibration http://harizanov.com/2013/09/thoughts-on-avr-adc-accuracy/
  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);  
  #elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB1286__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    ADCSRB &= ~_BV(MUX5);   // Without this the function always returns -1 on the ATmega2560 http://openenergymonitor.org/emon/node/2253#comment-11432
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);	
  #endif

  #if defined(__AVR__) 
    delay(2);                                        // Wait for Vref to settle
    ADCSRA |= _BV(ADSC);                             // Convert
    while (bit_is_set(ADCSRA,ADSC));
    result = ADCL;
    result |= ADCH<<8;
    result = 1126400L / result;                     //1100mV*1024 ADC steps http://openenergymonitor.org/emon/node/1186
    return result;
  #elif defined(__arm__)
    return (3300);                                  //Arduino Due
  #else 
    return (5000);                                  //Guess that other un-supported architectures will be running a 3.3V!
  #endif
}

//declare reset function at address 0
void(* resetFunc) (void) = 0;

/*
  Debug function 
*/
void debug(const __FlashStringHelper *str) {
  if(DEBUG) {
    Serial.println(str);
  }
}

/*
  On error, print PROGMEM string to serial monitor and stop
*/
void hang(const __FlashStringHelper *str) {
  debug(str);
  for(;;);
}

