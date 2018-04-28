#include <msp430.h>
#include <driverlib.h>

#include "gpio.h"
#include "gpio_native.h"

static volatile uint8_t * get_port(gpio_port_t port) {
    switch (port) {
    default:
    case GPIO_PORT_1: return &P1OUT;
    case GPIO_PORT_2: return &P2OUT;
    case GPIO_PORT_3: return &P3OUT;
    case GPIO_PORT_4: return &P4OUT;
    }
}

void gpio_set(gpio_port_t port, uint8_t pins) {
    *get_port(port) |= pins;
}

void gpio_clear(gpio_port_t port, uint8_t pins) {
    *get_port(port) &= ~pins;
}

void gpio_toggle(gpio_port_t port, uint8_t pins) {
    *get_port(port) ^= pins;
}
