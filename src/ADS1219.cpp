#include "ADS1219.h"


ADS1219::ADS1219(uint8_t i2c_addr, uint8_t drdy_pin, TwoWire* wire)
    : _i2c_addr(i2c_addr)
    , _drdy_pin(drdy_pin)
    , _wire(wire)
    , _timeout_ms(100UL)
{
}


ADS1219::~ADS1219()
{
}


void ADS1219::begin( void )
{
    _wire->begin();
    _begun = true;

#ifdef ARDUINO_ARCH_SAMD
    _maxBufferSize = 256; // and not 250 as in Adafruit_I2CDevice.cpp ?
#else
    _maxBufferSize = 32;  // see buffer size in Wire.h for other architectures
#endif

    return;
}


bool ADS1219::detect( void )
{
    if (!_begun) return false;

    _wire->beginTransmission(_i2c_addr);
    if (_wire->endTransmission() == 0) return true;

    return false;   
}


uint8_t ADS1219::reset(void)
{
    return send_cmd(ADS1219_CMD_RESET);
}


uint8_t ADS1219::start(void)
{
    return send_cmd(ADS1219_CMD_START_SYNC);
}


uint8_t ADS1219::powerDown(void)
{
    return send_cmd(ADS1219_CMD_POWERDOWN);
}


uint8_t ADS1219::getGain(uint8_t* gain )
{
    uint8_t code, r;
    code = _read_register( ADS1219_CMD_RREG_CONFIG, &r );
    if ( code != ADS1219_OK ) return code;

    // mask the gain bit (place 4)
    if ( r & ~ADS1219_CONFIG_MASK_GAIN )
        *gain = ADS1219_GAIN_FOUR;
    else 
        *gain = ADS1219_GAIN_ONE;
    
    return ADS1219_OK;
}


uint8_t ADS1219::setGain( uint8_t gain )
{
    uint8_t value;
    if ( gain == ADS1219_GAIN_ONE ) value = 0;
    else if ( gain == ADS1219_GAIN_FOUR ) value = (1 << 4);
    else return ADS1219_INVALID_GAIN;

    return _modify_register(value, ADS1219_CONFIG_MASK_GAIN);
}


uint8_t ADS1219::getVREF( uint8_t* type )
{    
    uint8_t code, r;
    code = _read_register( ADS1219_CMD_RREG_CONFIG, &r );
    if ( code != ADS1219_OK ) return code;

    if ( r & ~ADS1219_CONFIG_MASK_VREF )
        *type = ADS1219_VREF_EXTERNAL;
    else 
        *type = ADS1219_VREF_INTERNAL;

    return ADS1219_OK;
}


uint8_t ADS1219::setVREF( uint8_t type )
{
    uint8_t value;
    if ( type == ADS1219_VREF_INTERNAL ) value = 0;
    else if ( type == ADS1219_VREF_EXTERNAL ) value = 1;
    else return ADS1219_INVALID_VREF;

    return _modify_register(value, ADS1219_CONFIG_MASK_VREF);
}


uint8_t ADS1219::getDataRate( uint8_t* rate)
{
    uint8_t code, r;
    code = _read_register( ADS1219_CMD_RREG_CONFIG, &r );
    if ( code != ADS1219_OK ) return code;

    *rate = ( r & ~ADS1219_CONFIG_MASK_DR ) >> 2;
 
    return ADS1219_OK;
}


uint8_t ADS1219::setDataRate( uint8_t rate )
{
    if ( rate > 3 ) return ADS1219_INVALID_DATARATE;
    return _modify_register(rate << 2, ADS1219_CONFIG_MASK_DR); 
}


uint8_t ADS1219::setConversionMode( uint8_t mode )
{
    if ( mode > 1 ) return ADS1219_INVALID_CM;
    return _modify_register(mode << 1, ADS1219_CONFIG_MASK_CM); 
}


uint8_t ADS1219::getConversionMode( uint8_t* mode )
{
    uint8_t code, r;
    code = _read_register( ADS1219_CMD_RREG_CONFIG, &r );
    if ( code != ADS1219_OK ) return code;

    *mode = ( r & ~ADS1219_CONFIG_MASK_CM ) >> 1;
 
    return ADS1219_OK;
}


uint16_t ADS1219::getConversionTime( void )
{
    // we will round the conversion times to the next integer ms in the readout code below !
    // see specs table 4. 
    uint8_t rate;
    if ( getDataRate(&rate) ) return 50; // return largest value if error is encountered 
    switch( rate ) 
    {
        case ADS1219_DATARATE_20SPS:
            return 50;
        case ADS1219_DATARATE_90SPS:
            return 1;
        case ADS1219_DATARATE_330SPS:
            return 3;
        case ADS1219_DATARATE_1000SPS:
            return 1;
        default:
            return 50;
    }
}

bool ADS1219::conversionReady( uint8_t* err_code )
{
    uint8_t stat;
    *err_code = _read_register( ADS1219_CMD_RREG_STATUS, &stat );
    if ( *err_code != ADS1219_OK ) return false;

    return ( ( stat & 0x80 ) == 0x80 );
}



int32_t ADS1219::readSingleEnded( uint8_t channel, uint8_t* err_code )
{
    uint8_t mux;

    switch(channel)
    {
        case 0:
            mux = ADS1219_MUX_SINGLE_0;
            break;
        case 1:
            mux = ADS1219_MUX_SINGLE_1;
            break;
        case 2:
            mux = ADS1219_MUX_SINGLE_2;
            break;
        case 3:
            mux = ADS1219_MUX_SINGLE_3;
            break;
        default:
            *err_code = ADS1219_INVALID_MUX;
            return 0x80000000;
    }

    return _readout(mux, err_code );
}

 int32_t ADS1219::readShorted(uint8_t* err_code )
 {
    return _readout(ADS1219_MUX_SHORTED, err_code );
 }


int32_t ADS1219::_readout( uint8_t mux, uint8_t* err_code )
{
    bool ready = false;

    // Set the multiplexer configuration
    *err_code = _modify_register(mux, ADS1219_CONFIG_MASK_MUX );
    if ( *err_code ) return 0x80000000;
    
    // Start the conversion
    *err_code = start();
    if ( *err_code ) return 0x80000000;

    // get the conversion time
    uint16_t ct = getConversionTime();

    // Wait during the conversion time, add 10 % margin in the loop below and increment in steps 
    // of 10 % untill timeout, normally after first 10 % extra time, the conversion should
    // be ready
    delay(ct);

    // Add a timeout safety  
    unsigned long tstart = millis();
    while( ! ready && ( (millis() - tstart) < _timeout_ms ) ) {
        delay( ct > 20 ? 5 : 1 ); // extra delay of 5 ms for 50 ms conversion time, for the rest delay additional 1 ms
        ready = conversionReady(err_code);
    }

    if ( ! ready ) {
        *err_code = ADS1219_TIMEOUT;
        return 0x80000000;
    }

    return _read_value(err_code);
}




uint8_t ADS1219::send_cmd(uint8_t cmd)
{
    return _write(&cmd, 1);
}


uint8_t ADS1219::_write(const uint8_t *buffer, size_t len, bool stop, const uint8_t *prefix_buffer, size_t prefix_len)
{

    if ( (len + prefix_len) > this->maxBufferSize() ) 
        return ADS1219_BUFFER_TOO_LARGE;

    _wire->beginTransmission(_i2c_addr);

    // write prefix, usually address
    if ( (prefix_len != 0) && ( prefix_buffer != nullptr ) )
    {
        if ( _wire->write(prefix_buffer, prefix_len) != prefix_len ) 
            return ADS1219_FAILED_TO_WRITE;
    }

    // write data itself
    if ( _wire->write(buffer, len) != len )
        return ADS1219_FAILED_TO_WRITE;
    
    
    // end
    if ( _wire->endTransmission(stop) != 0)
        return ADS1219_FAILED_TO_END;

    return ADS1219_OK;
}


uint8_t ADS1219::_read(uint8_t *buffer, size_t len, bool stop)
{
    // different api here, for older architectures (e.g. atmelavr, only uint8_t version)
#ifdef ARDUINO_ARCH_SAMD
    size_t recv = _wire->requestFrom(_i2c_addr, len, stop);
#else
    size_t recv = _wire->requestFrom(_i2c_addr, static_cast<uint8_t>(len), static_cast<uint8_t>(stop));
#endif

    if (recv != len )
        return ADS1219_FAILED_TO_RECEIVE;

    // recieve the bytes from the buffer
    for (uint16_t i = 0; i < len; i++) {
        buffer[i] = _wire->read();
    }

    return ADS1219_OK;
}


uint8_t ADS1219::_read_register(uint8_t reg, uint8_t* data)
{
    uint8_t code;
    
    // send the command to read the register
    code = send_cmd(reg);
    if ( code != ADS1219_OK ) return code;

    // read result
    return _read(data, 1);
}


uint8_t ADS1219::_write_register(uint8_t data)
{
    uint8_t reg = ADS1219_CMD_WREG;
    // write the data, prefixed by the register
    // the ADS12129 has 2 8 bits registers, so we treat them separately here, 
    return _write( &data, 1, true, &reg, 1 );
}


uint8_t ADS1219::_modify_register(uint8_t value, uint8_t mask )
{
    uint8_t code, data;

    // read config register
    code = _read_register(ADS1219_CMD_RREG_CONFIG, &data);
    if ( code != ADS1219_OK ) return code;

    // modify, also mask the value bits, should be at the right position !
    // mask is 1 everywhere, except for the relevant bits
    data = (data & mask) | (value & ~mask);

    // write back
    return _write_register(data);
}

int32_t ADS1219::_read_value( uint8_t* err_code )
{
    // send the read command, if an error happenend, return max 32 bit integer, outside 24bit range !
    *err_code = send_cmd(ADS1219_CMD_RDATA);
    if ( *err_code ) return 0x80000000;

    // now get 3 bytes back
    *err_code = _read(_buffer, 3);
    if ( *err_code ) return 0x80000000;

    // now decode the bytes
    // start from the high end byte & shift to lower, that way C++ takes care of the 
    // sign while bitshifting ! 
    return (
         ( static_cast<int32_t>(_buffer[0]) << 24) | 
         ( static_cast<int32_t>(_buffer[1]) << 16) | 
         ( static_cast<int32_t>(_buffer[2]) << 8) ) >> 8;
}
}
