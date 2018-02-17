// eps.c
// Author: Haroun Ansari
#include "eps.h"
#include "eps_internal.h"

/******************************************************************************\
 *  Private support functions                                                 *
\******************************************************************************/
 eps_t eps_read(i2_t * channel, );
 eps_result_t eps_write(uint8_t * command, uint8_t * data_parameter);

 /******************************************************************************\
 *  Private support function implementations                                  *
 \******************************************************************************/

 eps_t eps_read(ep_t * channel, uint8_t * data[]) {
	 uint8_t slave_address = 0x2B;
	 
	 //Check if byte matches slave
	 i2c_error_t readErr1 i2c_read_byte(channel, value);
	 if (value != slave_address || readErr1 != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }

	 //get data[0]
	 i2c_error_t readErr2 i2c_read_byte(channel, value);
	 if (readErr2 != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }
	 data[0] = value;

	 //get data[1]
	 i2c_error_t readErr3 i2c_read_byte(channel, value);
	 if (readErr3 != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }
	 data[1] = value;
	 
	 //not sure what I should return for data array since it stores 2 bytes.
	 return EPS_NO_ERROR; 
 }

 eps_result_t eps_write(ep_t * channel, uint8_t * command, uint8_t * data_parameter) {
	 uint8_t slave_address = 0x2B;

	 // Write Slave Address via I2C
	 i2c_error_t writeErr1 = i2c_write_byte(channel, slave_address);
	 if (err1 != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }

	 // Write Command via I2C
	 i2c_error_t writeErr2 = i2c_write_byte(channel, command);
	 if (err2 != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }

	 // Write Command via I2C
	 i2c_error_t writeErr3 = i2c_write_byte(channel, data_parameter);
	 if (err2 != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }

	 //Read Response Bit 
	 i2c_error_t readErr1 = i2c_read_byte(channel, value);
	 if (value == 0xFFFF || readErr1 != I2C_NO_ERROR) {
		 return EPS_BAD_COMMUNICATION;
	 }
	
	 return EPS_NO_ERROR;
 }


