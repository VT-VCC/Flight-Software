#include "rfm.h"
#include "rfm_internal.h"

bool rfm_open(rfm_t * radio, spi_t * spi) {
    radio->spi = *spi;
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
    *frequency = (uint32_t)(RF69_FSTEP * carrier_freq);

    return RFM_NO_ERROR;
}

rfm_result_t rfm_set_mode(rfm_t * radio, int mode) {
    return RFM_NO_ERROR;
}

rfm_result_t rfm_set_address(rfm_t * radio, int address) {
    return RFM_NO_ERROR;
}

rfm_result_t rfm_set_network_id(rfm_t * radio, int network_id) {
    return RFM_NO_ERROR;
}

rfm_result_t rfm_set_sync_word(rfm_t * radio, int sync_word) {
    return RFM_NO_ERROR;
}

rfm_result_t rfm_set_power_mode(rfm_t * radio, int power_mode) {
    return RFM_NO_ERROR;
}

rfm_result_t rfm_set_high_power(rfm_t * radio, bool status) {
    return RFM_NO_ERROR;
}
