#include "unity.h"
#include "ADS1219.h"

ADS1219 adc;

void setUp(void) {
    adc.begin();
    adc.reset();
}

void tearDown(void) {
}

void test_ads1219_detect(void){
    // detect the device on the bus
    TEST_ASSERT(adc.detect());
}


void test_ads1219_readShorted(void)
{
    uint8_t retcode;
    int32_t value = adc.readShorted(&retcode);

    TEST_ASSERT_EQUAL(0, retcode);

    // TODO: add threshold for shorted ... if bias too large, faulty chip ??? what is a good value  ?
}

void test_ads1219_readSingleEnded(void)
{
    uint8_t retcode;
    int32_t value;

    // reset to default --> single shot mode
    TEST_ASSERT_EQUAL(0, adc.reset());

    for (uint8_t i = 0; i<4; i++)
    {
        value = adc.readSingleEnded(i, &retcode);
        TEST_ASSERT_EQUAL(0, retcode);
    }
}

void setup()
{
    delay(2000);

    UNITY_BEGIN();
    
    RUN_TEST(test_ads1219_detect);
    RUN_TEST(test_ads1219_readShorted);
    RUN_TEST(test_ads1219_readSingleEnded);

    UNITY_END();
}

void loop(){}
