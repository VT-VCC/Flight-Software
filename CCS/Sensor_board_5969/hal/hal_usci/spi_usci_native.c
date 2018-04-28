#include "spi.h"

#include <assert.h>

static uint16_t BASE_ADDRESSES[USCI_count] = {
#ifdef USCI_A0_BASE
    USCI_A0,
#endif
#ifdef USCI_A1_BASE
    USCI_A1,
#endif
#ifdef USCI_A2_BASE
    USCI_A2,
#endif
#ifdef USCI_A3_BASE
    USCI_A3,
#endif
#ifdef USCI_B0_BASE
    USCI_B0,
#endif
#ifdef USCI_B1_BASE
    USCI_B1,
#endif
#ifdef USCI_B2_BASE
    USCI_B2,
#endif
#ifdef USCI_B3_BASE
    USCI_B3,
#endif
};

// Is a block's base address that of an A or B block
static bool is_usci_a_block(uint16_t base_address) {
    return (false
#       ifdef USCI_A0_BASE
            || base_address == USCI_A0_BASE
#       endif
#       ifdef USCI_A1_BASE
            || base_address == USCI_A1_BASE
#       endif
#       ifdef USCI_A2_BASE
            || base_address == USCI_A2_BASE
#       endif
#       ifdef USCI_A3_BASE
            || base_address == USCI_A3_BASE
#       endif
        );
}

static bool usci_a_spi_open(usci_t usci, uint16_t base_address, uint32_t clock_rate, spi_t * out) {
    // Check if the SPI bus is already enabled
    bool is_in_reset_state = HWREG16(base_address + OFS_UCAxCTL1) & UCSWRST;
    if (!is_in_reset_state) {
        return false;
    }

    // Configure the SPI master block
    USCI_A_SPI_initMasterParam param = {0};
    param.selectClockSource = USCI_A_SPI_CLOCKSOURCE_ACLK;
    param.clockSourceFrequency = UCS_getACLK();
    param.desiredSpiClock = clock_rate;
    param.msbFirst = USCI_A_SPI_MSB_FIRST;
    param.clockPhase = USCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = USCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;

    // Initialize the SPI master block
    USCI_A_SPI_initMaster(base_address, &param);

    // Enable the SPI block
    USCI_A_SPI_enable(base_address);

    out->usci = usci;

    return true;
}

static bool usci_b_spi_open(usci_t usci, uint16_t base_address, uint32_t clock_rate, spi_t * out) {
    // Check if the SPI bus is already enabled
    bool is_in_reset_state = HWREG16(base_address + OFS_UCBxCTL1) & UCSWRST;
    if (!is_in_reset_state) {
        return false;
    }

    // Configure the SPI master block
    USCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = USCI_B_SPI_CLOCKSOURCE_ACLK;
    param.clockSourceFrequency = UCS_getACLK();
    param.desiredSpiClock = clock_rate;
    param.msbFirst = USCI_B_SPI_MSB_FIRST;
    param.clockPhase = USCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = USCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;

    // Initialize the SPI master block
    USCI_B_SPI_initMaster(base_address, &param);

    // Enable the SPI block
    USCI_B_SPI_enable(base_address);

    out->usci = usci;

    return true;
}

bool spi_open(usci_t usci, uint32_t clock_rate, spi_t * out) {
    uint16_t base_address = BASE_ADDRESSES[usci];

    if (is_usci_a_block(base_address)) {
        return usci_a_spi_open(usci, base_address, clock_rate, out);
    }
    else {
        return usci_b_spi_open(usci, base_address, clock_rate, out);
    }
}

static void usci_a_spi_close(uint16_t base_address) {
    // Disable the SPI block
    USCI_A_SPI_disable(base_address);
}

static void usci_b_spi_close(uint16_t base_address) {
    // Disable the SPI block
    USCI_B_SPI_disable(base_address);
}

void spi_close(spi_t * out) {
    uint16_t base_address = BASE_ADDRESSES[out->usci];

    if (is_usci_a_block(base_address)) {
        usci_a_spi_close(base_address);
    }
    else {
        usci_b_spi_close(base_address);
    }
}

static spi_error_t usci_a_spi_transfer_byte(uint16_t base_address, uint8_t send_byte, uint8_t * receive_byte) {
    // Check if the SPI bus is not enabled
    bool is_in_reset_state = HWREG16(base_address + OFS_UCAxCTL1) & UCSWRST;
    if (is_in_reset_state) {
        return SPI_CHANNEL_CLOSED;
    }

    // Wait for the TX buffer to be ready, and by extension the RX buffer
    // (transmitting while UCxxIFG & UCTXIFG == 0 is undefined behavior)
    while(!USCI_A_SPI_getInterruptStatus(base_address,
        USCI_A_SPI_TRANSMIT_INTERRUPT));

    USCI_A_SPI_transmitData(base_address, send_byte);
    *receive_byte = USCI_A_SPI_receiveData(base_address);

    return SPI_NO_ERROR;
}

static spi_error_t usci_b_spi_transfer_byte(uint16_t base_address, uint8_t send_byte, uint8_t * receive_byte) {
    // Check if the SPI bus is not enabled
    bool is_in_reset_state = HWREG16(base_address + OFS_UCBxCTL1) & UCSWRST;
    if (is_in_reset_state) {
        return SPI_CHANNEL_CLOSED;
    }

    // Wait for the TX buffer to be ready, and by extension the RX buffer
    // (transmitting while UCxxIFG & UCTXIFG == 0 is undefined behavior)
    while(!USCI_B_SPI_getInterruptStatus(base_address,
        USCI_B_SPI_TRANSMIT_INTERRUPT));

    USCI_B_SPI_transmitData(base_address, send_byte);
    *receive_byte = USCI_B_SPI_receiveData(base_address);

    return SPI_NO_ERROR;
}

spi_error_t spi_transfer_byte(spi_t * channel, uint8_t send_byte, uint8_t * receive_byte) {
    uint16_t base_address = BASE_ADDRESSES[channel->usci];

    if (is_usci_a_block(base_address)) {
        return usci_a_spi_transfer_byte(base_address, send_byte, receive_byte);
    }
    else {
        return usci_b_spi_transfer_byte(base_address, send_byte, receive_byte);
    }
}
