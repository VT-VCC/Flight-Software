// eps.c
// Author: Haroun Ansari
#include "eps.h"
#include "eps_internal.h"

 /******************************************************************************\
 *  Private support function implementations                                  *
 \******************************************************************************/

 eps_t eps_read(i2c_t * channel) {
	 uint8_t slave_address = 0x2B;
	 uint8_t * data[1];
	 
	 //Check if byte matches slave
	 i2c_error_t err i2c_read_byte(channel, value);
	 if (value != slave_address || err != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }

	 //get data[1] the MSB
	 i2c_error_t err i2c_read_byte(channel, value);
	 if (err != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }
	 data[1] = value;

	 //get data[0] the LSB
	 i2c_error_t err i2c_read_byte(channel, value);
	 if (err != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }
	 data[0] = value;
	 
	 //returns 2 bytes that are read with first byte as MSB and 2nd byte as LSB
	 return (data[1] << 8) | data[0] ; 
 }

 eps_t eps_write(i2c_t * channel, uint8_t command, uint8_t data_parameter) {
	 uint8_t slave_address = 0x2B;

	 // open channel for i2c
	 i2c_open(channel);

	 // Write Slave Address via I2C
	 i2c_error_t err = i2c_write_byte(channel, slave_address);
	 if (err != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }

	 // Write Command via I2C
	 i2c_error_t err = i2c_write_byte(channel, command);
	 if (err != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }

	 // Write Command via I2C
	 i2c_error_t err = i2c_write_byte(channel, data_parameter);
	 if (err != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }

	 //Read Response Bit 
	 i2c_error_t err = i2c_read_byte(channel, value);
	 if (value == 0xFFFF || err != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }

	 //close channel for i2c
	 i2c_close(channel);
	
	 // return the data parameter as the MSB and the command as the LSB
	 return (data_parameter << 8) | command;
 }


