#include "rfm.h"
#include "rfm_internal.h"

static bool chip_select(rfm_t * radio) {
    bool selected = *(radio->cs_value) & radio->cs_bit;
    // Set NSS low when in use
    *(radio->cs_value) &= ~(radio->cs_bit);
    return selected;
}

static void chip_unselect(rfm_t * radio) {
    // Set NSS high when not in use
    *(radio->cs_value) |= radio->cs_bit;
}

bool rfm_open(rfm_t * radio, spi_t * spi, volatile uint8_t *cs_value, uint8_t cs_bit) {
    radio->spi = *spi;

    radio->cs_value = cs_value;
    radio->cs_bit = cs_bit;
    chip_select(radio);

    //TODO initialize non-default registers

    // Enable overload current protection for the PA
    rfm_result_t res = rfm_write_reg(radio, REG_OCP, 0x1a);
    if (res != RFM_NO_ERROR) {
        return false;
    }

    uint8_t old_pa_level;
    res = rfm_read_reg(radio, REG_PALEVEL, &old_pa_level);
    if (res != RFM_NO_ERROR) {
        return false;
    }

    // Discard old PA level state
    old_pa_level &= 0x1f;

    // Enable PA 1 and 2 for high power mode
    res = rfm_write_reg(radio, REG_PALEVEL, old_pa_level | 0x40 | 0x20);
    if (res != RFM_NO_ERROR) {
        return false;
    }

    chip_unselect(radio);

    return true;
}

void rfm_close(rfm_t * radio) {
    spi_close(&radio->spi);
}

rfm_result_t rfm_read_reg(rfm_t * radio, uint8_t addr, uint8_t * value) {
    bool selected = chip_select(radio);

    uint8_t send[2] = { addr & 0x7F }, recv[2];
    if (spi_transfer_bytes(&radio->spi, send, recv, 2) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    *value = recv[1];

    if (selected) {
        chip_unselect(radio);
    }

    return RFM_NO_ERROR;
}

rfm_result_t rfm_write_reg(rfm_t * radio, uint8_t addr, uint8_t value) {
    bool selected = chip_select(radio);

    uint8_t send[2] = { addr | 0x80, value };
    if (spi_send_bytes(&radio->spi, send, 2) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    if (selected) {
        chip_unselect(radio);
    }

    return RFM_NO_ERROR;
}

rfm_result_t rfm_read_fifo(rfm_t * radio, uint8_t * data, uint8_t size) {
    return RFM_NO_ERROR;
}

rfm_result_t rfm_write_fifo(rfm_t * radio, uint8_t * data, uint8_t size) {
    bool selected = chip_select(radio);

    // Start write to FIFO register
    if (spi_send_byte(&radio->spi, REG_FIFO | 0x80) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    //TODO is this necessary?
    if (spi_send_byte(&radio->spi, size) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    // Write data bytes to the FIFO
    for (uint16_t i = 0; i < size; ++i) {
        if (spi_send_byte(&radio->spi, data[i]) != SPI_NO_ERROR) {
            return RFM_SPI_ERROR;
        }
    }

    if (selected) {
        chip_unselect(radio);
    }

    return RFM_SPI_ERROR;
}

rfm_result_t rfm_set_frequency(rfm_t * radio, uint32_t frequency) {
    //TODO move out of TX

    frequency /= FSTEP;

    if (rfm_write_reg(radio, REG_FRFMSB, (uint8_t)(frequency >> 16)) != RFM_NO_ERROR) {
        return RFM_SPI_ERROR;
    }
    if (rfm_write_reg(radio, REG_FRFMID, (uint8_t)(frequency >> 8)) != RFM_NO_ERROR) {
        return RFM_SPI_ERROR;
    }
    if (rfm_write_reg(radio, REG_FRFLSB, (uint8_t)(frequency)) != RFM_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    return RFM_NO_ERROR;
}

rfm_result_t rfm_get_frequency(rfm_t * radio, uint32_t * frequency) {
    // Read individual bit components
    uint8_t freq_lsb, freq_mid, freq_msb;
    rfm_result_t res = rfm_read_reg(radio, REG_FRFLSB, &freq_lsb);
    if (res != RFM_NO_ERROR) {
        return res;
    }
    res = rfm_read_reg(radio, REG_FRFMID, &freq_mid);
    if (res != RFM_NO_ERROR) {
        return res;
    }
    res = rfm_read_reg(radio, REG_FRFMSB, &freq_msb);
    if (res != RFM_NO_ERROR) {
        return res;
    }

    // Concatenate bits and scale by frequency stepping
    uint32_t carrier_freq = ((uint32_t)freq_msb << 16) | ((uint32_t)freq_mid << 8) | (freq_lsb);
    *frequency = (uint32_t)(FSTEP * carrier_freq);

    return RFM_NO_ERROR;
}

rfm_result_t rfm_set_mode(rfm_t * radio, rfm_mode_t mode) {
    //TODO cache mode internally

    switch (mode) {
        case RFM_MODE_TX:
        case RFM_MODE_RX:
        case RFM_MODE_SLEEP:
        case RFM_MODE_STDBY:
        case RFM_MODE_FS: {
            uint8_t old_mode;
            rfm_result_t res = rfm_read_reg(radio, REG_OPMODE, &old_mode);
            if (res != RFM_NO_ERROR) {
                return res;
            }

            // Discard the existing mode bits
            old_mode &= 0xE3;

            // Change to the new mode
            res = rfm_write_reg(radio, REG_OPMODE, old_mode | mode);
            if (res != RFM_NO_ERROR) {
                return res;
            }

            if (mode == RFM_MODE_TX) {
                //TODO enable high power regs
            }
            else if (mode == RFM_MODE_RX) {
                //TODO disable high power regs
            }

            break;
        }

        default:
            return RFM_INVALID_ARG;
    }

    return RFM_NO_ERROR;
}

rfm_result_t rfm_set_address(rfm_t * radio, uint8_t address) {
    return rfm_write_reg(radio, REG_NODEADRS, address);
}

rfm_result_t rfm_set_sync_word(rfm_t * radio, uint8_t * sync_word, uint8_t length) {
    if (length > 8) {
        return RFM_INVALID_ARG;
    }

    uint8_t old_config;
    rfm_result_t res = rfm_read_reg(radio, REG_SYNCCONFIG, &old_config);
    if (res != RFM_NO_ERROR) {
        return res;
    }

    // Discard old sync word size bits
    old_config &= 0xc7;

    // Update sync word size
    res = rfm_write_reg(radio, REG_SYNCCONFIG, old_config | ((length - 1) << 3));

    // Set new sync word
    for (uint8_t i = 0; i < length && res == RFM_NO_ERROR; ++i) {
        res = rfm_write_reg(radio, REG_SYNCVALUE1 + i, sync_word[i]);
    }

    return res;
}

rfm_result_t rfm_set_power_level(rfm_t * radio, uint8_t level) {
    if (level > 31) {
        return RFM_INVALID_ARG;
    }

    //TODO figure out PA levels for high power mode

    return RFM_NO_ERROR;
}
