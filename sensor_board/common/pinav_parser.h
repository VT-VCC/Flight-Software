#ifndef _PINAV_PARSER_H_
#define _PINAV_PARSER_H_

#include <stdint.h>

// All flight code will be in C, but the test library is technically C++.
// This means that all flight code must declare itself to have C linkage when
// compiled in C++ mode.
#ifdef __cplusplus
extern "C" {
#endif

// Status enum returned by parse function
typedef enum pinav_parser_status	{	PN_PARSE_OK, 
										PN_PARSE_NULL_OUTPUT_PTR,
										PN_PARSE_NULL_SENTENCE_PTR,
										PN_PARSE_UNRECOGNIZED_SENTENCE_TYPE,
										PN_PARSE_IMPROPER_SENTENCE_LENGTH,
										PN_PARSE_SENTENCE_FORMAT_ERROR,
										UNKNOWN_ERROR // @TODO: remove once I'm sure this never happens
									} pinav_parser_status_t;

// This identifies one of several sentence formats described in the piNAV datasheet
typedef enum pinav_sentence_id {NONE, GGA, LSP} pinav_sentence_id_t;

// Used in the pinav_gga struct
typedef enum fix_quality {
	FIX_INVALID = 0,
	FIX_VALID = 1
} fix_quality_t;

// Struct describing a GGA sentence
typedef struct pinav_gga {
	// The time of the fix in milliseconds since midnight UTC
	uint32_t fix_time_millis;
	// Latitude in millionths of a degree
	int32_t latitude;
	// Longitude in millionths of a degree
	int32_t longitude;
	// Elevation in centimeters
	// This could overflow only if we end up ~50x higher than the ISS
	int32_t elevation;
	// HDOP, represented as an 8.8 fixed point integer
	uint16_t hdop;
	// Number of satellites in view
	uint8_t sat_count;
	// Fix quality
	fix_quality_t fix_quality;
} pinav_gga_t;

// Struct describing an LSP sentence
typedef struct pinav_lsp {
	// The time of the fix in milliseconds since midnight UTC
	uint32_t fix_time_millis;
	// x, y, and z position in WGS-84, in centimeters
	// These types may need refinement, I'm not super confident
	// in my understanding of WGS-84
	int32_t wgs_x;
	int32_t wgs_y;
	int32_t wgs_z;
	// Number of satellites in view
	uint8_t sat_count;
	// PDOP, represented as an 8.8 fixed point integer
	uint16_t pdop;
} pinav_lsp_t;

// Union holding all possible piNAV sentence types
typedef union pinav_data {
	pinav_gga_t gga;
	pinav_lsp_t lsp;
} pinav_data_t;

// This is the struct that is populated by the parse function
// The id field specifies which field in the pinav_sentence
// union holds valid parsed data
typedef struct pinav_parse_output {
	pinav_sentence_id_t id;
	pinav_data_t data;
} pinav_parse_output_t;

// Parses the sentenced pointed to by sentence into the struct
// pointed to by out.
// Returns a status code stating that the operation succeded
// or why it failed
extern pinav_parser_status_t parse_pinav_sentence(pinav_parse_output_t * out, uint8_t * sentence);

#ifdef __cplusplus
}
#endif

#endif