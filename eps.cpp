// Tests the EPS

#include <catch/catch.hpp>
#include "eps.h"

TEST_CASE("Test eps implementation can read bytes", "[eps]") {
    i2c_t t;
    uint8_t test_data[3] =  {0x2B, 0xFF, 0x11 }; // slave, MSB, LSB

    //open channel for writing
    i2c_open(&t);

    //Using i2c write to channel
    REQUIRE(i2c_write_bytes(&t, test_data, 3) == I2C_NO_ERROR);

    //close channel
    i2c_close(&t);

    //read channel
    REQUIRE(eps_read(&t) == 0xFF11);

    
}

TEST_CASE("Test eps implementation can write bytes", "[eps]") {
    i2c_t t;
    uint8_t command = BOARD_STATUS;
    uint8_t data_parameter = 0x00;

    //Check if writing to channel is successful *returns (data_parameter << 8) | command;*
    REQUIRE(eps_write(&t, command, data_parameter) == 0x0001);

}

TEST_CASE("Test eps implementation can write AND read bytes", "[eps]") {
    i2c_t t;
    uint8_t command = BOARD_STATUS;
    uint8_t data_parameter = 0x00;

    //Write to Channel
    REQUIRE(eps_write(&t, command, data_parameter) == 0x0001);

    //Read from Channel
    REQUIRE(eps_read(&t) == 0x0001);
}