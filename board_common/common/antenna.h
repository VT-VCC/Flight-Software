#ifndef _BOARD_COMMON_ANTENNA_H_
#define _BOARD_COMMON_ANTENNA_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************\
 *  Macro list for the ways writing to an antenna can fail                        *
\******************************************************************************/
/// Macro for defining thing related to antenna errors
#define ANTENNA_ERROR_LIST(OP) \
    OP(NO_ERROR) \
    OP(COMMUNICATION_ERROR)

/// Enum representing possible error states for an antenna device
typedef enum antenna_error {
#   define ENUM_OP(E) ANTENNA_ ## E,
    ANTENNA_ERROR_LIST(ENUM_OP)
#   undef ENUM_OP
    antenna_count
} antenna_error_t;

/**
 * The connection to an I2C device
 */
typedef struct antenna {
    /**
     * The aliased I2C channel over which we talk
     */
    i2c_t * i2c;
} antenna_t;

typedef enum antenna_mode {
    ANTENNA_ALL_OFF = 0,
    ANTENNA_ALGORITHM_1 = 1,
    ANTENNA_ALGORITHM_2 = 2,
    ANTENNA_SLEEP = 3
} antenna_mode_t;

typedef struct antenna_state {
    // Are doors D1-D4 open
    bool door_open[4];
    // Current antenna state
    antenna_mode_t mode;
    // Are main heaters A1-A4 engaged
    bool main_heater_engaged[4];
    // Are backup heaters B1-B4 engaged
    bool backup_heater_engaged[4];
    // Elapsed running time in seconds
    uint8_t elapsed_time;
} antenna_state_t;

bool antenna_init(antenna_t * device, i2c_t * i2c);

antenna_error_t antenna_read_state(antenna_t * device, antenna_state_t * state);

antenna_error_t antenna_write_state(antenna_t * device, antenna_mode_t mode, bool heater_engaged[4]);

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_ANTENNA_H_
