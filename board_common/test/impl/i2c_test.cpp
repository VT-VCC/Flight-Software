#include "i2c_test.hpp"

void i2c_open(i2c_t * out) {
}

i2c_error_t i2c_write_byte(i2c_t * channel, uint8_t address, uint8_t byte) {
    return I2C_NO_ERROR;
}

i2c_error_t i2c_write_bytes(i2c_t * channel, uint8_t address, uint8_t * bytes, size_t n) {
    return I2C_NO_ERROR;
}

i2c_error_t i2c_read_byte(i2c_t * channel, uint8_t address, uint8_t * byte) {
    return I2C_NO_ERROR;
}

i2c_error_t i2c_read_bytes(i2c_t * channel, uint8_t address, uint8_t * bytes, size_t n) {
    return I2C_INCOMPLETE;
}
