#include <Arduino.h>
#include "ADS1219.h"


uint8_t retcode;
ADS1219 adc;

void setup() {
  delay(2000);
  
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Starting up...");
  delay(1000);

  adc.begin();
  Serial.println("Detecting adc on the bus");
  bool detected = adc.detect();

  if ( detected ) Serial.println("DETECTED !");
  else {
    Serial.println("NOT FOUND");
    while(true){
      delay(50);
    }
  }

  // set a reset
  Serial.println("cmd: reset()");
  retcode = adc.reset();
  Serial.print("Return : "); Serial.println(retcode);

  // read gain
  Serial.println("cmd: getGain()");
  uint8_t gain;
  retcode = adc.getGain(&gain);
  Serial.print("Return : "); Serial.println(retcode);
  Serial.print("Gain   : "); Serial.println(gain);

  Serial.println("cmd: setGain(ADS1219_GAIN_FOUR)");
  retcode = adc.setGain(ADS1219_GAIN_FOUR);
  Serial.print("Return : "); Serial.println(retcode);
  
  Serial.println("cmd: getGain()");
  retcode = adc.getGain(&gain);
  Serial.print("Return : "); Serial.println(retcode);
  Serial.print("Gain   : "); Serial.println(gain);

  // send a start
  //Serial.println("cmd: start()");
  //retcode = adc.start();
  //Serial.print("Return : "); Serial.println(retcode);
}

void loop() {
  // put your main code here, to run repeatedly:
}