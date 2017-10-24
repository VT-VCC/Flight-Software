#include "i2c.h"
/******************************************************************************\
 *  Public interface implementations                                          *
\******************************************************************************/
void i2c_open() {}

void i2c_write_string() {}

void i2c_read_string() {}

void i2c_write_bytes() {}

void i2c_read_bytes() {}

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
