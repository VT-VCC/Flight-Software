#include "pinav_parser.h"
#include <string.h>
#include <stdio.h>	// @TODO: remove when not needed for debugging

typedef enum generic_status { FAIL = 0, SUCCESS = 1 } generic_status_t;

// Private helper function to perform initial input validity checks
static pinav_parser_status_t verify_inputs(pinav_parse_output_t * out, uint8_t * sentence) {
	// Verify pointers
	if (!out) {
		return NULL_OUTPUT_PTR;
	}
	if (!sentence) {
		out->id = NONE;
		return NULL_SENTENCE_PTR;
	}

	// Verify that the sentence starts with '$'
	if (*sentence != '$') {
		out->id = NONE;
		return SENTENCE_FORMAT_ERROR;
	}

	// Verify that the CR+LF sequence is within the 
	// 80 charater limit specified by the datasheet
	// @TODO Verify whether the CR+LF is included in the 80 char total
	uint8_t * it;
	uint8_t found = 0;
	for (it = sentence; (it < sentence + 79) && (*it != '\0'); ++it) {
		if ((*it == '\r') && (*(it + 1) == '\n')) {
			found = 1;
			break;
		}
	}
	if (!found) {
		out->id = NONE;
		return IMPROPER_SENTENCE_LENGTH;
	}

	return OK;
}

// Private helper function to determine whether a string can be converted to a number
static generic_status_t can_convert_stoi(const uint8_t * s, size_t len) {
	generic_status_t result = SUCCESS;
	
	uint8_t * it;
	for (it = s; it < s + len; ++it) {
		if ((*it < '0') || (*it > '9')) {
			result = FAIL;
			break;
		}
	}

	return result;
}

// Private helper function for converting string to int
static generic_status_t stoi(uint32_t * i, uint8_t * s, size_t length) {
	if (can_convert_stoi(s, length)) {
		*i = *s - '0';
		uint8_t * it;
		for (it = s + 1; it < s + length; ++it) {
			*i *= 10;
			*i += *it - '0';
		}
		return SUCCESS;
	}
	return FAIL;
}

// Private helper function for parsing fix time from GGA sentence
static generic_status_t parse_gga_fix_time(uint32_t * time, uint8_t * start) {
	int32_t hours;
	if (!stoi(&hours, start, 2)) {
		return FAIL;
	}
	int32_t minutes;
	if (!stoi(&minutes, start + 2, 2)) {
		return FAIL;
	}
	int32_t seconds;
	if (!stoi(&seconds, start + 4, 2)) {
		return FAIL;
	}
	if (start[6] != '.') {
		return FAIL;
	}
	int32_t millis;
	if (!stoi(&millis, start + 7, 3)) {
		return FAIL;
	}

	// Set out to the number of milliseconds since midnight UTC
	*time = millis;
	*time += (seconds * 1000);
	*time += (minutes * 60000);
	*time += (hours * 60 * 60000);
	return SUCCESS;
}

// Private helper function for parsing out latitude from gga sentence
// uint8_t * it should point to the start of the latitude field
static generic_status_t parse_gga_lat(int32_t * lat, uint8_t * it) {
	int32_t degrees;
	if (!stoi((uint32_t *)&degrees, it, 2)) {
		return FAIL;
	}
	int32_t minutes;
	if (!stoi((uint32_t *)&minutes, it + 2, 2)) {
		return FAIL;
	}
	if (it[4] != '.') {
		return FAIL;
	}
	int32_t ten_thousandths_of_minute;
	if (!stoi((uint32_t *)&ten_thousandths_of_minute, it + 5, 4)) {
		return FAIL;
	}

	//calculate 600,000ths of a degree
	int32_t six_100000s_of_degrees = degrees * 600000;
	six_100000s_of_degrees += minutes * 10000;
	six_100000s_of_degrees += ten_thousandths_of_minute;

	// Scale to millionths of a degree
	int32_t millionths_of_degrees = (six_100000s_of_degrees * 10) / 6;

	// check N/S
	if (it[9] != ',') {	// If this isn't safe, we would have failed by now
		return FAIL;
	}
	if (it[10] == 'N') {
		*lat = millionths_of_degrees;
	}
	else if (it[10] == 'S') {
		*lat = 0 - millionths_of_degrees;
	}
	else {
		return FAIL;
	}

	return SUCCESS;
}

// Private helper function for parsing out longitude from gga sentence
// uint8_t * it should point to the start of the longitude field
static generic_status_t parse_gga_long(int32_t * lng, uint8_t * it) {
	int32_t degrees;
	if (!stoi((uint32_t *)&degrees, it, 3)) {
		return FAIL;
	}
	int32_t minutes;
	if (!stoi((uint32_t *)&minutes, it + 3, 2)) {
		return FAIL;
	}
	if (it[5] != '.') {
		return FAIL;
	}
	int32_t ten_thousandths_of_minute;
	if (!stoi((uint32_t *)&ten_thousandths_of_minute, it + 6, 4)) {
		return FAIL;
	}

	//calculate 600,000ths of a degree
	int32_t six_100000s_of_degrees = degrees * 600000;
	six_100000s_of_degrees += minutes * 10000;
	six_100000s_of_degrees += ten_thousandths_of_minute;

	// Scale to millionths of a degree
	int32_t millionths_of_degrees = (six_100000s_of_degrees * 10) / 6;

	// check N/S
	if (it[9] != ',') {	// If this isn't safe, we would have failed by now
		return FAIL;
	}
	if (it[10] == 'E') {
		*lng = millionths_of_degrees;
	}
	else if (it[10] == 'W') {
		*lng = 0 - millionths_of_degrees;
	}
	else {
		return FAIL;
	}

	return SUCCESS;
}

// Private helper function to find next comma 
// Returns number of characters >= 0 between *it and next occurance of ','
// Returns -1 if ',' not found
static int8_t next_comma(uint8_t * it) {
	int8_t count = 0;
	while (*it != ',') {
		if ((*it == '\r') || (*it == '\n') || (*it == '\0')) {
			return -1;
		}
		++count;
	}
	return count;
}


// Private helper function to parse the valid gga sentence
// pointed to by sentence into the output struct pointed to by out
static pinav_parser_status_t parse_gga(pinav_parse_output_t * out, uint8_t * sentence){
	uint8_t * it; // iterator for parsing sentence
	int8_t to_next_comma;	// distance to next comma in sentence

	out->id = GGA;

	it = sentence + 7; // place iterator at start of fix time field
	if (!parse_gga_fix_time(&(out->data.gga.fix_time_millis), it)) {
		return SENTENCE_FORMAT_ERROR;
	}
	if (it[10] != ',') {	// If this isn't safe, parse_gga_fix_time would have failed
		return SENTENCE_FORMAT_ERROR;
	}
	it += 11; // Place iterator at start of lat field 
	if (!parse_gga_lat(&(out->data.gga.latitude), it)) {
		return SENTENCE_FORMAT_ERROR;
	}
	it += 12; // Place iterator at start of long field
	if (!parse_gga_long(&(out->data.gga.longitude), it)) {
		return SENTENCE_FORMAT_ERROR;
	}

	/*to_next_comma = next_comma(it);
	if (to_next_comma < 1) {
		return SENTENCE_FORMAT_ERROR;
	}
	else {
		it += (to_next_comma + 1);
	}*/

	return UNKNOWN_ERROR;
}

// Private helper function to parse the valid lsp sentence
// pointed to by sentence into the output struct pointed to by out
static pinav_parser_status_t parse_lsp(pinav_parse_output_t * out, uint8_t * sentence){
	out->id = LSP;

	return UNKNOWN_ERROR;
}

pinav_parser_status_t parse_pinav_sentence(pinav_parse_output_t * out, uint8_t * sentence){
	// Basic checks to be performed before parsing
	pinav_parser_status_t input_check = verify_inputs(out, sentence);
	if (input_check != OK) {
		return input_check;
	}

	// Determine the sentence type and call appropriate parse function
	if (!strncmp(sentence + 1, "GPGGA,", 6)) {
		return parse_gga(out, sentence);
	}
	if (!strncmp(sentence + 1, "PSLSP,", 6)) {
		return parse_lsp(out, sentence);
	}

	// Default if sentence not recognized:
	out->id = NONE;
	return UNRECOGNIZED_SENTENCE_TYPE;
}
