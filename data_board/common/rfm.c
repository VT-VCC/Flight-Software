#include "rfm.h"
#include "rfm_internal.h"

bool rfm_open(rfm_t * radio, spi_t * spi) {
    radio->spi = *spi;

    //TODO initialize non-default registers

    // Enable overload current protection for the PA
    rfm_result_t res = rfm_write_reg(radio, REG_OCP, 0x1a);
    if (res != RFM_NO_ERROR) {
        return res;
    }

    uint8_t old_pa_level;
    res = rfm_read_reg(radio, REG_PALEVEL, &old_pa_level);
    if (res != RFM_NO_ERROR) {
        return res;
    }

    // Discard old PA level state
    old_pa_level &= 0x1f;

    // Enable PA 1 and 2 for high power mode
    res = rfm_write_reg(radio, REG_PALEVEL, old_pa_level | 0x40 | 0x20);
    if (res != RFM_NO_ERROR) {
        return res;
    }

    return true;
}

void rfm_close(rfm_t * radio) {
    spi_close(&radio->spi);
}

rfm_result_t rfm_read_reg(rfm_t * radio, uint8_t addr, uint8_t * value) {
    //TODO select and unselect slave

    if (spi_send_byte(&radio->spi, addr & 0x7F) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    if (spi_receive_byte(&radio->spi, value) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    return RFM_NO_ERROR;
}

rfm_result_t rfm_write_reg(rfm_t * radio, uint8_t addr, uint8_t value) {
    //TODO select and unselect slave

    if (spi_send_byte(&radio->spi, addr | 0x80) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    if (spi_send_byte(&radio->spi, value) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    return RFM_NO_ERROR;
}

rfm_result_t rfm_read_fifo(rfm_t * radio, uint8_t * data, uint8_t size) {
    return RFM_NO_ERROR;
}

rfm_result_t rfm_write_fifo(rfm_t * radio, uint8_t * data, uint8_t size) {
    //TODO select and unselect slave

    if (spi_send_byte(&radio->spi, REG_FIFO | 0x80) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    if (spi_send_byte(&radio->spi, size) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    for (uint16_t i = 0; i < size; i++) {
        if (spi_send_byte(&radio->spi, data[i]) != SPI_NO_ERROR) {
            return RFM_SPI_ERROR;
        }
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
            return RFM_NO_ERROR; //TODO put an actual error here
    }

    return RFM_NO_ERROR;
}

rfm_result_t rfm_set_address(rfm_t * radio, uint8_t address) {
    return rfm_write_reg(radio, REG_NODEADRS, address);
}

rfm_result_t rfm_set_sync_word(rfm_t * radio, uint8_t * sync_word, uint8_t length) {
    if (length > 8) {
        return RFM_NO_ERROR; //TODO put an actual error here
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
        return RFM_NO_ERROR; //TODO put an actual error here
    }

    //TODO figure out PA levels for high power mode

    return RFM_NO_ERROR;
}
