#include "unity.h"
#include "ADS1219.h"

ADS1219 adc;

void setUp(void) {
    adc.begin();
}

void tearDown(void) {
}

void test_ads1219_powerdown(void){
    
    uint8_t gain, vref, rate;

    // detect the device on the bus
    TEST_ASSERT(adc.detect());

    TEST_ASSERT_EQUAL_MESSAGE(0, adc.reset(), "error resetting");

    // set some funky config values
    TEST_ASSERT_EQUAL_MESSAGE(0, adc.setGain(ADS1219_GAIN_FOUR), "error setting gain");
    TEST_ASSERT_EQUAL_MESSAGE(0, adc.setVREF(ADS1219_VREF_EXTERNAL), "error setting vref");
    TEST_ASSERT_EQUAL_MESSAGE(0, adc.setDataRate(ADS1219_DATARATE_90SPS), "error setting datarate");

    TEST_ASSERT_EQUAL_MESSAGE(0, adc.powerDown(), "error sending power down");

    // sleep
    delay(2000);

    // issue start to wake up
    TEST_ASSERT_EQUAL_MESSAGE(0, adc.start(), "error starting after power up");

    // read back configuration
    TEST_ASSERT_EQUAL_MESSAGE(0, adc.getGain(&gain), "error reading gain after power up" );
    TEST_ASSERT_EQUAL_MESSAGE(0, adc.getVREF(&vref), "error reading vref after power up" );
    TEST_ASSERT_EQUAL_MESSAGE(0, adc.getDataRate(&rate), "error reading datarate after power up" );

    // check equal
    TEST_ASSERT_EQUAL_MESSAGE( ADS1219_GAIN_FOUR, gain, "gain mismatch after power up" );
    TEST_ASSERT_EQUAL_MESSAGE( ADS1219_VREF_EXTERNAL, vref, "vref type mismatch after power up" );
    TEST_ASSERT_EQUAL_MESSAGE( ADS1219_DATARATE_90SPS, rate, "datarate mismatch after power up");
}

void setup()
{
    delay(2000);

    UNITY_BEGIN();
    
    RUN_TEST(test_ads1219_powerdown);

    UNITY_END();
}

void loop(){}
