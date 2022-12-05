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
    
    adc.readShorted(&retcode);

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

        // test if returned value is less or equal to 2^23-1
        TEST_ASSERT_LESS_OR_EQUAL( (1 << 23)-1, value );
        TEST_ASSERT_GREATER_OR_EQUAL( -(1 << 23), value );
    }
}

void test_ads1219_readShorted_Offset_mV(void)
{
    uint8_t retcode;

    // read 10 offsets and assess the cumulative average is below 0.005 mV ?
    int32_t offset = adc.readShorted(&retcode, 10);

    float offset_mV = adc.milliVolts(offset, ADS1219_GAIN_ONE, &retcode );

    TEST_ASSERT_FLOAT_WITHIN(0.005, 0., offset_mV); // offset should be zero +/- 0.005 mV ??
}



void setup()
{
    delay(2000);

    UNITY_BEGIN();
    
    RUN_TEST(test_ads1219_detect);
    RUN_TEST(test_ads1219_readShorted);
    RUN_TEST(test_ads1219_readSingleEnded);
    RUN_TEST(test_ads1219_readShorted_Offset_mV);

    UNITY_END();
}

void loop(){}
