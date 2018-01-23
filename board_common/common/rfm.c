#include "rfm.h"
#include "rfm_internal.h"

#include <assert.h>



// Activate NSS (SPI chip select)
// This should only be used by read/write_reg and read/write_fifo as the
// slave expects NSS to switch between individual accesses (see t_nhigh)
static void chip_select(rfm_t * radio) {
    // Set NSS low when in use
    *(radio->cs_value) &= ~(radio->cs_bit);
}

static void chip_unselect(rfm_t * radio) {
    // Set NSS high when not in use
    *(radio->cs_value) |= radio->cs_bit;
}

bool rfm_open(rfm_t * radio, spi_t * spi, volatile uint8_t *cs_value, uint8_t cs_bit) {
    radio->spi = *spi;

    radio->mode = 0;

    radio->cs_value = cs_value;
    radio->cs_bit = cs_bit;
    chip_unselect(radio);

    //TODO reset chip to clear FIFO and restore default register values

    // These defaults assume that the chip is in a reset state
    struct { uint8_t addr; uint8_t value; } default_config[] = {
        { REG_DATAMODUL, DATAMODUL_DATAMODE_PACKET | DATAMODUL_MODULATIONTYPE_FSK | DATAMODUL_MODULATIONSHAPING_10 },
        { REG_BITRATEMSB, BITRATEMSB_9600 },
        { REG_BITRATELSB, BITRATELSB_9600 },
        { REG_FDEVMSB, RF_FDEVMSB_5000 },
        { REG_FDEVLSB, RF_FDEVLSB_5000 },
        { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_2 },
        { REG_DIOMAPPING2, RF_DIOMAPPING2_DIO4_01 | RF_DIOMAPPING2_CLKOUT_OFF }, 

    };

    // Load our register values
    for (int i = 0; i < sizeof(default_config)/sizeof(default_config[0]); ++i) {
        if (rfm_write_reg(radio, default_config[i].addr, default_config[i].value) != RFM_NO_ERROR) {
            return false;
        }
    }

    // Enable overload current protection for the power amplifier
    if (rfm_write_reg(radio, REG_OCP, 0x1a) != RFM_NO_ERROR) {
        return false;
    }

    // Read PA levels and clear their states
    uint8_t pa_level;
    if (rfm_read_reg(radio, REG_PALEVEL, &pa_level) != RFM_NO_ERROR) {
        return false;
    }
    pa_level &= 0x1f;

    // Enable PA 1 and 2 for high power mode
    pa_level |= PALEVEL_PA0_OFF | PALEVEL_PA1_ON | PALEVEL_PA2_ON;
    if (rfm_write_reg(radio, REG_PALEVEL, pa_level) != RFM_NO_ERROR) {
        return false;
    }

    // Move into standby mode
    if (rfm_set_mode(radio, RFM_MODE_STDBY) != RFM_NO_ERROR) {
        return false;
    }

    return true;
}

void rfm_close(rfm_t * radio) {
    spi_close(&radio->spi);
}

rfm_result_t rfm_read_reg(rfm_t * radio, uint8_t addr, uint8_t * value) {
    chip_select(radio);

    uint8_t send[2] = { addr & 0x7F }, recv[2];
    if (spi_transfer_bytes(&radio->spi, send, recv, 2) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }
    *value = recv[1];

    chip_unselect(radio);

    return RFM_NO_ERROR;
}

rfm_result_t rfm_write_reg(rfm_t * radio, uint8_t addr, uint8_t value) {
    chip_select(radio);

    uint8_t send[2] = { addr | 0x80, value };
    if (spi_send_bytes(&radio->spi, send, 2) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    chip_unselect(radio);

    return RFM_NO_ERROR;
}

rfm_result_t rfm_read_fifo(rfm_t * radio, uint8_t * data, uint8_t size) {
    chip_select(radio);

    // Start write to FIFO register
    if (spi_send_byte(&radio->spi, REG_FIFO & 0x7F) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    // Write data bytes to the FIFO
    if (spi_receive_bytes(&radio->spi, data, size) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    chip_unselect(radio);

    return RFM_NO_ERROR;
}

rfm_result_t rfm_write_fifo(rfm_t * radio, uint8_t * data, uint8_t size) {
    chip_select(radio);

    // Start write to FIFO register
    if (spi_send_byte(&radio->spi, REG_FIFO | 0x80) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    // Write data bytes to the FIFO
    if (spi_send_bytes(&radio->spi, data, size) != SPI_NO_ERROR) {
        return RFM_SPI_ERROR;
    }

    chip_unselect(radio);
    
    return RFM_NO_ERROR;
}

rfm_result_t rfm_set_frequency(rfm_t * radio, uint32_t frequency) {
    //TODO move out of TX

    frequency = (int)(frequency/FSTEP);

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
    assert(mode == RFM_MODE_SLEEP || mode == RFM_MODE_STDBY ||
        mode == RFM_MODE_FS || mode == RFM_MODE_TX ||
        mode == RFM_MODE_RX);

    if (mode == radio->mode) {
        return RFM_NO_ERROR;
    }

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

    // Move between +20 dBm mode for TX/RX modes
    if (mode == RFM_MODE_TX || mode == RFM_MODE_RX) {
        bool enable = (mode == RFM_MODE_TX);

        res = rfm_write_reg(radio, REG_TESTPA1, enable ? TESTPA1_20DBM : TESTPA1_NORMAL);
        if (res != RFM_NO_ERROR) {
            return res;
        }
        res = rfm_write_reg(radio, REG_TESTPA2, enable ? TESTPA2_20DBM : TESTPA2_NORMAL);
        if (res != RFM_NO_ERROR) {
            return res;
        }
    }

    // Block until ModeReady is raised
    // This should be immediate with the exception of TX mode
    uint8_t irq_flags = 0;
    while (radio->mode == RFM_MODE_SLEEP && !(irq_flags & IRQFLAGS1_MODEREADY)) {
        res = rfm_read_reg(radio, REG_IRQFLAGS1, &irq_flags);
        if (res != RFM_NO_ERROR) {
            return res;
        }
    }

    radio->mode = mode;

    return RFM_NO_ERROR;
}

rfm_result_t rfm_set_address(rfm_t * radio, uint8_t address) {
    return rfm_write_reg(radio, REG_NODEADRS, address);
}

rfm_result_t rfm_set_sync_word(rfm_t * radio, uint8_t * sync_word, uint8_t length) {
    assert(length < 8);

    // Read sync word config and discard size bits
    uint8_t sync_config;
    rfm_result_t res = rfm_read_reg(radio, REG_SYNCCONFIG, &sync_config);
    if (res != RFM_NO_ERROR) {
        return res;
    }
    sync_config &= 0xc7;

    // Update sync word size
    sync_config |= ((length - 1) << 3);
    res = rfm_write_reg(radio, REG_SYNCCONFIG, sync_config);
    if (res != RFM_NO_ERROR) {
        return res;
    }

    // Set new sync word
    for (uint8_t i = 0; i < length && res == RFM_NO_ERROR; ++i) {
        res = rfm_write_reg(radio, REG_SYNCVALUE1 + i, sync_word[i]);
        if (res != RFM_NO_ERROR) {
            return res;
        }
    }

    return res;
}

rfm_result_t rfm_set_power_level(rfm_t * radio, uint8_t level) {
    assert(level < 16);

    // Read PA config and discard level bits
    uint8_t pa_level;
    rfm_result_t res = rfm_read_reg(radio, REG_PALEVEL, &pa_level);
    if (res != SPI_NO_ERROR) {
        return res;
    }
    pa_level &= 0xe0;

    // Set new level
    pa_level |= level;
    return rfm_write_reg(radio, REG_PALEVEL, pa_level);
}
