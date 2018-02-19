#include "i2c.h"

/******************************************************************************\
 *  Public interface implementations                                          *
\******************************************************************************/

/*
i2c_error_t i2c_write_byte(i2_t * channel, const uint8_t * byte) {
    return i2c_write_byte(byte, 1);
}

i2c_error_t i2c_read_byte(i2_t * channel, const uint8_t byte) {
    return i2c_read_bytes(&byte, 1);
}
*/

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
