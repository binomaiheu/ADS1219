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
  Serial.println("Reset ADC");
  retcode = adc.reset();
  Serial.print("Return : "); Serial.println(retcode);

  // Setting internal voltage reference (a bit redundant after reset, but ok)
  adc.setVREF(ADS1219_VREF_INTERNAL);

  // Setting GAIN one
  adc.setGain(ADS1219_GAIN_ONE);

  // Setting the conversion mode
  adc.setConversionMode(ADS1219_CM_SINGLE_SHOT);

  // Setting the data rate
  adc.setDataRate( ADS1219_DATARATE_20SPS );
}

void loop() {
  // put your main code here, to run repeatedly:
  int32_t values[5];
  float   mvolts[5];

    // read shorted
  values[0] = adc.readShorted(&retcode);
  for ( uint8_t i = 0; i<4; i++ ) values[i+1] = adc.readSingleEnded(i, &retcode );

  // convert to mV
  for (uint8_t i = 0; i<5; i++ ) mvolts[i] = adc.milliVolts(values[i], ADS1219_GAIN_ONE, &retcode );

  Serial.print(values[0]);
  for ( uint8_t i = 0; i<4; i++ ) {
    Serial.print(";");
    Serial.print(values[i+1]);
  }
  Serial.print(";");
  Serial.print(mvolts[0], 6);
  for ( uint8_t i = 0; i<4; i++ ) {
    Serial.print(";");
    Serial.print(mvolts[i+1], 6);
  }
  Serial.println();
  
  delay(1000);
}