#include "unity.h"

#include "ADS1219.h"

ADS1219 adc;

void setUp(void) 
{
    adc.begin();
    adc.reset();
}

void tearDown(void) 
{
}

void test_ads1219_detect(void)
{
    // detect the device on the bus
    TEST_ASSERT(adc.detect());
}

void test_ads1219_reset(void)
{
    TEST_ASSERT_EQUAL(0, adc.reset());
}

void test_ads1219_read_gain(void)
{
    uint8_t gain;
    TEST_ASSERT_EQUAL(0, adc.getGain(&gain));
}

void test_ads1219_set_gain(void)
{
    uint8_t gain;

    // set gain to 1 & read back    
    TEST_ASSERT_EQUAL(0, adc.setGain(ADS1219_GAIN_ONE));    
    TEST_ASSERT_EQUAL(0, adc.getGain(&gain));
    TEST_ASSERT_EQUAL(ADS1219_GAIN_ONE, gain);
    
    // set gain to 4 and read back
    TEST_ASSERT_EQUAL(0, adc.setGain(ADS1219_GAIN_FOUR));    
    TEST_ASSERT_EQUAL(0, adc.getGain(&gain));
    TEST_ASSERT_EQUAL(ADS1219_GAIN_FOUR, gain);
}

void test_ads1219_read_vref(void)
{
    uint8_t type;
    TEST_ASSERT_EQUAL(0, adc.getVREF(&type));
}

void test_ads1219_set_vref(void)
{
    uint8_t type;

    TEST_ASSERT_EQUAL(0, adc.setVREF(ADS1219_VREF_EXTERNAL));
    TEST_ASSERT_EQUAL(0, adc.getVREF(&type));
    TEST_ASSERT_EQUAL(ADS1219_VREF_EXTERNAL, type);

    TEST_ASSERT_EQUAL(0, adc.setVREF(ADS1219_VREF_INTERNAL));
    TEST_ASSERT_EQUAL(0, adc.getVREF(&type));
    TEST_ASSERT_EQUAL(ADS1219_VREF_INTERNAL, type);
}



void setup()
{
    delay(2000);

    UNITY_BEGIN();
    
    RUN_TEST(test_ads1219_detect);
    RUN_TEST(test_ads1219_reset);
    RUN_TEST(test_ads1219_read_gain);
    RUN_TEST(test_ads1219_set_gain);
    RUN_TEST(test_ads1219_read_vref);
    RUN_TEST(test_ads1219_set_vref);


    // Don't forget to make tests for the whole configuration structure and peform different operations in succession
    // to see whether orring of bits is ok


    UNITY_END();
}

void loop(){}
