#pragma once

#include <Arduino.h>
#include <Wire.h>

// Default I2C address, A0 and A1 both to DGND
#define ADS1219_I2C_ADDRESS      0x40

// See table 7, p. 25 in spec
#define ADS1219_CMD_RESET        0x06  // Reset the device
#define ADS1219_CMD_START_SYNC   0x08  // Start or restart conversions
#define ADS1219_CMD_POWERDOWN    0x02  // Enter power-down mode
#define ADS1219_CMD_RDATA        0x10  // Read data by command
#define ADS1219_CMD_RREG_CONFIG  0x20  // Read register at address 0 (config)
#define ADS1219_CMD_RREG_STATUS  0x24  // Read register at address 1 (status)
#define ADS1219_CMD_WREG         0x40  // Write configuration register

// Mask values, 1 everywhere, except for the 
// relevant bits
#define ADS1219_CONFIG_MASK_VREF 0xFE  // bit 0
#define ADS1219_CONFIG_MASK_CM   0xFD  // bit 1, conversion mode mask
#define ADS1219_CONFIG_MASK_DR   0xF3  // bits 2 & 3, data rate
#define ADS1219_CONFIG_MASK_GAIN 0xEF  // bit 4,
#define ADS1219_CONFIG_MASK_MUX  0x1F  // bits 5, 6 & 7 : Input multiplexer configuration

#define ADS1219_MUX_DIFF_0_1     0x00  // read differential between AINp = AIN0 and AINn = AIN1 (default)
#define ADS1219_MUX_DIFF_2_3     0x20  // read differential between AINp = AIN2 and AINn = AIN3
#define ADS1219_MUX_DIFF_1_2     0x40  // read differential between AINp = AIN1 and AINn = AIN2
#define ADS1219_MUX_SINGLE_0     0x60  // read single between AINp = AIN0 and AINn = AGND
#define ADS1219_MUX_SINGLE_1     0x80  // read single between AINp = AIN1 and AINn = AGND 
#define ADS1219_MUX_SINGLE_2     0xA0  // read single between AINp = AIN2 and AINn = AGND
#define ADS1219_MUX_SINGLE_3     0xC0  // read single between AINp = AIN3 and AINn = AGND
#define ADS1219_MUX_SHORTED      0xE0  // read AINp and AINn both shorted to AVDD/2


// Config and status register
#define ADS1219_GAIN_ONE         1     // gain 1 (default)
#define ADS1219_GAIN_FOUR        4     // gain 4
#define ADS1219_VREF_INTERNAL    0     // internal 2.048 V reference
#define ADS1219_VREF_EXTERNAL    1     // external reference using REFP and REFN inputs

#define ADS1219_DATARATE_20SPS   0     // (default)
#define ADS1219_DATARATE_90SPS   1
#define ADS1219_DATARATE_330SPS  2
#define ADS1219_DATARATE_1000SPS 3

#define ADS1219_CM_SINGLE_SHOT   0     // single shot conversion mode
#define ADS1219_CM_CONTINUOUS    1     // continuous conversion mode

// Some error codes
#define ADS1219_OK                 0     // all is well
#define ADS1219_BUFFER_TOO_LARGE   1     // the buffers passed to the _write is too large for the architecture internal Wire buffer
#define ADS1219_FAILED_TO_WRITE    2     // error during writing, wrong number of bytes returned
#define ADS1219_FAILED_TO_END      3     // error during endTransmission
#define ADS1219_FAILED_TO_RECEIVE  4     // incorrect number of bytes recieved from ADS
#define ADS1219_INVALID_GAIN       5     // invalid gain
#define ADS1219_INVALID_VREF       6     // invalid vref
#define ADS1219_INVALID_DATARATE   7     // invalid datarate
#define ADS1219_INVALID_CM         8     // invalid conversion mode
#define ADS1219_TIMEOUT            9     // timeout during conversion
#define ADS1219_INVALID_MUX       10     // invalid mux config pattern given

/**
 * @brief Class to communicate with and ADS1219 chip via I2C 
 *
 * Most routines return their status code as the return value and return values through pointers in the arguments, 
 * except for the routines which read the conversion results.  
 */
class ADS1219 {
public:
    
    /**
     * @brief Constructor
     * 
     * @param i2c_addr the I2C address, default for this module is 0x40, (A0 and A1  to DGND), see specs for wiring 
     * @param drdy_pin GPIO pin to which the DRDY signal is connected, per default it's not used and the conversion 
     *        readiness is read from the status register
     * @param wire address of the TwoWire bus object
     */
    ADS1219(uint8_t i2c_addr = ADS1219_I2C_ADDRESS, uint8_t drdy_pin = 0, TwoWire *wire = &Wire);

    virtual ~ADS1219();

    /**
     * @brief Startup the device
     *
     * This does nothing more than starting the wire bus and setting the begun flag 
     */
    void begin( void );


    /**
     * @brief Detect presence of the device
     * 
     * Simple scan, may give false positive if no pull-ups on I2C (see Adafruit_I2CDevice.cpp)
     */
    bool detect( void );


    /**
     * @brief How many bytes we can read in a transaction
     * 
     * Depends, see Wire.h for specific architecture. RingBuffer in SAMD seems to have 256, others 32
     * Note that in I2CDevice from Adafruit, there is 250 for SAMD ??? 
     */
    size_t maxBufferSize() { return _maxBufferSize; }


    /**
     * @brief Issue a reset command
     * 
     * This command resets the device to the default states. No delay time is required after the RESET command is
     * latched before starting to communicate with the device as long as the timing requirements (see the I2C Timing
     * Requirements table) for the (repeated) START and STOP conditions are met.
     * 
     * @return error code
     */
    uint8_t reset(void);


    /**
     * @brief Start command
     * 
     *  @return error code
     */
    uint8_t start(void);


    /**
     * @brief Power down command
     * 
     * The POWERDOWN command places the device into power-down mode. This command shuts down all internal
     * analog components, but holds all register values. In case the POWERDOWN command is issued when a
     * conversion is ongoing, the conversion completes before the ADS1219 enters power-down mode. As soon as a
     * START/SYNC command is issued, all analog components return to their previous states.
     * 
     * @return error code
     */
    uint8_t powerDown(void);


    /**
     * @brief return the device gain
     * 
     * @param gain variable pointer to recieve the returned gain value
     * 
     * @return error code
     */
    uint8_t getGain( uint8_t* gain );


    /**
     * @brief set the device gain 
     * 
     * @param gain gain value, either ADS1219_GAIN_ONE or ADS1219_GAIN_FOUR
     * 
     * @return error code
     */
    uint8_t setGain( uint8_t gain = ADS1219_GAIN_ONE );


    /**
     * @brief getVREF
     * 
     * Returns the voltage type
     * 
     * @param type variable pointer to recieve the voltage reference type, either internal (ADS1219_VREF_INTERNAL) or external (ADS1219_VREF_EXTERNAL)
     * 
     * @return error code
     */
    uint8_t getVREF( uint8_t *type );


    /**
     * @brief setVREF
     * 
     * Set the voltage type : reference, internal or external
     * 
     * @param type the voltage reference setting : internal (ADS1219_VREF_INTERNAL) or external (ADS1219_VREF_EXTERNAL)
     * @param arefn in case of external reference, set the negative reference mV (default 0 mV for internal)
     * @param arefp in case of external reference, set the positive reference mV (default 2048 mV for internal)
     * 
     * @return error code
     */
    uint8_t setVREF( uint8_t type, float aref_n = 0.f, float aref_p = 2048.f );


    /**
     * @brief getDataRate() 
     * 
     * Returns the data rate
     *  - ADS1219_DATARATE_20SPS 0 (default)
     *  - ADS1219_DATARATE_90SPS 1
     *  - ADS1219_DATARATE_330SPS 2
     *  - ADS1219_DATARATE_1000SPS 3
     *
     * @param rate variable pointer to hold the returned rate value
     * 
     * @return error code
     */
    uint8_t getDataRate( uint8_t* rate);


    /**
     * @brief setDataRate()
     * 
     * Sets the datarate
     *
     * @return error code
     */
    uint8_t setDataRate( uint8_t rate );


    /**
     * @brief set the conversion mode, continuous or single shot (default)
     * 
     * @param mode The requested mode : ADS1219_CM_SINGLE_SHOT(=0), or ADS1219_CM_CONTINUOUS(=1)
     * 
     * @return error code
     */
    uint8_t setConversionMode( uint8_t mode );

    /**
     * @brief gets the conversion mode
     * 
     * @param mode pointer to the variable to return the mode to
     * 
     * @return error code
     */
    uint8_t getConversionMode( uint8_t* mode );


    /**
     * @brief returns the conversion time in ms, depending on the configured datarate
     * 
     * This routine checks the register for the current datarate & calculates the 
     * conversion time in ms. The conversion time is given by table 4 in the specs and 
     * is roughly independent of the conversion mode. It's value is given by 1000 devided 
     * by the datarate : 20, 90, 330 or 1000, so being 50 ms, 11 ms, 3 ms or 1 ms. 
     * 
     * If an error is encountered during the reading of the register, the largest conversion
     * rate of 50 ms is returned. 
     * 
     * @return conversion time in ms 
     */
    uint16_t getConversionTime( void );


    /**
     * @brief Checks the status register to see if a conversion result is ready
     * 
     * The bit (bit 7 in the status register) is reset when the conversion data is read. 
     * 
     * @param err_code returns the error code as the result is returned by the function
     * 
     * @return true if a conversion result is ready 
     */
    bool conversionReady( uint8_t* err_code );


    /**
     * @brief Reads a single ended result from channel
     * 
     * @param channel the channel number to read 0-3
     * @param err_code returns an error code, 0 if all was well
     * @param offset_cycles the number of offset cycles to assess the internal bias (default 0)
     * 
     * @return the value
     */
    int32_t readSingleEnded(uint8_t channel, uint8_t* err_code, uint16_t offset_samples = 0 );


    /**
     * @brief Read shorted value
     * 
     * @param err_code returns an error code, 0 if all was well
     * @param samples read and average over this many samples, default 1
     * 
     * @return the value
     */
    int32_t readShorted(uint8_t* err_code, uint16_t samples = 1 );


    /**
     * @brief Convert to millivolt
     * 
     * @param adc_count : The raw ADC counts
     * @param gain : The gain setting : ADS1219_GAIN_ONE or ADS1219_GAIN_FOUR
     * @param err_code  : the error code
     * 
     * @return the adc_count value converted t millivolts, depending on the gain & INTERNAL or EXTERNAL reference 
     */
    float milliVolts(int32_t adc_count, uint8_t gain, uint8_t* err_code);


    /**
     * @brief Send a single byte command, low level routine to build the above more advances
     * 
     * @param cmd The command byte
     * 
     * @return error code
     */
    uint8_t send_cmd(uint8_t cmd);

private:
    // Low level routines
    uint8_t _write(const uint8_t *buffer, size_t len, bool stop = true, const uint8_t *prefix_buffer = nullptr, size_t prefix_len = 0);
    uint8_t _read(uint8_t *buffer, size_t len, bool stop = true);

    uint8_t _read_register(uint8_t reg, uint8_t* data);
    uint8_t _write_register(uint8_t data);
    uint8_t _modify_register(uint8_t value, uint8_t mask );

    int32_t _read_value( uint8_t* err_code );
    int32_t _readout( uint8_t mux, uint8_t* err_code );

private:
    uint8_t  _i2c_addr;   //! I2C address, default is 0x40 when A0 and A1 both connected to DGND (see spec p22)
    uint8_t  _drdy_pin;   //! data ready pin (default is 0, meaning it's not used)
    TwoWire* _wire;       //! the wire bus
    bool     _begun;      //! flag to indicate if the device has started
    
    uint8_t  _buffer[3];  //! buffer to recieve the ADC readout value

    unsigned long _timeout_ms; //! timeout in ms to wait for the ADC conversion result

    size_t   _maxBufferSize;

    float    _aref_n;     //! analog negative reference in mV
    float    _aref_p;     //! analog positive reference in mV
};
