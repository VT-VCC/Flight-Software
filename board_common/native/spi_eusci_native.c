#include "spi.h"

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

/*
// Buffer to store received bytes
static volatile uint8_t rx_buffer[8] = {0};

// Generate interrupt handlers for all blocks
#define EMIT_USCI_ISR(block, sub_block, buffer_index) \
    __attribute__((interrupt(USCI_##sub_block##_VECTOR))) \
    void USCI_##sub_block##_ISR(void) { \
        switch (__even_in_range(UC##sub_block##IV, 18)) { \
            // USCI RX interrupt flag \
            case USCI_SPI_UCRXIFG: { \
                rx_buffer[buffer_index] = \
                    EUSCI_##block##_SPI_receiveData(EUSCI_##sub_block##_BASE); \
                break; \
            } \
            default: break; \
        } \
    }

#ifdef EUSCI_A0_BASE
    EMIT_USCI_ISR(A, A0, 0)
#endif
#ifdef EUSCI_A1_BASE
    EMIT_USCI_ISR(A, A1, 1)
#endif
#ifdef EUSCI_A2_BASE
    EMIT_USCI_ISR(A, A2, 2)
#endif
#ifdef EUSCI_A3_BASE
    EMIT_USCI_ISR(A, A3, 3)
#endif
#ifdef EUSCI_B0_BASE
    EMIT_USCI_ISR(B, B0, 4)
#endif
#ifdef EUSCI_B1_BASE
    EMIT_USCI_ISR(B, B1, 5)
#endif
#ifdef EUSCI_B2_BASE
    EMIT_USCI_ISR(B, B2, 6)
#endif
#ifdef EUSCI_B3_BASE
    EMIT_USCI_ISR(B, B3, 7)
#endif
*/

// Is a block's base address that of an A or B block
static bool is_eusci_a_block(uint16_t base_address) {
    return (false
#       ifdef EUSCI_A0_BASE
            || base_address == EUSCI_A0_BASE
#       endif
#       ifdef EUSCI_A1_BASE
            || base_address == EUSCI_A1_BASE
#       endif
#       ifdef EUSCI_A2_BASE
            || base_address == EUSCI_A2_BASE
#       endif
#       ifdef EUSCI_A3_BASE
            || base_address == EUSCI_A3_BASE
#       endif
        );
}

static bool eusci_a_spi_open(uint16_t base_address, uint32_t clock_rate, spi_t * out) {
    // Configure the SPI master block
    EUSCI_A_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_A_SPI_CLOCKSOURCE_ACLK;
    param.clockSourceFrequency = CS_getACLK();
    param.desiredSpiClock = clock_rate;
    param.msbFirst = EUSCI_A_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_A_SPI_3PIN;

    // Check if the bus is currently enabled
    bool is_in_reset_state = HWREG16(base_address + OFS_UCAxCTLW0) & UCSWRST;
    if (!is_in_reset_state) {
        return false;
    }

    // Initialize the SPI master block
    EUSCI_A_SPI_initMaster(base_address, &param);

    // Enable the SPI block
    EUSCI_A_SPI_enable(base_address);

/*
    // Clear and enable the RX interrupt
    EUSCI_A_SPI_clearInterrupt(base_address, EUSCI_A_SPI_RECEIVE_INTERRUPT);
    EUSCI_A_SPI_enableInterrupt(base_address, EUSCI_A_SPI_RECEIVE_INTERRUPT);
*/

    out->eusci = eusci;

    return true;
}

static bool eusci_b_spi_open(uint16_t base_address, uint32_t clock_rate, spi_t * out) {
    // Configure the SPI master block
    EUSCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_ACLK;
    param.clockSourceFrequency = CS_getACLK();
    param.desiredSpiClock = clock_rate;
    param.msbFirst = EUSCI_B_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_B_SPI_3PIN;

    // Initialize the SPI master block
    EUSCI_B_SPI_initMaster(base_address, &param);

    // Enable the SPI block
    EUSCI_B_SPI_enable(base_address);

/*
    // Clear and enable the RX interrupt
    EUSCI_B_SPI_clearInterrupt(base_address, EUSCI_B_SPI_RECEIVE_INTERRUPT);
    EUSCI_B_SPI_enableInterrupt(base_address, EUSCI_B_SPI_RECEIVE_INTERRUPT);
*/

    out->eusci = eusci;

    return true;
}

bool spi_open(eusci_t eusci, uint32_t clock_rate, spi_t * out) {
    uint16_t base_address = BASE_ADDRESSES[eusci];

    // Check if the SPI bus is already enabled
    bool is_in_reset_state = HWREG16(base_address + OFS_UCBxCTLW0) & UCSWRST;
    if (!is_in_reset_state) {
        return false;
    }

    if (is_eusci_a_block(base_address)) {
        return eusci_a_spi_open(base_address, clock_rate, out);
    }
    else {
        return eusci_b_spi_open(base_address, clock_rate, out);
    }
}

static void eusci_a_spi_close(uint16_t base_address) {
/*
    // Disable the RX interrupt
    EUSCI_A_SPI_disableInterrupt(base_address, EUSCI_A_SPI_RECEIVE_INTERRUPT);
*/

    // Disable the SPI block
    EUSCI_A_SPI_disable(base_address);
}

static void eusci_b_spi_close(uint16_t base_address) {
/*
    // Disable the RX interrupt
    EUSCI_B_SPI_disableInterrupt(base_address, EUSCI_A_SPI_RECEIVE_INTERRUPT);
*/

    // Disable the SPI block
    EUSCI_B_SPI_disable(base_address);
}

void spi_close(spi_t * out) {
    uint16_t base_address = BASE_ADDRESSES[out->eusci];

    if (is_eusci_a_block(base_address)) {
        eusci_a_spi_close(base_address);
    }
    else {
        eusci_b_spi_close(base_address);
    }
}

/*
static spi_error_t eusci_a_spi_send_byte(uint16_t base_address, uint8_t send_byte) {
    // Wait for the TX buffer to be ready
    // (transmitting when UCTXIFG = 0 is undefined behavior)
    while(!EUSCI_A_SPI_getInterruptStatus(base_address,
        EUSCI_A_SPI_TRANSMIT_INTERRUPT));

    rx_buffer = 0;

    EUSCI_A_SPI_transmitData(base_address, send_byte);

    return SPI_NO_ERROR;
}

static spi_error_t eusci_b_spi_send_byte(uint16_t base_address, uint8_t send_byte) {
    // Wait for the TX buffer to be ready
    // (transmitting when UCTXIFG = 0 is undefined behavior)
    while(!EUSCI_B_SPI_getInterruptStatus(base_address,
        EUSCI_B_SPI_TRANSMIT_INTERRUPT));

    rx_buffer = 0;

    EUSCI_B_SPI_transmitData(base_address, send_byte);

    return SPI_NO_ERROR;
}

spi_error_t spi_send_byte(spi_t * channel, uint8_t send_byte) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];

    if (is_eusci_a_block(base_address)) {
        eusci_a_spi_send_byte(base_address, send_byte);
    }
    else {
        eusci_b_spi_send_byte(base_address, send_byte);
    }
}

spi_error_t spi_receive_byte(spi_t * channel, uint8_t * receive_byte) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];

    switch (base_address) {
#       ifdef EUSCI_A0_BASE
            case EUSCI_A0_BASE:
                *receive_byte = rx_buffer[0];
                break;
#       endif
#       ifdef EUSCI_A1_BASE
            case EUSCI_A1_BASE:
                *receive_byte = rx_buffer[1];
                break;
#       endif
#       ifdef EUSCI_A2_BASE
            case EUSCI_A2_BASE:
                *receive_byte = rx_buffer[2];
                break;
#       endif
#       ifdef EUSCI_A3_BASE
            case EUSCI_A3_BASE:
                *receive_byte = rx_buffer[3];
                break;
#       endif
#       ifdef EUSCI_B0_BASE
            case EUSCI_B0_BASE:
                *receive_byte = rx_buffer[4];
                break;
#       endif
#       ifdef EUSCI_B1_BASE
            case EUSCI_B1_BASE:
                *receive_byte = rx_buffer[5];
                break;
#       endif
#       ifdef EUSCI_B2_BASE
            case EUSCI_B2_BASE:
                *receive_byte = rx_buffer[6];
                break;
#       endif
#       ifdef EUSCI_B3_BASE
            case EUSCI_B3_BASE:
                *receive_byte = rx_buffer[7];
                break;
#       endif
    }

    return SPI_NO_ERROR;
}

spi_error_t spi_transfer_byte(spi_t * channel, uint8_t send_byte, uint8_t * receive_byte) {
    spi_error_t err = spi_send_byte(channel, send_byte);
    if (err != SPI_NO_ERROR) {
        return err;
    }

    return spi_receive_byte(channel, receive_byte);
}
*/

spi_error_t spi_send_byte(spi_t * channel, uint8_t byte) {
    uint8_t received = 0;
    return spi_transfer_byte(channel, byte, &received);
}

spi_error_t spi_receive_byte(spi_t * channel, uint8_t * byte) {
    return spi_transfer_byte(channel, 0, byte);
}

static spi_error_t eusci_a_spi_transfer_byte(uint16_t base_address, uint8_t send_byte, uint8_t * receive_byte) {
    // Wait for the TX buffer to be ready, and by extension the RX buffer
    // (transmitting while UCxxIFG & UCTXIFG == 0 is undefined behavior)
    while(!EUSCI_A_SPI_getInterruptStatus(base_address,
        EUSCI_A_SPI_TRANSMIT_INTERRUPT));

    EUSCI_A_SPI_transmitData(base_address, send_byte);
    *receive_byte = EUSCI_A_SPI_receiveData(receive_byte);

    return SPI_NO_ERROR;
}

static spi_error_t eusci_b_spi_transfer_byte(uint16_t base_address, uint8_t send_byte, uint8_t * receive_byte) {
    // Wait for the TX buffer to be ready, and by extension the RX buffer
    // (transmitting while UCxxIFG & UCTXIFG == 0 is undefined behavior)
    while(!EUSCI_B_SPI_getInterruptStatus(base_address,
        EUSCI_B_SPI_TRANSMIT_INTERRUPT));

    EUSCI_B_SPI_transmitData(base_address, send_byte);
    *receive_byte = EUSCI_B_SPI_receiveData(receive_byte);

    return SPI_NO_ERROR;
}

spi_error_t spi_transfer_byte(spi_t * channel, uint8_t send_byte, uint8_t * receive_byte) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];

    // Check if the SPI bus is not enabled
    bool is_in_reset_state = HWREG16(base_address + OFS_UCBxCTLW0) & UCSWRST;
    if (is_in_reset_state) {
        return SPI_CHANNEL_CLOSED;
    }

    if (is_eusci_a_block(base_address)) {
        return eusci_a_spi_transfer_byte(base_address, send_byte, receive_byte);
    }
    else {
        return eusci_b_spi_transfer_byte(base_address, send_byte, receive_byte);
    }
}
