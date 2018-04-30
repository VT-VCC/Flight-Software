/*
 * global_defines.h
 *
 *  Created on: Apr 30, 2018
 *      Author: Nick
 */

#ifndef GLOBAL_DEFINES_H_
#define GLOBAL_DEFINES_H_

#include <hal/uart.h>

#define PERSISTENT __attribute__((section(".persistent")))

extern uart_t standard_output;

// Print a formatted message across the UART output
#define DEBUG_VA_ARGS(...) , ## __VA_ARGS__
#define DEBUG(format, ...) do { \
        char buffer[255]; \
        int len = snprintf(buffer, 255, (format) DEBUG_VA_ARGS(__VA_ARGS__)); \
        uart_write_bytes(&standard_output, buffer, len); \
    } while(0)
#define WTF() DEBUG("[ERROR] %s (%s:%d)\n", __func__, __FILE__, __LINE__)

#endif /* GLOBAL_DEFINES_H_ */
