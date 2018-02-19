#include "i2c.h"

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

bool i2c_open(eusci_t eusci, i2c_data_rate_t data_rate, i2c_t * out) {
    // TODO: Need error check to make sure the same module isn't opened twice

    uint16_t base_address = BASE_ADDRESSES[eusci];
    assert(base_address == EUSCI_B0_BASE || base_address == EUSCI_B1_BASE ||
        base_address == EUSCI_B2_BASE || base_address == EUSCI_B3_BASE);

    EUSCI_B_I2C_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = CS_getSMCLK();
    param.dataRate = data_rate;
    param.byteCounterThreshold = 1;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;

    // Initialize and enable the master
    EUSCI_B_I2C_initMaster(base_address, &param);
    EUSCI_B_I2C_enable(base_address);

    // Clear and enable interrupts
    EUSCI_B_I2C_clearInterrupt(base_address,
        EUSCI_B_I2C_RECEIVE_INTERRUPT0 | EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT |
        EUSCI_B_I2C_NAK_INTERRUPT | EUSCI_B_I2C_STOP_INTERRUPT);
    EUSCI_B_I2C_enableInterrupt(base_address,
        EUSCI_B_I2C_RECEIVE_INTERRUPT0 | EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT |
        EUSCI_B_I2C_NAK_INTERRUPT | EUSCI_B_I2C_STOP_INTERRUPT);

    out->eusci = eusci;

    return true;
}

i2c_error_t i2c_write_byte(i2c_t * channel, const uint8_t data) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];

    EUSCI_B_I2C_setSlaveAddress(base_address, 0x69);

    // Enter transmit mode
    EUSCI_B_I2C_setMode(base_address, EUSCI_B_I2C_TRANSMIT_MODE);

    // Block until the bus is free
    //while (EUSCI_B_I2C_isBusBusy(base_address));

    // Transmit the byte
    EUSCI_B_I2C_masterSendSingleByte(base_address, data);

    return I2C_NO_ERROR;
}

i2c_error_t i2c_write_bytes(i2c_t * channel, const uint8_t * data, size_t length) {
    return I2C_NO_ERROR;
}

i2c_error_t i2c_read_byte(i2c_t * channel, uint8_t * data) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];

    // Enter transmit mode
    EUSCI_B_I2C_setMode(base_address, EUSCI_B_I2C_RECEIVE_MODE);

    // Block until the bus is free
    while (EUSCI_B_I2C_isBusBusy(base_address));

    // Send START and STOP then read a byte
    *data = EUSCI_B_I2C_masterReceiveSingleByte(base_address);

    return I2C_NO_ERROR;
}

i2c_error_t i2c_read_bytes(i2c_t * channel, uint8_t * data, size_t length) {
    return I2C_NO_ERROR;
}

i2c_error_t i2c_close(i2c_t * channel) {
    return I2C_NO_ERROR;
}