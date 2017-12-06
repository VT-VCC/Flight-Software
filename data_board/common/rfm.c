#include "rfm.h"

bool rfm_open(rfm_t * radio, spi_t * spi) {
    radio->spi = *spi;
    return true;
}

void rfm_close(rfm_t * radio) {
    spi_close(&radio->spi);
}

rfm_result_t rfm_read_reg(rfm_t * radio, uint8_t addr, uint8_t * value) {
    return RFM_NO_ERROR;
}

rfm_result_t rfm_write_reg(rfm_t * radio, uint8_t addr, uint8_t value) {
    return RFM_NO_ERROR;
}

rfm_result_t rfm_read_fifo(rfm_t * radio, uint8_t * data, uint16_t size) {
    return RFM_NO_ERROR;
}

rfm_result_t rfm_write_fifo(rfm_t * radio, uint8_t * data, uint16_t size) {
    return RFM_NO_ERROR;
}

rfm_result_t rfm_set_frequency(rfm_t * radio, int frequency) {
    return RFM_NO_ERROR;
}

rfm_result_t rfm_get_frequency(rfm_t * radio, int * frequency) {
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
