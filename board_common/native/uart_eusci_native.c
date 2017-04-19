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

volatile int read_byte = 0;

__attribute__((interrupt(USCI_A0_VECTOR)))
void USCI_A0_ISR(void) {
    switch (__even_in_range(UCA0IV, 18)) {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
            P4OUT = 1 << 6;
            read_byte = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
            break;
        case USCI_UART_UCTXIFG: break;
        case USCI_UART_UCSTTIFG: break;
        case USCI_UART_UCTXCPTIFG: break;
    }
    if (!read_byte) { read_byte = 1; }
}

__attribute__((interrupt(USCI_A1_VECTOR)))
void USCI_A1_ISR(void) {
    switch (__even_in_range(UCA1IV, 18)) {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
            read_byte = EUSCI_A_UART_receiveData(EUSCI_A1_BASE);
            break;
        case USCI_UART_UCTXIFG: break;
        case USCI_UART_UCSTTIFG: break;
        case USCI_UART_UCTXCPTIFG: break;
    }
    if (!read_byte) { read_byte = 1; }
}

bool uart_open(eusci_t on, uart_baud_rate_t baud_rate, uart_t * out) {
    EUSCI_A_UART_initParam param = {0};

    assert(on < EUSCI_count);
#ifdef EUSCI_B0_BASE
    assert(on < EUSCI_B0);
#endif

    // XXX: Assumes the clock source is running at 32.768 Khz
    // In low frequency mode, the source is the ACLK
    // In high frequency mode, the source is the SMCLK
    // Computed using
    // http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
    switch (baud_rate) {
        case BAUD_9600:
            param.clockPrescalar = 3;
            param.firstModReg = 0;
            param.secondModReg = 146;
            param.overSampling = 0;
            break;
        case BAUD_115200:
            assert(false);
        default:
            assert(false);
    }

    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_ACLK;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;

    uint16_t base_address = BASE_ADDRESSES[on];

    EUSCI_A_UART_disable(base_address);

    EUSCI_A_UART_init(base_address, &param);

    EUSCI_A_UART_enable(base_address);
    EUSCI_A_UART_resetDormant(base_address);

    // XXX: TODO: rewrite transmit/recieve using interrupts
    EUSCI_A_UART_enableInterrupt(base_address, EUSCI_A_UART_RECEIVE_INTERRUPT); // Enable interrupt

    out->eusci = on;

    return true;
}

uart_error_t uart_write_byte(uart_t * channel, uint8_t byte) {
    // XXX: TODO: should acquire/check that the current thread has acquired a
    // lock on the EUSCI module
    EUSCI_A_UART_transmitData(BASE_ADDRESSES[channel->eusci], byte);

    if (read_byte) {
        for (int i = 0; i < 32; ++i) {
            P4OUT ^= 1 << 6;
            __delay_cycles(800000UL);
        }
        read_byte = 0;
    }

    return UART_NO_ERROR;
}
