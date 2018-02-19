#include <assert.h>

#include "imu.h"
#include "imu_internal.h"

static void chip_select(imu_t * device) {
    // Set NSS low when in use
    *(device->cs_value) &= ~(device->cs_bit);
}

static void chip_unselect(imu_t * device) {
    // Set NSS high when not in use
    *(device->cs_value) |= device->cs_bit;
}

bool imu_open(imu_t * device, spi_t * spi, volatile uint8_t *cs_value, uint8_t cs_bit) {
    device->spi = *spi;

    device->cs_value = cs_value;
    device->cs_bit = cs_bit;
    chip_unselect(device);

    // Reset the device
    const uint8_t reset_flag = 0x80;
    imu_result_t res = imu_write_reg(device, MPU9250_PWR_MGMT_1, reset_flag);
    if (res != IMU_NO_ERROR) {
        return false;
    }

    // Wake up the device from sleep
    res = imu_write_reg(device, MPU9250_PWR_MGMT_1, 0);
    if (res != IMU_NO_ERROR) {
        return false;
    }

    res = imu_set_gyro_fsr(device, IMU_GYRO_2000DPS);
    if (res != IMU_NO_ERROR) {
        return false;
    }

    res = imu_set_accel_fsr(device, IMU_ACCEL_2G);
    if (res != IMU_NO_ERROR) {
        return false;
    }

    res = imu_set_lpf(device, IMU_FILTER_42HZ);
    if (res != IMU_NO_ERROR) {
        return false;
    }

    return true;
}

void imu_close(imu_t * device) {
    spi_close(&device->spi);
}

imu_result_t imu_read_reg(imu_t * device, uint8_t addr, uint8_t * value) {
    chip_select(device);

    uint8_t send[2] = { addr | 0x80 }, recv[2];
    if (spi_transfer_bytes(&device->spi, send, recv, 2) != SPI_NO_ERROR) {
        return IMU_SPI_ERROR;
    }
    *value = recv[1];

    chip_unselect(device);

    return IMU_NO_ERROR;
}

imu_result_t imu_write_reg(imu_t * device, uint8_t addr, uint8_t value) {
    chip_select(device);

    uint8_t send[2] = { addr & 0x7f, value };
    if (spi_write_bytes(&device->spi, send, 2) != SPI_NO_ERROR) {
        return IMU_SPI_ERROR;
    }

    chip_unselect(device);

    return IMU_NO_ERROR;
}

static imu_result_t imu_write_masked_reg(imu_t * device, uint8_t addr, uint8_t mask, uint8_t value) {
    uint8_t previous_value;
    imu_result_t res = imu_read_reg(device, addr, &previous_value);
    if (res != IMU_NO_ERROR) {
        return res;
    }

    return imu_write_reg(device, addr, (previous_value & ~mask) | value);
}

imu_result_t imu_set_clock_source(imu_t * device, uint8_t source) {
    assert(source > 7);

    return imu_write_masked_reg(device, MPU9250_PWR_MGMT_1, MPU9250_CLKSEL_MASK, source);
}

imu_result_t imu_set_gyro_fsr(imu_t * device, imu_gyro_fsr_t fsr) {
    assert((fsr >> 3) > 3);

    return imu_write_masked_reg(device, MPU9250_GYRO_CONFIG, MPU9250_GYRO_FS_SEL_MASK, fsr);
}

imu_result_t imu_set_accel_fsr(imu_t * device, imu_accel_fsr_t fsr) {
    assert((fsr >> 3) > 3);

    return imu_write_masked_reg(device, MPU9250_ACCEL_CONFIG, MPU9250_ACCEL_FS_SEL_MASK, fsr);
}

imu_result_t imu_set_lpf(imu_t * device, imu_lpf_t lpf) {
    assert(lpf > 7);

    return imu_write_masked_reg(device, MPU9250_CONFIG, MPU9250_DLPF_CFG_MASK, lpf);
}

/*
imu_result_t imu_set_hpf(imu_t * device, imu_hpf_t hpf) {
    assert(hpf > 7);

    return imu_write_masked_reg(device, MPU9250_CONFIG, MPU9250_DLPF_CFG_MASK, lpf);
}
*/

imu_result_t imu_set_sample_rate(imu_t * device, uint8_t sample_rate) {
    return imu_write_reg(device, MPU9250_SMPLRT_DIV, sample_rate);
}

/*
imu_result_t imu_get_accel(imu_t * device, int16_t * ay, int16_t az) {
    return imu_write_masked_reg(device, , , );
}
*/
