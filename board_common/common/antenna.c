#include "antenna.h"

#define SLAVE_ADDRESS 0x33

bool antenna_init(antenna_t * device, i2c_t * i2c) {
    device->i2c = i2c;

    return true;
}

antenna_error_t antenna_read_state(antenna_t * device, antenna_state_t * state) {
    i2c_error_t err;

    uint8_t data[3];
    err = i2c_read_bytes(device->i2c, SLAVE_ADDRESS, data, 3);
    if (err != I2C_NO_ERROR) {
        return ANTENNA_COMMUNICATION_ERROR;
    }

    state->door_open[0] = (data[0] & 0x80);
    state->door_open[1] = (data[0] & 0x40);
    state->door_open[2] = (data[0] & 0x20);
    state->door_open[3] = (data[0] & 0x10);

    state->mode = (data[0] & 0x03);

    state->main_heater_engaged[0] = (data[1] & 0x80);
    state->main_heater_engaged[1] = (data[1] & 0x40);
    state->main_heater_engaged[2] = (data[1] & 0x20);
    state->main_heater_engaged[3] = (data[1] & 0x10);

    state->backup_heater_engaged[0] = (data[1] & 0x08);
    state->backup_heater_engaged[1] = (data[1] & 0x04);
    state->backup_heater_engaged[2] = (data[1] & 0x02);
    state->backup_heater_engaged[3] = (data[1] & 0x01);

    state->elapsed_time = data[2];

    return ANTENNA_NO_ERROR;
}

antenna_error_t antenna_write_state(antenna_t * device, antenna_mode_t mode, bool heater_engaged[4]) {
    uint8_t command = 0;

    command |= (heater_engaged[0] << 3);
    command |= (heater_engaged[0] << 2);
    command |= (heater_engaged[0] << 1);
    command |= (heater_engaged[0] << 0);
    
    command |= ((mode & 0x03) << 4);
    
    i2c_error_t err = i2c_write_byte(device->i2c, SLAVE_ADDRESS, command);
    if (err != I2C_NO_ERROR) {
        return ANTENNA_COMMUNICATION_ERROR;
    }

    return ANTENNA_NO_ERROR;
}
