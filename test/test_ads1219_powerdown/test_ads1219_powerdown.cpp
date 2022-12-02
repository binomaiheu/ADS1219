#include "unity.h"
#include "ADS1219.h"

ADS1219 adc;

void setUp(void) {
    adc.begin();
}

void tearDown(void) {
}

void test_ads1219_powerdown(void){
    // detect the device on the bus
    TEST_ASSERT(adc.detect());
    TEST_FAIL_MESSAGE("Not implemented yet");
}

void setup()
{
    delay(2000);

    UNITY_BEGIN();
    
    RUN_TEST(test_ads1219_powerdown);

    UNITY_END();
}

void loop(){}
