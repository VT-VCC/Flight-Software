#ifndef _NATIVE_GPIO_H_
#define _NATIVE_GPIO_H_

typedef enum gpio_port {
    GPIO_PORT_1,
    GPIO_PORT_2,
    GPIO_PORT_3,
    GPIO_PORT_4,
    GPIO_PORT_5,
    GPIO_PORT_6,
    GPIO_PORT_7,
    GPIO_PORT_COUNT
} gpio_port;

typedef enum gpio_pin {
    GPIO_PIN_0 = 1 << 0,
    GPIO_PIN_1 = 1 << 1,
    GPIO_PIN_2 = 1 << 2,
    GPIO_PIN_3 = 1 << 3,
    GPIO_PIN_4 = 1 << 4,
    GPIO_PIN_5 = 1 << 5,
    GPIO_PIN_6 = 1 << 6,
    GPIO_PIN_7 = 1 << 7
} gpio_pin;

#endif // _NATIVE_GPIO_H_
