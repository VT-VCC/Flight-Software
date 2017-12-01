#include "i2c.h"
/******************************************************************************\
 *  Public interface implementations                                          *
\******************************************************************************/

i2c_error_t i2c_write_bytes(i2_t * channel, const uint8_t * bytes, size_t n) {
    i2c_error_t err;
    for (size_t i = 0; i < n; i++) {
        err = uart_write_byte(channel, bytes[i]);
        if (err != I2C_NO_ERROR)
            return err;
    }
    return err;
}

i2c_error_t i2c_read_bytes(i2_t * channel, const uint8_t * bytes, size_t n) {
    i2c_error_t err;
    for (size_t i = 0; i < n; i++) {
        err = uart_read_byte(channel, bytes[i]);
        if (err != I2C_NO_ERROR)
            return err;
    }
    return err;
}

i2c_error_t i2c_read_byte(i2_t * channel, const uint8_t byte) {
    return i2c_read_bytes(&byte, 1);
}

i2c_error_t i2c_write_byte(i2_t * channel, const uint8_t * byte) {
    return i2c_write_byte(byte, 1);
}

#ifndef NDEBUG
const char * i2c_error_string(i2c_error_t t) {
    switch(t) {
#       define STRING_OP(E) case I2C_ ## E: return #E;
        I2C_ERROR_LIST(STRING_OP)
#       undef STRING_OP
        default:
            return "I2C error unknown";
    }
}
#endif
