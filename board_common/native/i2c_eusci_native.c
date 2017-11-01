#include "i2c_eusci_native.h"

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


__attribute__((interrupt(USCI_B0_VECTOR)))
void USCI_B0_ISR(void) {
  // TODO: Figure out proper inturrupt vectors to use here
}


__attribute__((interrupt(USCI_B1_VECTOR)))
void USCI_B1_ISR(void) {
  // TODO: Figure out proper inturrupt vectors to use here
}

bool i2c_open(eusci_t eusci, i2c_t * out) {

    // TODO: Need error check to make sure the same module isn't opened twice

    assert(on < EUSCI_count);
#ifdef EUSCI_B0_BASE
    assert(on < EUSCI_B0);
#endif

    EUSCI_B_I2C_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = CS_getSMCLK();
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
    param.byteCounterThreshold = RXCOUNT;
    param.autoSTOPGeneration = EUSCI_B_I2C_SEND_STOP_AUTOMATICALLY_ON_BYTECOUNT_THRESHOLD;

    uint16_t base_address = BASE_ADDRESSES[eusci];

    // Start the I2C in Master mode
    EUSCI_B_I2C_initMaster(base_address, &param);

    //uint8_t slave_address = 0x00;

    //EUSCI_B_I2C_setSlaveAddress(base_address, slave_address);

    EUSCI_B_I2C_setMode(base_address, EUSCI_B_I2C_RECEIVE_MODE);

    //Enable I2C Module to start operations
    EUSCI_B_I2C_enable(base_address);

    EUSCI_B_I2C_clearInterrupt(base_address,
                               EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
                               EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT +
                               EUSCI_B_I2C_NAK_INTERRUPT +
                               EUSCI_B_I2C_STOP_INTERRUPT
                               );

    //Enable master Receive interrupt
    EUSCI_B_I2C_enableInterrupt(base_address,
                                EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
                                EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT +
                                EUSCI_B_I2C_NAK_INTERRUPT +
                                EUSCI_B_I2C_STOP_INTERRUPT
                                );

    out->eusci = on;

    return true;
}

i2c_error_t i2c_write_byte(i2c_t * channel, uint8_t slave_address, uint8_t byte) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];
    EUSCI_B_I2C_setSlaveAddress(base_address, slave_address);
    EUSCI_B_I2C_masterSendSingleByte(base_address, byte);

    // what's happening here?
    /*if (read_byte) {
        for (int i = 0; i < 32; ++i) {
            P4OUT ^= 1 << 6;
            __delay_cycles(800000UL);
        }
        read_byte = 0;
    }*/

    return I2C_NO_ERROR;
}

i2c_error_t i2c_read_byte(i2c_t * channel, uint8_t slave_address, uint8_t * byte) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];
    EUSCI_B_I2C_setSlaveAddress(base_address, slave_address);
    byte = EUSCI_B_I2C_masterReceiveSingleByte(base_address);

    // what's happening here?
    /*if (read_byte) {
        for (int i = 0; i < 32; ++i) {
            P4OUT ^= 1 << 6;
            __delay_cycles(800000UL);
        }
        read_byte = 0;
    }*/

    return I2C_NO_ERROR;
}

i2c_error_t i2c_close(i2c_t * channel) {
    EUSCI_B_I2C_disable(BASE_ADDRESSES[channel->eusci]);
    return I2C_NO_ERROR;
}
