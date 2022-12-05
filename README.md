# Texas Instruments ADS1219 ADC I2C Library

Here I started an own library for the ADS1219 24-bits adc inspired by existing code, partly
to learn, partly since the existing repo's didn't seem complete. 

Work in progres...

## Features
- In the library i found, (https://github.com/OM222O/ADS1219/), the DRDY pin was used to wait for the result of the conversion, here I'm using the chip's status register. 
- I added a timeout while waiting for the conversion result such that if something goes wrong there in the code, the microcontroller doesn't hang while waiting forever for the conversion result to become available. However for noise reasons, we first wait during the converstion time (1000 / data rate SPS), then an additional small amaount, 5 ms for 
lowest rate, 1 additional ms for the others and then check the DRDY register. Normally after the first iteration, the while loop should exit, but the timeout allows to try a few times more (albeit with increased noise) untill the timeout expires. 

## Architectures

At the moment my main interest if for the SAMD21 (`atmelsam`) and atmel 1284-P (`atmelavr`) µcontrollers, feel free to get this working on other platforms. Probably better idea to have this depend on a generic I2C communication library, but wanted to minimze dependencies there.   

## Unit tests

The library includes a number of unit tests to test the functionality of a connected ADS1219 device. These tests are included in the `test` folder. You can run them via `pio test` or pushing the test button. Several
test folder exist : 

- `test_ads1219_config` : performs various read/write tests on the configuration registry in the chip
- `test_ads1219_readout`: performs various tests with the readout, single shot and continuous mode
- `test_ads1219_powerdown`: tests the powerdown behaviour 

See https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md for list of assertions


## Inspired by

- Adafruit I2C device
- https://github.com/OM222O/ADS1219/
- https://github.com/miketeachman/micropython-ads1219
- https://github.com/DocSunset/ADS1219
- https://github.com/adafruit/Adafruit_BusIO/blob/master/Adafruit_I2CDevice.h
