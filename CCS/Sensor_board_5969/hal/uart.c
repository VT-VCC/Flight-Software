#include <hal/uart.h>

/******************************************************************************\
 *  Public interface implementations                                          *
\******************************************************************************/
uart_error_t uart_write_bytes(uart_t * channel, const uint8_t * bytes, size_t n) {
    uart_error_t err;
    size_t i;
    for (i = 0; i < n; ++i) {
        err = uart_write_byte(channel, bytes[i]);
        if (err != UART_NO_ERROR) {
            return err;
        }
    }
    return UART_NO_ERROR;
}

uart_error_t uart_write_string(uart_t * channel, const char * str) {
    uart_error_t err;
    while (*str) {
        err = uart_write_byte(channel, (uint8_t) *str);
        if (err != UART_NO_ERROR) {
            return err;
        }
        ++str;
    }
    return err;
}

#ifndef NDEBUG
const char * uart_error_string(uart_error_t t) {
    switch(t) {
#       define STRING_OP(E) case UART_ ## E: return #E;
        UART_ERROR_LIST(STRING_OP)
#       undef STRING_OP
        default:
            return "UART error unknown";
    }
}
#endif
