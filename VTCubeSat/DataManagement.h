/*
 * DataManagement.h
 *
 *  Created on: Mar 8, 2017
 *      Author: souldia
 */

#ifndef DATAMANAGEMENT_H_
#define DATAMANAGEMENT_H_

typedef struct data_handle {
	  uint16_t idx : 10;
	  uint8_t overwrite_counter : 4; // easy to pack
	  uint8_t observation_type : 3;
	  uint8_t valid : 1;             // 0 when invalid
	  uint8_t dirtyBit : 1;
} data_handle_t;

/**
 * enumeration for storing data to the memory
 */
typedef enum store_result {
	STORE_OK,
	STORE_NOT_OK,
	STORE_OVERWRITE_TRANSMITTED,
	STORE_OVERWRITE_UNTRANSMITTED
} store_result_t;

/**
 *  enumeration for retrieving data to the memory
 */
typedef enum retrieve_result {
	RETRIEVE_OK,
	RETRIEVE_INCORRECT_IDX,
	RETRIEVE_INVALID_IDX,
	RETRIEVE_INCORRECT_TYPE,
	RETRIEVE_NOT_OK
} retrieve_result_t;

typedef enum flush_result {
	FLUSH_SUCCES = 0,
	FLUSH_UNSUCCESSFUL
} flush_result;

/**
 * data package
 */
struct dataPack {
	struct list_elem elem;
	data_handle_t handle;
	char data[100];
};

int write_to_fileDescriptor(int fd, char* buff, size_t num_byte);

void find_buffer_store(int sensorNumber);

store_result_t storeData(const char *fileName, char buff[], uint8_t sensorNumber);
#endif /* DATAMANAGEMENT_H_ */
