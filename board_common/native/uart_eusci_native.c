#include "uart.h"

#include <assert.h>

static uint16_t BASE_ADDRESSES[EUSCI_count] = {
#ifdef EUSCI_A0_BASE
    EUSCI_A0_BASE,
#endif
#ifdef EUSCI_A1_BASE
    EUSCI_A1_BASE,
#endif
#ifdef EUSCI_A2_BASE
    EUSCI_A2_BASE,
#endif
#ifdef EUSCI_A3_BASE
    EUSCI_A3_BASE,
#endif
#ifdef EUSCI_B0_BASE
    EUSCI_B0_BASE,
#endif
#ifdef EUSCI_B1_BASE
    EUSCI_B1_BASE,
#endif
#ifdef EUSCI_B2_BASE
    EUSCI_B2_BASE,
#endif
#ifdef EUSCI_B3_BASE
    EUSCI_B3_BASE,
#endif
};

bool uart_open(eusci_t on, uart_baud_rate_t baud_rate, uart_t * out) {
    EUSCI_A_UART_initParam param;

    assert(on < EUSCI_count);
#ifdef EUSCI_B0_BASE
    assert(on < EUSCI_B0);
#endif

    // XXX: Assumes a 16MHz core clock
    switch (baud_rate) {
        case BAUD_9600:
            param.clockPrescalar = 104;
            param.firstModReg = 2;
            param.secondModReg = 182;
            param.overSampling = 1;
            break;
        case BAUD_115200:
            param.clockPrescalar = 8;
            param.firstModReg = 10;
            param.secondModReg = 247;
            param.overSampling = 1;
            break;
        default:
            assert(false);
    }

    uint16_t base_address = BASE_ADDRESSES[on];

    EUSCI_A_UART_init(base_address, &param);
    EUSCI_A_UART_enable(base_address);
    // XXX: TODO: rewrite transmit/recieve using interrupts

    out->eusci = on;
}

uart_error_t uart_write_byte(uart_t * channel, uint8_t byte) {
    // XXX: TODO: should acquire/check that the current thread has acquired a
    // lock on the EUSCI module
    EUSCI_A_UART_transmitData(BASE_ADDRESSES[channel->eusci], byte);
}
