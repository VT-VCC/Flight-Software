#include <assert.h>

#include "gpio.h"

#include "mmc.h"
#include "mmc_internal.h"

static void mmc_select(mmc_t * device) {
    // Set CS low when in use
    gpio_clear(device->cs_port, device->cs_pin);

    // Wait for the device to not be busy
    spi_error_t err;
    uint8_t value;
    do {
        err = spi_read_byte(device->spi, &value);
    } while (err ==SPI_NO_ERROR && value == 0);
}

static void mmc_release(mmc_t * device) {
    // Set CS high when not in use
    gpio_set(device->cs_port, device->cs_pin);

    // Clock for 8 cycles after releasing
    spi_write_byte(device->spi, 0xff);
}

static uint8_t mmc_send_cmd(mmc_t * device, uint8_t cmd, uint32_t arg) {
    uint8_t buf[6];
    buf[0] = cmd | 0x40;
    // Arguments in big-endian
    buf[1] = (arg >> 24) & 0xFF;
    buf[2] = (arg >> 16) & 0xFF;
    buf[3] = (arg >> 8) & 0xFF;
    buf[4] = arg & 0xFF;
    // CRC only matters for entering idle
    buf[5] = (cmd == MMC_GO_IDLE_STATE) ? 0x95 : 0xff;

    mmc_select(device);

    // Send the command
    spi_error_t err = spi_write_bytes(device->spi, buf, 6);
    if (err != SPI_NO_ERROR) {
        return 0;
    }

    // Wait for a response
    uint8_t ret = 0;
    do {
        err = spi_read_byte(device->spi, &ret);
    } while (err == SPI_NO_ERROR && ret & MMC_BUSY_MASK);

    return ret;
}

static bool wait_for_response(mmc_t * device, uint8_t token) {
    spi_error_t err;
    uint8_t value;

    int i = 0;
    do {
        err = spi_read_byte(device->spi, &value);
        ++i;
    } while (err == SPI_NO_ERROR && value != token && i < 1000);

    return i < 1000;
}

bool mmc_init(mmc_t * device, spi_t * spi, gpio_port_t cs_port, gpio_pin_t cs_pin) {
    device->spi = spi;

    device->cs_port = cs_port;
    device->cs_pin = cs_pin;
    mmc_release(device);

    // Send power up sequence
    spi_error_t err;
    int i = 0;
    do {
        err = spi_write_byte(device->spi, 0xff);
        ++i;
    } while (err == SPI_NO_ERROR && i < 9);

    // Reset and initialize chip
    return mmc_idle(device) == MMC_NO_ERROR;
}

/*
mmc_error_t mmc_read_register(mmc_t * device, uint8_t addr, uint8_t len, uint8_t * block) {

}
*/

mmc_error_t mmc_set_block_len(mmc_t * device, uint32_t len) {
    mmc_select(device);
    uint8_t value = mmc_send_cmd(device, MMC_SET_BLOCKLEN, len);
    mmc_release(device);

    if (value == MMC_SUCCESS) {
        return MMC_NO_ERROR;
    }
    return MMC_BAD_RESPONSE;
}

mmc_error_t mmc_idle(mmc_t * device) {
    mmc_select(device);

    // Reset card
    if (mmc_send_cmd(device, MMC_GO_IDLE_STATE, 0) != 1) {
        return MMC_BAD_RESPONSE;
    }

    // Wait for initialization
    while (mmc_send_cmd(device, MMC_SEND_OP_COND, 0) != 0);

    mmc_release(device);

    return MMC_NO_ERROR;
}

mmc_error_t mmc_read_block(mmc_t * device, uint32_t addr, uint32_t len, uint8_t * block) {
    mmc_error_t err = MMC_NO_ERROR;
    mmc_select(device);

    // Set the block length
    err = mmc_set_block_len(device, len);
    if (err != MMC_NO_ERROR) {
        goto cleanup;
    }

    // Request a single block
    mmc_send_cmd(device, MMC_READ_SINGLE_BLOCK, addr);

    // Wait for the data block start token
    if (!wait_for_response(device, MMC_START_DATA_BLOCK_TOKEN)) {
        err = MMC_BAD_RESPONSE;
        goto cleanup;
    }

    // Read in the data block
    if (spi_read_bytes(device->spi, block, len) != SPI_NO_ERROR) {
        err = MMC_BAD_RESPONSE;
        goto cleanup;
    }

    // Discard proceeding CRC bytes
    spi_write_byte(device->spi, 0xff);
    spi_write_byte(device->spi, 0xff);

cleanup:
    mmc_release(device);
    return err;
}

mmc_error_t mmc_write_block(mmc_t * device, uint32_t addr, uint32_t len, uint8_t * block) {
    mmc_error_t err = MMC_NO_ERROR;
    mmc_select(device);

    // Set the block length
    err = mmc_set_block_len(device, len);
    if (err != MMC_NO_ERROR) {
        goto cleanup;
    }

    // Request a chance to write
    if (mmc_send_cmd(device, MMC_WRITE_BLOCK, addr) != MMC_SUCCESS) {
        err = MMC_BAD_RESPONSE;
        goto cleanup;
    }

    // Send data block token
    spi_write_byte(device->spi, MMC_START_DATA_BLOCK_WRITE);

    // Write the data block
    if (spi_write_bytes(device->spi, block, len) != SPI_NO_ERROR) {
        err = MMC_BAD_RESPONSE;
        goto cleanup;
    }

    // Send dummy CRC bytes
    spi_write_byte(device->spi, 0xff);
    spi_write_byte(device->spi, 0xff);

cleanup:
    mmc_release(device);
    return err;
}

mmc_error_t mmc_read_sector(mmc_t * device, uint32_t index, uint8_t * sector) {
    return mmc_read_block(device, index * MMC_SECTOR_SIZE, MMC_SECTOR_SIZE, sector);
}

mmc_error_t mmc_write_sector(mmc_t * device, uint32_t index, uint8_t * sector) {
    return mmc_write_block(device, index * MMC_SECTOR_SIZE, MMC_SECTOR_SIZE, sector);
}
