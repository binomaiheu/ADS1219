#pragma once

#include <Arduino.h>
#include <Wire.h>

// Default I2C address, A0 and A1 both to DGND
#define ADS1219_I2C_ADDRESS 0x40

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
#define ADD1219_CONFIG_MASK_DR   0xF3  // bits 2 & 3, data rate
#define ADS1219_CONFIG_MASK_GAIN 0xEF  // bit 4,
#define ADS1219_CONFIG_MASK_MUX  0x1F  // bits 5, 6 & 7 : Input multiplexer configuration


// Config and status register
#define ADS1219_GAIN_ONE         1     // gain 1 (default)
#define ADS1219_GAIN_FOUR        4     // gain 4
#define ADS1219_VREF_INTERNAL    0     // internal 2.048 V reference
#define ADS1219_VREF_EXTERNAL    1     // external reference using REFP and REFN inputs


// Some error codes
#define ADS1219_OK                 0     // all is well
#define ADS1219_BUFFER_TOO_LARGE   1     // the buffers passed to the _write is too large for the architecture internal Wire buffer
#define ADS1219_FAILED_TO_WRITE    2     // error during writing, wrong number of bytes returned
#define ADS1219_FAILED_TO_END      3     // error during endTransmission
#define ADS1219_FAILED_TO_RECEIVE  4     // incorrect number of bytes recieved from ADS
#define ADS1219_INVALID_GAIN       5     // invalid gain
#define ADS1219_INVALID_VREF       6     // invalid vref

class ADS1219 {
public:
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
     */
    uint8_t reset(void);


    /**
     * @brief Start command
     * 
     *  
     */
    uint8_t start(void);


    /**
     * @brief Power down command
     * 
     * The POWERDOWN command places the device into power-down mode. This command shuts down all internal
     * analog components, but holds all register values. In case the POWERDOWN command is issued when a
     * conversion is ongoing, the conversion completes before the ADS1219 enters power-down mode. As soon as a
     * START/SYNC command is issued, all analog components return to their previous states.
     */
    uint8_t powerDown(void);


    /**
     * @brief Send a single byte command 
     */
    uint8_t send_cmd(uint8_t cmd);


    /**
     * @brief return the device gain 
     */
    uint8_t getGain( uint8_t* gain );

    /**
     * @brief set the device gain 
     */
    uint8_t setGain( uint8_t gain = ADS1219_GAIN_ONE );


    /**
     * @brief getVREF
     * 
     * Returns the voltage type
     */
    uint8_t getVREF( uint8_t *type );

    /**
     * @brief setVREF
     * 
     * Set the voltage type : reference, internal or external
     */
    uint8_t setVREF( uint8_t type );


private:
    // Low level routines
    uint8_t _write(const uint8_t *buffer, size_t len, bool stop = true, const uint8_t *prefix_buffer = nullptr, size_t prefix_len = 0);
    uint8_t _read(uint8_t *buffer, size_t len, bool stop = true);

    uint8_t _read_register(uint8_t reg, uint8_t* data);
    uint8_t _write_register(uint8_t data);
    uint8_t _modify_register(uint8_t value, uint8_t mask );

private:
    uint8_t  _i2c_addr;   //! I2C address, default is 0x40 when A0 and A1 both connected to DGND (see spec p22)
    uint8_t  _drdy_pin;   //! data ready pin (default is 0, meaning it's not used)
    TwoWire* _wire;       //! the wire bus
    bool     _begun;      //! flag to indicate if the device has started

    size_t   _maxBufferSize;
};
