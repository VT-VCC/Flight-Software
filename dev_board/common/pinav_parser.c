#include "pinav_parser.h"
#include <string.h>
//#include <stdio.h>	// @TODO: remove when not needed for debugging

typedef enum generic_status { FAIL = 0, SUCCESS = 1 } generic_status_t;

// Private helper function for verifying that the sentence has a valid checksum
// The checksum is calculated as the logical XOR of all of the bytes after the initial '$'
static generic_status_t verifyChecksum(uint8_t * sentence) {
	uint8_t calculatedChecksum = sentence[1];
	size_t i;
	// XOR all bytes up to the * character
	for (i = 2; sentence[i] != '*'; ++i) {
		if ((sentence[i] == '\r') || (sentence[i] == '\n')) {
			return FAIL;
		}
		calculatedChecksum ^= sentence[i];
	}
	uint8_t expectedChecksum;
	size_t checksumChar = i + 1;
	// Check for valid hex characters and read expected value
	if ((sentence[checksumChar] >= '0') && (sentence[checksumChar] <= '9')) {
		expectedChecksum = sentence[checksumChar] - '0';
	}
	else if ((sentence[checksumChar] >= 'A') && (sentence[checksumChar] <= 'F')) {
		expectedChecksum = 10 + sentence[checksumChar] - 'A';
	}
	else {
		return FAIL;
	}
	expectedChecksum <<= 4;
	checksumChar += 1;
	if ((sentence[checksumChar] >= '0') && (sentence[checksumChar] <= '9')) {
		expectedChecksum |= (sentence[checksumChar] - '0');
	}
	else if ((sentence[checksumChar] >= 'A') && (sentence[checksumChar] <= 'F')) {
		expectedChecksum |= (10 + sentence[checksumChar] - 'A');
	}
	else {
		return FAIL;
	}
	// Fail out if expected and calculated checksums don't match
	if (expectedChecksum != calculatedChecksum) {
		return FAIL;
	}
	return SUCCESS;
}

// Private helper function to perform initial input validity checks
static pinav_parser_status_t verify_inputs(pinav_parse_output_t * out, uint8_t * sentence) {
	// Verify pointers
	if (!out) {
		return PN_PARSE_NULL_OUTPUT_PTR;
	}
	if (!sentence) {
		out->id = NONE;
		return PN_PARSE_NULL_SENTENCE_PTR;
	}

	// Verify that the sentence starts with '$'
	if (*sentence != '$') {
		out->id = NONE;
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}

	// Verify that the CR+LF sequence is within the 
	// 80 charater limit specified by the datasheet
	// @TODO Verify whether the CR+LF is included in the 80 char total
	uint8_t * it;
	uint8_t found = 0;
	for (it = sentence; (it < sentence + PINAV_MAX_SENTENCE_LEN) && (*it != '\0'); ++it) {
		if ((*it == '\r') && (*(it + 1) == '\n')) {
			found = 1;
			break;
		}
	}
	if (!found) {
		out->id = NONE;
		return PN_PARSE_IMPROPER_SENTENCE_LENGTH;
	}

	if (!verifyChecksum(sentence)) {
		return PN_PARSE_CHECKSUM_FAILURE;
	}

	return PN_PARSE_OK;
}

// Private helper function to determine whether a character represents an integer
static generic_status_t isInteger(uint8_t character) {
	if (character >= '0' && character <= '9') {
		return SUCCESS;
	}
	else {
		return FAIL;
	}
}

// Private helper function for converting string to int
static generic_status_t stoi(int32_t * i, uint8_t * s, size_t length) {
	uint8_t negative = 0; //@TODO: change this to a bool if we figure out how we're handling bools
	if (*s == '-') {
		negative = 1;
		++s;
	}

	if (isInteger(s[0])) {
		*i = *s - '0';
	}
	else {
		return FAIL;
	}
	uint8_t * it;
	for (it = s + 1; it < s + length; ++it) {
		if (isInteger(*it)) {
			*i *= 10;
			*i += *it - '0';
		}
		else {
			return FAIL;
		}
	}
	if (negative) {
		*i = 0 - *i;
	}
	
	return SUCCESS;
}

// Private helper function to find the number of characters from *it to the next occurence of '.'
// Returns -1 if ',' or end of string are encountered before '.' is found
static size_t charsToDecimalPoint(uint8_t * it) {
	size_t len = 0;
	while (*it != '.') {
		if (*it == ',' || *it == '\r' || *it == '\n') {
			return -1;
		}
		++len;
		++it;
	}
	return len;
}

// Private helper function to find next field deliminator 
// Returns number of characters >= 0 between *it and next occurance of ',' or '*' (* denotes start of checksum)
// Returns -1 if ',' or '*' not found
static int8_t next_deliminator(uint8_t * it) {
	int8_t count = 0;
	while (*it != ',' && *it != '*') {
		if ((*it == '\r') || (*it == '\n') || (*it == '\0')) {
			return -1;
		}
		++count;
		++it;
	}
	return count;
}

// Private helper function for converting a string to a 32.32 fixed point number
// out should point to the number to be written into
// s should point to the start of the field to be parsed
// Note: the function expects a deliminator at the end of the number, 
//		and will fail if used on the last field in a sentence
static generic_status_t string_to_fixed_point(int64_t * out, uint8_t * s) {
	uint8_t negative = 0;	//@TODO: replace with bool once we figure out how we're handling bools
	if (*s == '-') {
		negative = 1;
		++s;
	}

	// Integer portion
	size_t integerLength = charsToDecimalPoint(s);
	if (integerLength == -1) {	// Handle number with no decimal
		integerLength = next_deliminator(s);
		if (integerLength == -1) {
			return FAIL;
		}
	}
	int32_t intergerPortion;
	if (!stoi(&intergerPortion, s, integerLength)) {
		return FAIL;
	}
	s += integerLength;
	// Fractional portion
	uint32_t fractionalPortion = 0;
	if(*s == '.') {
		++s; //move pointer past the decimal point
		size_t fractionalLength = next_deliminator(s);
		if (fractionalLength == -1) {
			return FAIL;
		}
		uint32_t numerator;
		if (!stoi((uint32_t*)&numerator, s, fractionalLength)) {
			return FAIL;
		}
		uint32_t denominator = 1;
		size_t i;
		for (i = 0; i < fractionalLength; ++i) { // denominator = 10^fractionalLength
			denominator *= 10;
		}
		for (i = 0; i < 32; ++i) {	// Calculate each bit of fractionalPortion
			numerator *= 2;
			fractionalPortion <<= 1;
			fractionalPortion += numerator / denominator;
			numerator -= (numerator / denominator) * denominator;
		}
	}
	int64_t result = intergerPortion;
	result <<= 32;
	result |= fractionalPortion;
	if (negative) {
		result = 0 - result;
	}
	*out = result;

	return SUCCESS;
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
	if (!stoi(&degrees, it, 2)) {
		return FAIL;
	}
	int32_t minutes;
	if (!stoi(&minutes, it + 2, 2)) {
		return FAIL;
	}
	if (it[4] != '.') {
		return FAIL;
	}
	int32_t ten_thousandths_of_minute;
	if (!stoi(&ten_thousandths_of_minute, it + 5, 4)) {
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
	if (!stoi(&degrees, it, 3)) {
		return FAIL;
	}
	int32_t minutes;
	if (!stoi(&minutes, it + 3, 2)) {
		return FAIL;
	}
	if (it[5] != '.') {
		return FAIL;
	}
	int32_t ten_thousandths_of_minute;
	if (!stoi(&ten_thousandths_of_minute, it + 6, 4)) {
		return FAIL;
	}

	//calculate 600,000ths of a degree
	int32_t six_100000s_of_degrees = degrees * 600000;
	six_100000s_of_degrees += minutes * 10000;
	six_100000s_of_degrees += ten_thousandths_of_minute;

	// Scale to millionths of a degree
	int32_t millionths_of_degrees = (six_100000s_of_degrees * 10) / 6;

	// check N/S
	if (it[10] != ',') {	// If this isn't safe, we would have failed by now
		return FAIL;
	}
	if (it[11] == 'E') {
		*lng = millionths_of_degrees;
	}
	else if (it[11] == 'W') {
		*lng = 0 - millionths_of_degrees;
	}
	else {
		return FAIL;
	}

	return SUCCESS;
}

// Private helper function for parsing the gga
// "Number of satellites being tracked" field
// uint8_t * it should point to the start of the field
static generic_status_t parse_tracked_sats(uint8_t * out, uint8_t * it) {
	uint32_t sat_count;
	int8_t len;
	len = next_deliminator(it);
	if (len < 1 || len > 2) { // Can't track a 3+ digit number of satellites
		return FAIL;
	}
	if (stoi((uint32_t*)&sat_count, it, len)) {
		*out = (uint8_t) sat_count;
	}
	return SUCCESS;
}

static generic_status_t parse_gga_hdop(uint16_t * out, uint8_t * it) {
	int64_t hdop;
	if (!string_to_fixed_point(&hdop, it)) {
		return FAIL;
	}
	hdop >>= 24; // Convert to 8.8 fixed point
	*out = (uint16_t)hdop;

	return SUCCESS;
}

// Private helper function to parse gga altitude field
static generic_status_t parse_gga_altitude(int32_t * out, uint8_t * it) {
	int64_t altitude;
	if (!string_to_fixed_point(&altitude, it)) {
		return FAIL;
	}
	altitude *= 100; // Convert meters to cm
	*out = altitude >> 32; // Convert fixed-point to integer

	return SUCCESS;
}

// Private helper function to parse the valid gga sentence
// pointed to by sentence into the output struct pointed to by out
static pinav_parser_status_t parse_gga(pinav_parse_output_t * out, uint8_t * sentence){
	uint8_t * it; // iterator for parsing sentence

	out->id = GGA;

	it = sentence + 7; // place iterator at start of fix time field
	if (!parse_gga_fix_time(&(out->data.gga.fix_time_millis), it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	if (it[10] != ',') {	// If this isn't safe, parse_gga_fix_time would have failed
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += 11; // Place iterator at start of lat field 
	if (!parse_gga_lat(&(out->data.gga.latitude), it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += 12; // Place iterator at start of long field
	if (!parse_gga_long(&(out->data.gga.longitude), it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += 13; // Place iterator on the Fix Quality field
	if (*it == '0') {
		out->data.gga.fix_quality = FIX_INVALID;
	}
	else if (*it == '1') {
		out->data.gga.fix_quality = FIX_VALID;
	}
	else {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += 2; // Place iterator at the start of the tracked satellites field
	if (!parse_tracked_sats(&out->data.gga.sat_count, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1;	// Place iterator on the HDOP field
								// If this were unsafe, parse_gga_tracked_sats would have failed
	if (!parse_gga_hdop(&out->data.gga.hdop, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1; // Place iterator on the altitude field
	if (!parse_gga_altitude(&out->data.gga.altitude, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1;
	if (*it != 'M') {
		return PN_PARSE_UNEXPECTED_UNIT_ENCOUNTERED;
	}

	// We've got everything we need

	return PN_PARSE_OK;
}

// Private helper function to parse the valid lsp sentence
// pointed to by sentence into the output struct pointed to by out
static pinav_parser_status_t parse_lsp(pinav_parse_output_t * out, uint8_t * sentence){
	uint8_t * it; // iterator for parsing sentence

	out->id = LSP;

	it = sentence + 7; // place iterator at start of gps time seconds field
	if (!string_to_fixed_point((int64_t*)&out->data.lsp.gps_time_seconds, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1; // place iterator at the start of the gps week field
	int64_t gps_week;
	if (!string_to_fixed_point(&gps_week, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	out->data.lsp.gps_week = (uint16_t)(gps_week >> 32); // Convert to 16-bit integer
	it += next_deliminator(it) + 1; // Place iterator at start of X position field
	if (!string_to_fixed_point(&out->data.lsp.wgs_x, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1; // Place iterator at start of Y position field
	if (!string_to_fixed_point(&out->data.lsp.wgs_y, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1; // Place iterator at start of Z position field
	if (!string_to_fixed_point(&out->data.lsp.wgs_z, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1; // Place iterator at start of the sat_count field
	if (!parse_tracked_sats(&out->data.lsp.sat_count, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1;	// Place iterator on the PDOP field
	int64_t pdop;
	if (!string_to_fixed_point(&pdop, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	pdop >>= 24; // Convert to 8.8 fixed point
	out->data.lsp.pdop = (uint16_t)pdop;

	return PN_PARSE_OK;
}

// Private helper function to parse the valid lsv sentence
// pointed to by sentence into the output struct pointed to by out
static pinav_parser_status_t parse_lsv(pinav_parse_output_t * out, uint8_t * sentence) {
	uint8_t * it; // iterator for parsing sentence

	out->id = LSV;

	it = sentence + 7; // place iterator at start of gps time seconds field
	if (!string_to_fixed_point((int64_t*)&out->data.lsv.gps_time_seconds, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1; // place iterator at the start of the gps week field
	int64_t gps_week;
	if (!string_to_fixed_point(&gps_week, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	out->data.lsv.gps_week = (uint16_t)(gps_week >> 32); // Convert to 16-bit integer
	it += next_deliminator(it) + 1; // Place iterator at start of X velocity field
	if (!string_to_fixed_point(&out->data.lsv.wgs_vx, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1; // Place iterator at start of Y velocity field
	if (!string_to_fixed_point(&out->data.lsv.wgs_vy, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1; // Place iterator at start of Z velocity field
	if (!string_to_fixed_point(&out->data.lsv.wgs_vz, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1; // Place iterator at start of the sat_count field
	if (!parse_tracked_sats(&out->data.lsv.sat_count, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	it += next_deliminator(it) + 1;	// Place iterator on the PDOP field
	int64_t pdop;
	if (!string_to_fixed_point(&pdop, it)) {
		return PN_PARSE_SENTENCE_FORMAT_ERROR;
	}
	pdop >>= 24; // Convert to 8.8 fixed point
	out->data.lsv.pdop = (uint16_t)pdop;

	return PN_PARSE_OK;
}

pinav_parser_status_t parse_pinav_sentence(pinav_parse_output_t * out, uint8_t * sentence){
	// Basic checks to be performed before parsing
	pinav_parser_status_t input_check = verify_inputs(out, sentence);
	if (input_check != PN_PARSE_OK) {
		return input_check;
	}

	// Determine the sentence type and call appropriate parse function
	if (!strncmp(sentence + 1, "GPGGA,", 6)) {
		return parse_gga(out, sentence);
	}
	if (!strncmp(sentence + 1, "PSLSP,", 6)) {
		return parse_lsp(out, sentence);
	}
	if (!strncmp(sentence + 1, "PSLSV,", 6)) {
		return parse_lsv(out, sentence);
	}

	// Default if sentence not recognized:
	out->id = NONE;
	return PN_PARSE_UNRECOGNIZED_SENTENCE_TYPE;
}
