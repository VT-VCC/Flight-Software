#include "i2c.h"

#include <assert.h>

bool i2c_open() {
    assert(false);
    
    return true;
}


i2c_error_t i2c_write_byte(i2c_t * channel, uint8_t address, uint8_t bytes) {
    assert(false);

    return I2C_NO_ERROR;
}

i2c_error_t i2c_write_bytes(i2c_t * channel, uint8_t address, uint8_t * bytes, size_t n) {
    assert(false);

    return I2C_NO_ERROR;
}

i2c_error_t i2c_read_byte(i2c_t * channel, uint8_t address, uint8_t * bytes) {
    assert(false);

    return I2C_NO_ERROR;
}

i2c_error_t i2c_read_bytes(i2c_t * channel, uint8_t address, uint8_t * bytes, size_t n) {
    assert(false);

    return I2C_NO_ERROR;
}
