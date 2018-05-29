#ifndef _BOARD_COMMON_GPIO_H_
#define _BOARD_COMMON_GPIO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int gpio_port_t;
typedef int gpio_pin_t;

extern void gpio_set(gpio_port_t port, uint8_t pins);

extern void gpio_clear(gpio_port_t port, uint8_t pins);

extern void gpio_toggle(gpio_port_t port, uint8_t pins);

#ifdef __cplusplus
}
#endif

#ifndef USIP_NATIVE
#   include "gpio_test.hpp"
#endif

#ifdef USIP_BOARD
#   include "gpio_native.h"
#endif

#endif // _BOARD_COMMON_GPIO_H_

