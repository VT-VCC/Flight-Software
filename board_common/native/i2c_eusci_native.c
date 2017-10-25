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

__attribute__((interrupt(USCI_A0_VECTOR)))
void USCI_A0_ISR(void) {}

__attribute__((interrupt(USCI_A1_VECTOR)))
void USCI_A1_ISR(void) {}

void i2c_open(eusci_t eusci, i2c_t * out) {

    USCI_B_I2C_initMasterParam param = {0};

    // Initializate parameters for the I2C

    uint16_t base_address = BASE_ADDRESSES;

    // Start the I2C in Master mode

    USCI_B_I2C_initMaster(base_address, &param);

    assert(false);
}

void i2c_write_string() {}

void i2c_read_string() {}

void i2c_write_bytes() {}

void i2c_read_bytes() {}
