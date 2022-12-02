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

  Serial.println("cmd: getGain()");
  retcode = adc.getGain(&gain);
  Serial.print("Return : "); Serial.println(retcode);
  Serial.print("Gain   : "); Serial.println(gain);


  // read vref
  Serial.println("cmd: getVREF()");
  uint8_t type;
  retcode = adc.getVREF(&type);
  Serial.print("Return : "); Serial.println(retcode);
  Serial.print("VREF   : "); 
  type == ADS1219_VREF_INTERNAL ? Serial.println("internal") : Serial.println("external");

  Serial.println("cmd: setVREF(EXTERNAL)");
  retcode = adc.setVREF(ADS1219_VREF_EXTERNAL);
  Serial.print("Return : "); Serial.println(retcode);
  
  Serial.println("cmd: getVREF()");
  retcode = adc.getVREF(&type);
  Serial.print("Return : "); Serial.println(retcode);
  Serial.print("VREF   : "); 
  type == ADS1219_VREF_INTERNAL ? Serial.println("internal") : Serial.println("external");

  Serial.println("cmd: getGain()");
  retcode = adc.getGain(&gain);
  Serial.print("Return : "); Serial.println(retcode);
  Serial.print("Gain   : "); Serial.println(gain);


  Serial.println("cmd:: setDataRate");
  uint8_t rate;
  adc.setDataRate( ADS1219_DATARATE_330SPS );
  adc.getDataRate(&rate);
  Serial.print( "Rate (330 SPS) : "); Serial.println(rate);

  adc.setDataRate( ADS1219_DATARATE_1000SPS );
  adc.getDataRate(&rate);
  Serial.print( "Rate (1000 SPS): "); Serial.println(rate);

  adc.setDataRate( ADS1219_DATARATE_90SPS );
  adc.getDataRate(&rate);
  Serial.print( "Rate (90 SPS): "); Serial.println(rate);



  Serial.println("cmd: setConversionMode()");
  adc.setConversionMode(ADS1219_CM_CONTINUOUS);
  uint8_t mode;
  adc.getConversionMode(&mode);
  Serial.print("CM (cont) : "); Serial.println(mode);


  Serial.println("cmd: getGain()");
  retcode = adc.getGain(&gain);
  Serial.print("Return : "); Serial.println(retcode);
  
  adc.getDataRate(&rate);
  Serial.print( "Rate (90 SPS): "); Serial.println(rate);



  // send a start

  Serial.print("Conversion done ? "); Serial.println(adc.conversionReady(&retcode));

  Serial.println("cmd: start()");
  retcode = adc.start();
  Serial.print("Return : "); Serial.println(retcode);
  unsigned long tstart = millis();
  while( ! adc.conversionReady(&retcode)) {
    delay(2);
  }
  unsigned long delta = millis() - tstart;
  Serial.print("Conversion done in "); Serial.print(delta); Serial.println(" millis.");



  // read shorted
  int32_t value = adc.readShorted(&retcode);
  Serial.print("Shorted read: "); Serial.println( value );

  value = adc.readSingleEnded(0, &retcode);
  Serial.print("Ch0 read: "); Serial.println( value );

}

void loop() {
  // put your main code here, to run repeatedly:
}