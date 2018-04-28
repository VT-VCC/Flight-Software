ifndef _COMMON_EPS_H_
#define _COMMON_EPS_H_

#include "i2c.h"
#include "eps_internals.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	* Macro list for results of operations on the Lithium
	*/
#define EPS_RESULT_LIST(OP) \
    OP(NO_ERROR) \
    OP(BAD_COMMUNICATION)

	/**
	* Enumeration of possible results for trying to communicate with the Lithium
	*/
	typedef enum eps_result {
#   define STRING_OP(E) EPS_ ## E,
		EPS_RESULT_LIST(STRING_OP)
#   undef STRING_OP
		EPS_count
	} eps_result_t;


	/**
	* Represents a connection to a Lithium radio
	*/
	typedef struct eps {
		/**
		* The I2C channel over which we talk
		*/
		i2c_t i2c;
	} eps_t;

	/**
	* EPS write function.
	*
	* @param i2c channel that we read from
	* command byte
	* data parameter byte
	*
	* @return data value that is read from EPS 
	* if successful and returns EPS_BAD_COMMUNICATION
	* if unsuccessful
	*/
	eps_t eps_write(i2c_t * channel, uint8_t command, uint8_t data_parameter);

	/**
	* EPS read function.
	*
	* @param i2c channel that we read from
	*
	* @return data value that is read from EPS
	* if successful and returns EPS_BAD_COMMUNICATION
	* if unsuccessful
	*/
	eps_t eps_read(i2c_t * channel);



#ifdef __cplusplus
}
#endif

#endif // _COMMON_LITHIUM_H_