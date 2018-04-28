#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <string.h>
#include <assert.h>
#include "i2c.h"

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

// Semaphore for locking a task while transmitting/receiving
SemaphoreHandle_t i2c_semaphore[EUSCI_count];
StaticSemaphore_t i2c_semaphore_buffer[EUSCI_count];

static uint8_t *i2c_send_buffer[EUSCI_count];
static uint8_t *i2c_recv_buffer[EUSCI_count];
static size_t i2c_buffer_index[EUSCI_count];
static size_t i2c_buffer_size[EUSCI_count];

#ifdef EUSCI_B2_BASE
__attribute__((interrupt(USCI_B2_VECTOR)))
void USCI_B2_ISR(void) {
    switch (__even_in_range(UCB2IV, USCI_I2C_UCBIT9IFG)) {
    case USCI_I2C_UCNACKIFG: { // EUSCI_B_I2C_NAK_INTERRUPT
        //TODO allow configuration of NACK response
        // Try to re-send the packet if it was NACKed
        EUSCI_B_I2C_masterSendStart(EUSCI_B2_BASE);
        break;
    }

    case USCI_I2C_UCSTPIFG: { // EUSCI_B_I2C_STOP_INTERRUPT
        BaseType_t higher_task_taken;
        xSemaphoreGiveFromISR(i2c_semaphore[EUSCI_B2], &higher_task_taken);
        portYIELD_FROM_ISR(higher_task_taken);
        break;
    }

    case USCI_I2C_UCRXIFG0: { // EUSCI_B_I2C_RECEIVE_INTERRUPT0
        // Read a single byte
        size_t index = i2c_buffer_index[EUSCI_B2]++,
            size = i2c_buffer_size[EUSCI_B2];

        if (index == size - 1) {
            i2c_recv_buffer[EUSCI_B2][index] = EUSCI_B_I2C_masterReceiveMultiByteFinish(EUSCI_B2_BASE);
        } else if (index < size) {
            i2c_recv_buffer[EUSCI_B2][index] = EUSCI_B_I2C_masterReceiveMultiByteNext(EUSCI_B2_BASE);
        }
        break;
    }

    case USCI_I2C_UCTXIFG0: { // EUSCI_B_I2C_TRANSMIT_INTERRUPT0
        size_t index = i2c_buffer_index[EUSCI_B2]++,
            size = i2c_buffer_size[EUSCI_B2];

        if (index == size - 1) {
            EUSCI_B_I2C_masterSendMultiByteFinish(EUSCI_B2_BASE, i2c_send_buffer[EUSCI_B2][index]);
        } else if (index < size) {
            EUSCI_B_I2C_masterSendMultiByteNext(EUSCI_B2_BASE, i2c_send_buffer[EUSCI_B2][index]);
        }
        break;
    }
    }
}
#endif

bool i2c_open(eusci_t eusci, i2c_data_rate_t data_rate, i2c_t * out) {
    uint16_t base_address = BASE_ADDRESSES[eusci];
    assert(base_address == EUSCI_B0_BASE);

    EUSCI_B_I2C_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = CS_getSMCLK();
    param.dataRate = data_rate;
    param.byteCounterThreshold = 0;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;

    // Initialize and enable the master
    EUSCI_B_I2C_initMaster(base_address, &param);
    EUSCI_B_I2C_enable(base_address);

    // Clear and enable interrupts
    EUSCI_B_I2C_clearInterrupt(base_address,
        EUSCI_B_I2C_RECEIVE_INTERRUPT0 | EUSCI_B_I2C_TRANSMIT_INTERRUPT0 |
        EUSCI_B_I2C_NAK_INTERRUPT | EUSCI_B_I2C_STOP_INTERRUPT);
    EUSCI_B_I2C_enableInterrupt(base_address,
        EUSCI_B_I2C_RECEIVE_INTERRUPT0 | EUSCI_B_I2C_TRANSMIT_INTERRUPT0 |
        EUSCI_B_I2C_NAK_INTERRUPT | EUSCI_B_I2C_STOP_INTERRUPT);

    out->eusci = eusci;

    i2c_semaphore[eusci] = xSemaphoreCreateBinaryStatic(&i2c_semaphore_buffer[eusci]);

    return true;
}

i2c_error_t i2c_write_byte(i2c_t * channel, uint8_t address, uint8_t byte) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];

    EUSCI_B_I2C_setSlaveAddress(base_address, address);
    EUSCI_B_I2C_setMode(base_address, EUSCI_B_I2C_TRANSMIT_MODE);

    EUSCI_B_I2C_masterSendSingleByte(base_address, 0);

    // Block until completed
    while (EUSCI_B_I2C_isBusBusy(base_address));

    //TODO check for ACK/NACK

    return I2C_NO_ERROR;
}

i2c_error_t i2c_write_bytes(i2c_t * channel, uint8_t address, uint8_t * bytes, size_t n) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];

    EUSCI_B_I2C_setSlaveAddress(base_address, address);
    EUSCI_B_I2C_setMode(base_address, EUSCI_B_I2C_TRANSMIT_MODE);

    taskENTER_CRITICAL();
    i2c_buffer_index[channel->eusci] = 1;
    i2c_buffer_size[channel->eusci] = n;
    i2c_send_buffer[channel->eusci] = bytes;
    taskEXIT_CRITICAL();

    // Start by sending first byte
    EUSCI_B_I2C_masterSendMultiByteStart(base_address, bytes[0]);

    // Wait for STOP
    if (xSemaphoreTake(i2c_semaphore[channel->eusci], 50) == pdTRUE) {
        return I2C_NO_ERROR;
    }

    // The timeout was hit and our transfer was never completed
    return I2C_INCOMPLETE;
}

i2c_error_t i2c_read_byte(i2c_t * channel, uint8_t address, uint8_t * byte) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];

    EUSCI_B_I2C_setSlaveAddress(base_address, address);
    EUSCI_B_I2C_setMode(base_address, EUSCI_B_I2C_RECEIVE_MODE);

    taskENTER_CRITICAL();
    i2c_buffer_index[channel->eusci] = 0;
    i2c_buffer_size[channel->eusci] = 0;
    taskEXIT_CRITICAL();

    // Read for a single byte
    *byte = EUSCI_B_I2C_masterReceiveSingleByte(base_address);

    // Block until completed
    while (EUSCI_B_I2C_isBusBusy(base_address));

    //TODO check for ACK/NACK

    return I2C_NO_ERROR;
}

i2c_error_t i2c_read_bytes(i2c_t * channel, uint8_t address, uint8_t * bytes, size_t n) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];

    EUSCI_B_I2C_setSlaveAddress(base_address, address);
    EUSCI_B_I2C_setMode(base_address, EUSCI_B_I2C_RECEIVE_MODE);

    taskENTER_CRITICAL();
    i2c_buffer_index[channel->eusci] = 0;
    i2c_buffer_size[channel->eusci] = n;
    i2c_recv_buffer[channel->eusci] = bytes;
    taskEXIT_CRITICAL();

    // Start reading
    EUSCI_B_I2C_masterReceiveStart(base_address);

    // Wait for STOP
    if (xSemaphoreTake(i2c_semaphore[channel->eusci], 50) == pdTRUE) {
        return I2C_NO_ERROR;
    }

    // The timeout was hit and our transfer was never completed
    return I2C_INCOMPLETE;
}
