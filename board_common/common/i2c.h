#ifndef _BOARD_COMMON_I2C_H_
#define _BOARD_COMMON_I2C_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************\
 *  Macro list for the ways writing to a UART can fail                        *
\******************************************************************************/
/// Macro for defining thing related to UART errors
#define I2C_ERROR_LIST(OP) \
    OP(NO_ERROR) \
    OP(CHANNEL_CLOSED) \
    OP(SIGNAL_FAULT)

/// Enum representing possible error states for a UART channel.
typedef enum uart_error {
#   define ENUM_OP(E) I2C_ ## E,
    I2C_ERROR_LIST(ENUM_OP)
#   undef ENUM_OP
    I2C_count
} i2c_error_t;

#ifndef NDEBUG
/// Get a string representation of the error. Only available in debug builds
const char * i2c_error_string(uart_error_t t);
#endif

/** Opaque type for the I2C state
 *
 */
typedef struct i2c i2c_t;

#ifdef __cplusplus
}
#endif

i2c_error_t i2c_write_bytes(i2_t * channel, const uint8_t * bytes, size_t n);

i2c_error_t i2c_read_bytes(i2_t * channel, const uint8_t * bytes, size_t n);

#ifdef USIP_NATIVE
#   include "i2c_native.h"
#else
#   include "i2c_test.hpp"
#endif

#endif // _BOARD_COMMON_I2c_H_
