#include "i2c.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <string.h>
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

// Semaphore for locking a task while transmitting/receiving
SemaphoreHandle_t i2c_semaphore[USCI_count];
StaticSemaphore_t i2c_semaphore_buffer[USCI_count];

static uint8_t *i2c_send_buffer[USCI_count];
static uint8_t *i2c_recv_buffer[USCI_count];
static size_t i2c_buffer_index[USCI_count];
static size_t i2c_buffer_size[USCI_count];

#ifdef USCI_B2_BASE
__attribute__((interrupt(USCI_B2_VECTOR)))
void USCI_B2_ISR(void) {
    switch (__even_in_range(UCB2IV, USCI_I2C_UCBIT9IFG)) {
    case USCI_I2C_UCNACKIFG: { // USCI_B_I2C_NAK_INTERRUPT
        //TODO allow configuration of NACK response
        // Try to re-send the packet if it was NACKed
        USCI_B_I2C_masterSendStart(USCI_B2_BASE);
        break;
    }

    case USCI_I2C_UCSTPIFG: { // USCI_B_I2C_STOP_INTERRUPT
        BaseType_t higher_task_taken;
        xSemaphoreGiveFromISR(i2c_semaphore[USCI_B2], &higher_task_taken);
        portYIELD_FROM_ISR(higher_task_taken);
        break;
    }

    case USCI_I2C_UCRXIFG: { // USCI_B_I2C_RECEIVE_INTERRUPT
        // Read a single byte
        size_t index = i2c_buffer_index[USCI_B2]++,
            size = i2c_buffer_size[USCI_B2];

        if (index == size - 1) {
            i2c_recv_buffer[USCI_B2][index] = USCI_B_I2C_masterReceiveMultiByteFinish(USCI_B2_BASE);
        } else if (index < size) {
            i2c_recv_buffer[USCI_B2][index] = USCI_B_I2C_masterReceiveMultiByteNext(USCI_B2_BASE);
        }
        break;
    }

    case USCI_I2C_UCTXIFG: { // USCI_B_I2C_TRANSMIT_INTERRUPT
        size_t index = i2c_buffer_index[USCI_B2]++,
            size = i2c_buffer_size[USCI_B2];

        if (index == size - 1) {
            USCI_B_I2C_masterSendMultiByteFinish(USCI_B2_BASE, i2c_send_buffer[USCI_B2][index]);
        } else if (index < size) {
            USCI_B_I2C_masterSendMultiByteNext(USCI_B2_BASE, i2c_send_buffer[USCI_B2][index]);
        }
        break;
    }
    }
}
#endif

bool i2c_open(usci_t usci, i2c_data_rate_t data_rate, i2c_t * out) {
    uint16_t base_address = BASE_ADDRESSES[usci];
    assert(base_address == USCI_B0_BASE || base_address == USCI_B1_BASE ||
        base_address == USCI_B2_BASE || base_address == USCI_B3_BASE);

    USCI_B_I2C_initMasterParam param = {0};
    param.selectClockSource = USCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = UCS_getSMCLK();
    param.dataRate = data_rate;

    // Initialize and enable the master
    USCI_B_I2C_initMaster(base_address, &param);
    USCI_B_I2C_enable(base_address);

    // Clear and enable interrupts
    USCI_B_I2C_clearInterrupt(base_address,
        USCI_B_I2C_RECEIVE_INTERRUPT | USCI_B_I2C_TRANSMIT_INTERRUPT  |
        USCI_B_I2C_NAK_INTERRUPT | USCI_B_I2C_STOP_INTERRUPT);
    USCI_B_I2C_enableInterrupt(base_address,
        USCI_B_I2C_RECEIVE_INTERRUPT | USCI_B_I2C_TRANSMIT_INTERRUPT |
        USCI_B_I2C_NAK_INTERRUPT | USCI_B_I2C_STOP_INTERRUPT);

    out->usci = usci;

    i2c_semaphore[usci] = xSemaphoreCreateBinaryStatic(&i2c_semaphore_buffer[usci]);

    return true;
}

i2c_error_t i2c_write_byte(i2c_t * channel, uint8_t address, uint8_t byte) {
    uint16_t base_address = BASE_ADDRESSES[channel->usci];

    USCI_B_I2C_setSlaveAddress(base_address, address);
    USCI_B_I2C_setMode(base_address, USCI_B_I2C_TRANSMIT_MODE);

    USCI_B_I2C_masterSendSingleByte(base_address, 0);

    // Block until completed
    while (USCI_B_I2C_isBusBusy(base_address));

    //TODO check for ACK/NACK

    return I2C_NO_ERROR;
}

i2c_error_t i2c_write_bytes(i2c_t * channel, uint8_t address, uint8_t * bytes, size_t n) {
    uint16_t base_address = BASE_ADDRESSES[channel->usci];

    USCI_B_I2C_setSlaveAddress(base_address, address);
    USCI_B_I2C_setMode(base_address, USCI_B_I2C_TRANSMIT_MODE);

    taskENTER_CRITICAL();
    i2c_buffer_index[channel->usci] = 1;
    i2c_buffer_size[channel->usci] = n;
    i2c_send_buffer[channel->usci] = bytes;
    taskEXIT_CRITICAL();

    // Start by sending first byte
    USCI_B_I2C_masterSendMultiByteStart(base_address, bytes[0]);

    // Wait for STOP
    if (xSemaphoreTake(i2c_semaphore[channel->usci], 50) == pdTRUE) {
        return I2C_NO_ERROR;
    }

    // The timeout was hit and our transfer was never completed
    return I2C_INCOMPLETE;
}

i2c_error_t i2c_read_byte(i2c_t * channel, uint8_t address, uint8_t * byte) {
    uint16_t base_address = BASE_ADDRESSES[channel->usci];

    USCI_B_I2C_setSlaveAddress(base_address, address);
    USCI_B_I2C_setMode(base_address, USCI_B_I2C_RECEIVE_MODE);

    taskENTER_CRITICAL();
    i2c_buffer_index[channel->usci] = 0;
    i2c_buffer_size[channel->usci] = 0;
    taskEXIT_CRITICAL();

    // Read for a single byte
    *byte = USCI_B_I2C_masterReceiveSingle(base_address);

    // Block until completed
    while (USCI_B_I2C_isBusBusy(base_address));

    //TODO check for ACK/NACK

    return I2C_NO_ERROR;
}

i2c_error_t i2c_read_bytes(i2c_t * channel, uint8_t address, uint8_t * bytes, size_t n) {
    uint16_t base_address = BASE_ADDRESSES[channel->usci];

    USCI_B_I2C_setSlaveAddress(base_address, address);
    USCI_B_I2C_setMode(base_address, USCI_B_I2C_RECEIVE_MODE);

    taskENTER_CRITICAL();
    i2c_buffer_index[channel->usci] = 0;
    i2c_buffer_size[channel->usci] = n;
    i2c_recv_buffer[channel->usci] = bytes;
    taskEXIT_CRITICAL();

    // Start reading
    USCI_B_I2C_masterReceiveMultiByteStart(base_address);

    // Wait for STOP
    if (xSemaphoreTake(i2c_semaphore[channel->usci], 50) == pdTRUE) {
        return I2C_NO_ERROR;
    }

    // The timeout was hit and our transfer was never completed
    return I2C_INCOMPLETE;
}
