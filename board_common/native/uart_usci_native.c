#include "uart.h"

#include <assert.h>

static uint16_t BASE_ADDRESSES[USCI_count] = {
#ifdef USCI_A0_BASE
    USCI_A0_BASE,
#endif
#ifdef USCI_A1_BASE
    USCI_A1_BASE,
#endif
#ifdef USCI_A2_BASE
    USCI_A2_BASE,
#endif
#ifdef USCI_A3_BASE
    USCI_A3_BASE,
#endif
#ifdef USCI_B0_BASE
    USCI_B0_BASE,
#endif
#ifdef USCI_B1_BASE
    USCI_B1_BASE,
#endif
#ifdef USCI_B2_BASE
    USCI_B2_BASE,
#endif
#ifdef USCI_B3_BASE
    USCI_B3_BASE,
#endif
};

volatile int read_byte = 0;

__attribute__((interrupt(USCI_A0_VECTOR)))
void USCI_A0_ISR(void) {
    switch (__even_in_range(UCA0IV, 18)) {
        case USCI_NONE: break;
        case USCI_UCRXIFG:
            //TODO find out why USCI_A_UART_receiveData doesn't link
            read_byte = (HWREG8(USCI_A0_BASE + OFS_UCAxRXBUF));
            break;
    }
}

__attribute__((interrupt(USCI_A1_VECTOR)))
void USCI_A1_ISR(void) {
    switch (__even_in_range(UCA1IV, 18)) {
        case USCI_NONE: break;
        case USCI_UCRXIFG:
            read_byte = (HWREG8(USCI_A1_BASE + OFS_UCAxRXBUF));
            break;
    }
}

bool uart_open(usci_t on, uart_baud_rate_t baud_rate, uart_t * out) {
    USCI_A_UART_initParam param = {0};

    assert(on < USCI_count);
#ifdef USCI_B0_BASE
    assert(on < USCI_B0);
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

    param.selectClockSource = USCI_A_UART_CLOCKSOURCE_ACLK;
    param.parity = USCI_A_UART_NO_PARITY;
    param.msborLsbFirst = USCI_A_UART_LSB_FIRST;
    param.numberofStopBits = USCI_A_UART_ONE_STOP_BIT;
    param.uartMode = USCI_A_UART_MODE;

    uint16_t base_address = BASE_ADDRESSES[on];

    USCI_A_UART_disable(base_address);

    USCI_A_UART_init(base_address, &param);

    USCI_A_UART_enable(base_address);
    USCI_A_UART_resetDormant(base_address);

    // XXX: TODO: rewrite transmit/recieve using interrupts
    USCI_A_UART_enableInterrupt(base_address, USCI_A_UART_RECEIVE_INTERRUPT); // Enable interrupt

    out->usci = on;

    return true;
}

uart_error_t uart_write_byte(uart_t * channel, uint8_t byte) {
    // XXX: TODO: should acquire/check that the current thread has acquired a
    // lock on the USCI module
    USCI_A_UART_transmitData(BASE_ADDRESSES[channel->usci], byte);

    read_byte = 0;
    
    return UART_NO_ERROR;
}
