#ifndef _COMMON_IMU_H_
#define _COMMON_IMU_H_

#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Macro list for results of operations on the IMU
 */
#define IMU_RESULT_LIST(OP) \
    OP(NO_ERROR) \
    OP(SPI_ERROR)

/**
 * Enumeration of possible results for trying to communicate with the IMU
 */
typedef enum imu_result {
#   define STRING_OP(E) IMU_ ## E,
    IMU_RESULT_LIST(STRING_OP)
#   undef STRING_OP
    IMU_count
} imu_result_t;

#undef IMU_RESULT_LIST

typedef enum imu_gyro_fsr {
    IMU_GYRO_250DPS = (0 << 3),
    IMU_GYRO_500DPS = (1 << 3),
    IMU_GYRO_1000DPS = (2 << 3),
    IMU_GYRO_2000DPS = (3 << 3),
} imu_gyro_fsr_t;

typedef enum imu_accel_fsr {
    IMU_ACCEL_2G = (0 << 3),
    IMU_ACCEL_4G = (1 << 3),
    IMU_ACCEL_8G = (2 << 3),
    IMU_ACCEL_16G = (3 << 3),
} imu_accel_fsr_t;

typedef enum imu_lpf {
    IMU_FILTER_188HZ,
    IMU_FILTER_98HZ,
    IMU_FILTER_42HZ,
    IMU_FILTER_20HZ,
    IMU_FILTER_10HZ,
    IMU_FILTER_5HZ,
} imu_lpf_t;

/**
 * The connection to an IMU device
 */
typedef struct imu {
    /**
     * The owned SPI channel over which we talk
     */
    spi_t spi;
    volatile uint8_t *cs_value;
    uint8_t cs_bit;
} imu_t;

/**
 * Open a connection to an MPU-9250
 *
 * @param device The output device object
 * @param spi The SPI channel to use. Takes ownership of the lifecycle of this
 *      channel
 *
 * @return True if and only if the device connection succeeds
 */
bool imu_open(imu_t * device, spi_t * spi, volatile uint8_t *cs_value, uint8_t cs_bit);

/**
 * Close a connection to an IMU device
 *
 * @param device The device to close
 */
void imu_close(imu_t * device);

imu_result_t imu_read_reg(imu_t * device, uint8_t addr, uint8_t * value);

imu_result_t imu_write_reg(imu_t * device, uint8_t addr, uint8_t value);

imu_result_t imu_set_clock_source(imu_t * device, uint8_t source);

imu_result_t imu_set_gyro_fsr(imu_t * device, imu_gyro_fsr_t fsr);

imu_result_t imu_set_accel_fsr(imu_t * device, imu_accel_fsr_t fsr);

imu_result_t imu_set_lpf(imu_t * device, imu_lpf_t lpf);

#ifdef __cplusplus
}
#endif

#endif // _COMMON_IMU_H_
