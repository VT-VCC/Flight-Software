/*
 * DataManagement.c
 *
 *  Created on: Mar 8, 2017
 *      Author: souldia
 */
#include <stdio.h>
#include "DataManagement.h"
#include "list.h"
#include <errno.h>

uint32_t imuDataCounter = 0x00;
uint32_t radioDataCounter = 0x00;
uint32_t gpsDataCounter = 0x00;
uint8_t PACKAGENUM = 3;
uint32_t MAXLENGTHBLOCK = 1024;
uint32_t RETRIVAL_INDEX_IMU = 0x00;

/**
 * creating a list of package number to be written to the file
 */
struct list package[PACKAGENUM];

void initialize() {
	for (int i = 0; i < PACKAGENUM; i++) {
		list_init(&(package[i]));
	}
}

/**
 * Get the file descriptor for this file
 */
int getFileDescriptor(char *fileName) {
	FILE *fp;
	if ((fp = fopen(fileName, "r")) == NULL) {
		// fail to open the file and need to handle that
		perror();
		return -1;
	}
	return fileno(fp); // this should not fail, so no check needed
}
store_result_t storeData(const char *fileName, char buff[],
		uint8_t sensorNumber) {
	assert(sensorNumber == 1);
	struct list *imuPack = package[sensorNumber];
	//check if list buffer need to be overwritten
	if ((imuDataCounter % MAXLENGTHBLOCK) == 0) {
		// need to copy data from buffer and set its bit to dirty so that
		// it can be overwritten
		int fd = getFileDescriptor(fileName);
		if (fd < 0) {
			return STORE_NOT_OK;
		}
		struct list *foo_list = package[sensorNumber];
		struct list_elem *e;
		// here we have a choice we can remove them or just set their dirty bit flag
		// if we remove, there will be cost of adding
		// if we set flag, there will be cost of traversing the list
		for (e = list_begin(&foo_list); e != list_end(&foo_list);
				e = list_next(e)) {

			struct dataPack *currPack = list_entry(e, struct dataPack, elem);
			// write data that is in this package
			// we can use fprintf when we would like to put it into specific format
			// we probably want a fix data length for reading and writing
			if (write_to_fileDescriptor(fd, (&currPack)->data,
					sizeof((&currPack).data)) == -1) {
				perror("Failed to copy to file");
				return STORE_NOT_OK;
			}
			(&currPack)->handle.dirtyBit = 1;
		}
		//now store the new data in the buffer
		find_buffer_store(sensorNumber, imuDataCounter, buff);
	} else {
		find_buffer_store(sensorNumber, imuDataCounter, buff);
	}
	return STORE_OK;
}
/**
 * This function find the correct buffer and store the data
 */
void find_buffer_store(int sensorNumber, int sensorType, char buff) {
	struct list *foo_list = package[sensorNumber];
	// need to add it at the end of this buffer
	uint8_t pos = (sensorType % MAXLENGTHBLOCK) / 100;
	struct list *newList = package[sensorNumber];
	struct list_elem *curPos = list_begin(&foo_list);
	uint8_t counter = 0;
	while (counter < pos) {
		curPos = list_next(curPos);
		counter++;
	}
	assert(curPos != NULL);
	struct dataPack *currPack = list_entry(curPos, struct dataPack, elem);
	currPack.data = buff;
	currPack->handle.dirtyBit = 0;
}

/**
 * This method write character by character to make bit checking easier
 */
int write_to_fileDescriptor(int fd, char* buff, size_t num_byte) {
	size_t n_left = num_byte;
	char * buff_pos = buff;
	ssize_t num_written;

	while (n_left > 0) {
		num_written = write(fd, buff_pos, n_left);
		if (num_written < 0) {
			switch (errno) {
			case EAGAIN:
				//perror("The O_NONBLOCKFLAG is set for this fd and the process will be delayed.\n");
				return -1;
			case EDESTADDRREQ:
				//perror("fd refers to a datagram socket for which a peer address has not been set using connect.\n");
				return -1;
			case EFAULT:
				//perror("buffer is outside your accessible address space.\n");
				return -1;
			case EINTR:
				num_written = 0;
				break;
			case EPIPE:
				//perror("The fd is connected to a socket whose reading end is closed.\n");
				return -1;
			}
		}
		n_left -= num_written;
		buff_pos += num_written;
	}
	return 0;
}

/**
 * Retrieve data stored in the buffer or memory
 * pos: should be negative if we would like to retrieve from last position
 * (use pos to only retrieve a specific unit)
 * sensorNumber: specify which sensor data you will want to retrieve
 *
 */
retrieve_result_t retrieveData(int sensor_type, int pos, char *fileName,
		char buff[]) {
	// we would like to retrive specific data
	struct dataPack *currPack;
	if (pos > 0) {
		struct list *foo_list = package[sensor_type];
		struct list_elem *e;
		// here we have a choice we can remove them or just set their dirty bit flag
		// if we remove, there will be cost of adding
		// if we set flag, there will be cost of traversing the list
		for (e = list_begin(&foo_list); e != list_end(&foo_list);
				e = list_next(e)) {

			currPack = list_entry(e, struct dataPack, elem);
			if (currPack->handle.idx == pos) {
				break;
			}
		}
		// this mean its not in the buffer and we need to get it from the file
		// we can do this better when we know what kind of position are going to be given
		// handle are exact position from in the file
		if (currPack == NULL) {
			int fd = getFileDescriptor(fileName);
			if (fd < 0) {
				return RETRIEVE_NOT_OK;
			}
			if (lseek(fd, pos, SEEK_SET) < 0) {
				print("%s\n", strerror(errno));
				return RETRIEVE_NOT_OK;
			}
			if (read_from_fileDescriptor(fd, buff, sizeof(buff)) < 0) {
				return RETRIEVE_NOT_OK;
			}
			//XXX: need to verify its the right data (so we need to serialize the data)
			return RETRIEVE_OK;
		}		//its in the buffer
		else {
			buff = currPack->data;
			//assuming they are the same size
		}

	}				// retrieve last position
	else {
		//check to see if its in the buffer
		struct list *foo_list = package[sensor_type];
		struct list_elem *e;
		// here we have a choice we can remove them or just set their dirty bit flag
		// if we remove, there will be cost of adding
		// if we set flag, there will be cost of traversing the list
		for (e = list_begin(&foo_list); e != list_end(&foo_list);
				e = list_next(e)) {

			currPack = list_entry(e, struct dataPack, elem);
			if (currPack->handle.idx == pos) {
				break;
			}
		}
		if (currPack == NULL) {
			int fd = getFileDescriptor(fileName);
			if (fd < 0) {
				return RETRIEVE_NOT_OK;
			}
			if (lseek(fd, RETRIVAL_INDEX_IMU, SEEK_SET) < 0) {
				print("%s\n", strerror(errno));
				return RETRIEVE_NOT_OK;
			}
			if (read_from_fileDescriptor(fd, buff, sizeof(buff)) < 0) {
				return RETRIEVE_NOT_OK;
			}
			//XXX: need to verify its the right data (so we need to serialize the data)
			return RETRIEVE_OK;
		} else {
			buff = currPack->data; //assuming they are the same size
		}
	}
	return RETRIEVE_OK;
}
/**
 * Character by character reading from the fd
 */
int read_from_fileDescriptor(int fd, char buff[], size_t max_length) {
	int num_read;
	char c, *buff_pos = buff;
	int i;
	for (i = 0; (i < max_length); i++) {
		if ((num_read = read(fd, &c, 1)) == 1) {
			if (c == '\n') {
				break;
			}
			if (c != '\r') {
				*buff_pos++ = c;
			}
		} else if (num_read == 0) {
			if (i == 0) {
				return EOF; //EOF no data
			} else {
				break;
			}
		} else {
			return -1; //error
		}
	}
	*buff_pos = 0;
	return 0;
}
