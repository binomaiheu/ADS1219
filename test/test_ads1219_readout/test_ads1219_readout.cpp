#include "unity.h"
#include "ADS1219.h"


#define TEST_ADS1219_VARIANCE_NUM 100


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

void test_ads1219_readShorted_offset_mV(void)
{
    uint8_t retcode;

    // read 10 offsets and assess the cumulative average is below 0.005 mV ?
    int32_t offset = adc.readShorted(&retcode, 10);

    float offset_mV = adc.milliVolts(offset, ADS1219_GAIN_ONE, &retcode );

    TEST_ASSERT_FLOAT_WITHIN(0.005, 0., offset_mV); // offset should be zero +/- 0.005 mV ??
}


void test_ads1219_readShorted_stddev_mV(void)
{
    // Apply Welford's method to compute the sample variance for reading each channel
    // Test whether resulting std is smaller than 0.001 mV
    uint8_t retcode;
    int32_t value;    
    float fvalue, var_mV, mean_mV, tmp;

    TEST_ASSERT_EQUAL(0, adc.reset());

    var_mV = 0.;
    mean_mV = 0.;
    
    for (uint8_t j=0; j<TEST_ADS1219_VARIANCE_NUM; j++)
    {
        value = adc.readShorted(&retcode);
        TEST_ASSERT_EQUAL(0, retcode);

        fvalue = adc.milliVolts(value, ADS1219_GAIN_ONE, &retcode );
        TEST_ASSERT_EQUAL(0, retcode);

        tmp = mean_mV;
        mean_mV += ( fvalue - mean_mV)/(j+1);
        var_mV += ( fvalue - mean_mV)*(fvalue - tmp);
        Serial.println(var_mV);
    }

    // compute sample stddev
    var_mV = sqrt(var_mV/(TEST_ADS1219_VARIANCE_NUM-1));

    TEST_ASSERT_FLOAT_WITHIN( 0.001, 0., var_mV);
}


void test_ads1219_readSingleEnded_stddev_mV(uint8_t chan)
{
    // Apply Welford's method to compute the sample variance for reading each channel
    // Test whether resulting std is smaller than 0.001 mV
    uint8_t retcode;
    int32_t value;    
    float fvalue, var_mV, mean_mV, tmp;

    TEST_ASSERT_EQUAL(0, adc.reset());

    var_mV = 0.;
    mean_mV = 0.;
    
    for (uint8_t j=0; j<TEST_ADS1219_VARIANCE_NUM; j++)
    {
        value = adc.readSingleEnded(chan, &retcode);
        TEST_ASSERT_EQUAL(0, retcode);

        fvalue = adc.milliVolts(value, ADS1219_GAIN_ONE, &retcode );
        TEST_ASSERT_EQUAL(0, retcode);

        tmp = mean_mV;
        mean_mV += ( fvalue - mean_mV)/(j+1);
        var_mV += ( fvalue - mean_mV)*(fvalue - tmp);
        Serial.println(var_mV);
    }

    // compute sample stddev
    var_mV = sqrt(var_mV/(TEST_ADS1219_VARIANCE_NUM-1));

    TEST_ASSERT_FLOAT_WITHIN( 0.001, 0., var_mV);
}

void test_ads1219_readSingleEnded_stddev_mV_ch0(void)
{
    return test_ads1219_readSingleEnded_stddev_mV(0);
}

void test_ads1219_readSingleEnded_stddev_mV_ch1(void)
{
    return test_ads1219_readSingleEnded_stddev_mV(1);
}

void test_ads1219_readSingleEnded_stddev_mV_ch2(void)
{
    return test_ads1219_readSingleEnded_stddev_mV(2);
}

void test_ads1219_readSingleEnded_stddev_mV_ch3(void)
{
    return test_ads1219_readSingleEnded_stddev_mV(3);
}


void setup()
{
    delay(2000);

    UNITY_BEGIN();
    
    RUN_TEST(test_ads1219_detect);
    RUN_TEST(test_ads1219_readShorted);
    RUN_TEST(test_ads1219_readSingleEnded);
    RUN_TEST(test_ads1219_readShorted_offset_mV);
    RUN_TEST(test_ads1219_readShorted_stddev_mV);
    RUN_TEST(test_ads1219_readSingleEnded_stddev_mV_ch0);
    RUN_TEST(test_ads1219_readSingleEnded_stddev_mV_ch1);
    RUN_TEST(test_ads1219_readSingleEnded_stddev_mV_ch2);
    RUN_TEST(test_ads1219_readSingleEnded_stddev_mV_ch3);

    UNITY_END();
}

void loop(){}
