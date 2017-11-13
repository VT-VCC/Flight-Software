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

bool spi_open(eusci_t eusci, uint32_t clock_rate, spi_t * out) {
    // Configure the SPI master block
    //TODO select between A and B blocks appropriately
    EUSCI_A_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_A_SPI_CLOCKSOURCE_ACLK;
    param.clockSourceFrequency = CS_getACLK();
    param.desiredSpiClock = clock_rate;
    param.msbFirst = EUSCI_A_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_A_SPI_3PIN;

    uint16_t base_address = BASE_ADDRESSES[eusci];

    // Check if the bus is currently enabled
    bool is_in_reset_state = HWREG16(base_address + OFS_UCAxCTLW0) & UCSWRST;
    if (!is_in_reset_state) {
        return false;
    }

    // Initialize the SPI master block
    EUSCI_A_SPI_initMaster(base_address, &param);

    // Enable the SPI block
    EUSCI_A_SPI_enable(base_address);

    out->eusci = eusci;

    return true;
}

void spi_close(spi_t * out) {
    uint16_t base_address = BASE_ADDRESSES[out->eusci];

    // Disable the SPI block
    EUSCI_A_SPI_enable(base_address);
}

spi_error_t spi_send_byte(spi_t * channel, uint8_t byte) {
    uint8_t received = 0;
    return spi_transfer_byte(channel, byte, &received);
}

spi_error_t spi_receive_byte(spi_t * channel, uint8_t * byte) {
    return spi_transfer_byte(channel, 0, byte);
}

spi_error_t spi_transfer_byte(spi_t * channel, uint8_t send_byte, uint8_t * receive_byte) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];

    // Check if the bus is enabled
    bool is_reset = HWREG16(base_address + OFS_UCAxCTLW0) & UCSWRST;
    if (!is_reset) {
        return SPI_CHANNEL_CLOSED;
    }

    // Wait for TX buffer to be ready
    // (transmitting when UCTXIFG = 0 is undefined behavior)
    while(!EUSCI_A_SPI_getInterruptStatus(base_address,
        EUSCI_A_SPI_TRANSMIT_INTERRUPT));

    // Transmit byte
    EUSCI_A_SPI_transmitData(base_address, send_byte);

    // Wait for byte to be received
    while(!EUSCI_A_SPI_getInterruptStatus(base_address,
        EUSCI_A_SPI_RECEIVE_INTERRUPT));

    // Receive byte
    *receive_byte = EUSCI_A_SPI_receiveData(receive_byte);

    return SPI_NO_ERROR;
}
