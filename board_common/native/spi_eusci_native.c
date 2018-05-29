#include "spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <string.h>
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

// Semaphore for locking a task while transmitting
StaticSemaphore_t spi_semaphore_buffer[EUSCI_count];
SemaphoreHandle_t spi_semaphore[EUSCI_count];

static uint8_t *spi_send_buffer[EUSCI_count];
static uint8_t *spi_recv_buffer[EUSCI_count];
static size_t spi_buffer_index[EUSCI_count];
static size_t spi_buffer_size[EUSCI_count];

#ifdef EUSCI_A3_BASE
__attribute__((interrupt(USCI_A3_VECTOR)))
void USCI_A3_ISR(void) {
    switch (__even_in_range(UCA3IV, 4)) {
        case USCI_SPI_UCRXIFG:
            while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_A3_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT));

            bool can_recv = spi_recv_buffer[EUSCI_A3] != 0,
                can_send = spi_send_buffer[EUSCI_A3] != 0;

            // Read the current byte
            if (can_recv) {
                spi_recv_buffer[EUSCI_A3][spi_buffer_index[EUSCI_A3]] = EUSCI_A_SPI_receiveData(EUSCI_A3_BASE);
            }

            spi_buffer_index[EUSCI_A3]++;
            if (spi_buffer_index[EUSCI_A3] < spi_buffer_size[EUSCI_A3]) {
                // Send the next byte
                EUSCI_A_SPI_transmitData(EUSCI_A3_BASE, (can_send ? spi_send_buffer[EUSCI_A3][spi_buffer_index[EUSCI_A3]] : 0));
            }
            else {
                // Unblock the transfer
                BaseType_t higher_task_taken;
                xSemaphoreGiveFromISR(spi_semaphore[EUSCI_A3], &higher_task_taken);
                portYIELD_FROM_ISR(higher_task_taken);
            }
            break;
    }
}
#endif

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

static bool eusci_a_spi_open(eusci_t eusci, uint16_t base_address, uint32_t clock_rate, spi_t * out) {
    // Check if the SPI bus is already enabled
    bool is_in_reset_state = HWREG16(base_address + OFS_UCAxCTLW0) & UCSWRST;
    if (!is_in_reset_state) {
        return false;
    }

    // Configure the SPI master block
    EUSCI_A_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_A_SPI_CLOCKSOURCE_ACLK;
    param.clockSourceFrequency = CS_getACLK();
    param.desiredSpiClock = clock_rate;
    param.msbFirst = EUSCI_A_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW;
    param.spiMode = EUSCI_A_SPI_3PIN;

    // Initialize the SPI master block
    EUSCI_A_SPI_initMaster(base_address, &param);

    // Enable the SPI block
    EUSCI_A_SPI_enable(base_address);

    // Clear and enable the RX interrupt
    EUSCI_A_SPI_clearInterrupt(base_address, EUSCI_A_SPI_RECEIVE_INTERRUPT);
    EUSCI_A_SPI_enableInterrupt(base_address, EUSCI_A_SPI_RECEIVE_INTERRUPT);

    out->eusci = eusci;

    spi_semaphore[eusci] = xSemaphoreCreateBinaryStatic(&spi_semaphore_buffer[eusci]);

    return true;
}

static bool eusci_b_spi_open(eusci_t eusci, uint16_t base_address, uint32_t clock_rate, spi_t * out) {
    // Check if the SPI bus is already enabled
    bool is_in_reset_state = HWREG16(base_address + OFS_UCBxCTLW0) & UCSWRST;
    if (!is_in_reset_state) {
        return false;
    }

    // Configure the SPI master block
    EUSCI_B_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_ACLK;
    param.clockSourceFrequency = CS_getACLK();
    param.desiredSpiClock = clock_rate;
    param.msbFirst = EUSCI_B_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW;
    param.spiMode = EUSCI_B_SPI_3PIN;

    // Initialize the SPI master block
    EUSCI_B_SPI_initMaster(base_address, &param);

    // Enable the SPI block
    EUSCI_B_SPI_enable(base_address);

    // Clear and enable the RX interrupt
    EUSCI_B_SPI_clearInterrupt(base_address, EUSCI_B_SPI_RECEIVE_INTERRUPT);
    EUSCI_B_SPI_enableInterrupt(base_address, EUSCI_B_SPI_RECEIVE_INTERRUPT);

    out->eusci = eusci;

    spi_semaphore[eusci] = xSemaphoreCreateBinaryStatic(&spi_semaphore_buffer[eusci]);

    return true;
}

bool spi_open(eusci_t eusci, uint32_t clock_rate, spi_t * out) {
    uint16_t base_address = BASE_ADDRESSES[eusci];

    if (is_eusci_a_block(base_address)) {
        return eusci_a_spi_open(eusci, base_address, clock_rate, out);
    }
    else {
        return eusci_b_spi_open(eusci, base_address, clock_rate, out);
    }
}

static void eusci_a_spi_close(uint16_t base_address) {
    // Disable the SPI block
    EUSCI_A_SPI_disable(base_address);
}

static void eusci_b_spi_close(uint16_t base_address) {
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

static spi_error_t eusci_a_spi_transfer_bytes(eusci_t eusci, uint16_t base_address, uint8_t * send_bytes, uint8_t * receive_bytes, size_t length) {
    // Check if the SPI bus is not enabled
    bool is_in_reset_state = HWREG16(base_address + OFS_UCAxCTLW0) & UCSWRST;
    if (is_in_reset_state) {
        return SPI_CHANNEL_CLOSED;
    }

    taskENTER_CRITICAL();
    spi_buffer_index[eusci] = 0;
    spi_buffer_size[eusci] = length;
    spi_send_buffer[eusci] = send_bytes;
    spi_recv_buffer[eusci] = receive_bytes;
    taskEXIT_CRITICAL();

    // Wait for the TX buffer to be ready, and by extension the RX buffer
    // (transmitting while UCxxIFG & UCTXIFG == 0 is undefined behavior)
    while(!EUSCI_A_SPI_getInterruptStatus(base_address, EUSCI_A_SPI_TRANSMIT_INTERRUPT));

    EUSCI_A_SPI_transmitData(base_address, (send_bytes == 0 ? 0 : send_bytes[0]));

    // Block until the ISR is fired
    if (xSemaphoreTake(spi_semaphore[eusci], 5) == pdTRUE) {
        memcpy(receive_bytes, spi_recv_buffer[eusci], length);
        return SPI_NO_ERROR;
    }
    else {
        // The timeout was hit and our transfer was never completed
        return SPI_INCOMPLETE;
    }
}

static spi_error_t eusci_b_spi_transfer_bytes(eusci_t eusci, uint16_t base_address, uint8_t * send_bytes, uint8_t * receive_bytes, size_t length) {
    // Check if the SPI bus is not enabled
    bool is_in_reset_state = HWREG16(base_address + OFS_UCBxCTLW0) & UCSWRST;
    if (is_in_reset_state) {
        return SPI_CHANNEL_CLOSED;
    }

    taskENTER_CRITICAL();
    spi_buffer_index[eusci] = 0;
    spi_buffer_size[eusci] = length;
    spi_send_buffer[eusci] = send_bytes;
    spi_recv_buffer[eusci] = receive_bytes;
    taskEXIT_CRITICAL();

    // Wait for the TX buffer to be ready, and by extension the RX buffer
    // (transmitting while UCxxIFG & UCTXIFG == 0 is undefined behavior)
    while(!EUSCI_B_SPI_getInterruptStatus(base_address, EUSCI_B_SPI_TRANSMIT_INTERRUPT));

    EUSCI_B_SPI_transmitData(base_address, (send_bytes == 0 ? 0 : send_bytes[0]));

    // Block until the ISR is fired
    if (xSemaphoreTake(spi_semaphore[eusci], 5) == pdTRUE) {
        memcpy(receive_bytes, spi_recv_buffer[eusci], length);
        return SPI_NO_ERROR;
    }
    else {
        // The timeout was hit and our transfer was never completed
        return SPI_INCOMPLETE;
    }
}

spi_error_t spi_transfer_bytes(spi_t * channel, uint8_t * send_bytes, uint8_t * receive_bytes, size_t length) {
    uint16_t base_address = BASE_ADDRESSES[channel->eusci];

    if (is_eusci_a_block(base_address)) {
        return eusci_a_spi_transfer_bytes(channel->eusci, base_address, send_bytes, receive_bytes, length);
    }
    else {
        return eusci_b_spi_transfer_bytes(channel->eusci, base_address, send_bytes, receive_bytes, length);
    }
}
