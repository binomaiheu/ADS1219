# Implementation I2C communication library for the Texas Instruments ADS1219 ADC

Here I started an own library for the ADS1219 24-bits adc inspired by existing code, partly
to learn, partly since the existing repo's didn't seem complete. Will refactor to platformio library. 

Work in progress...

## Features
- In the library i found, (https://github.com/OM222O/ADS1219/), the DRDY pin was used to wait for the result of the conversion, here I'm using the status register as well
- I added a timeout while waiting for the conversion result such that if something goes wrong there in the code, the microcontroller doesn't hang while waiting forever for the conversion result to become available


## Inspired by

- Adafruit I2C device
- https://github.com/OM222O/ADS1219/
- https://github.com/miketeachman/micropython-ads1219
- https://github.com/DocSunset/ADS1219
- https://github.com/adafruit/Adafruit_BusIO/blob/master/Adafruit_I2CDevice.h
