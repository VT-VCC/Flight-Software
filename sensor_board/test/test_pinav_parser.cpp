#include <catch/catch.hpp>

#include "pinav_parser.h"

// @TODO: incomplete
TEST_CASE("Valid GGA sentence", "[pinav_parser]") {
	pinav_parse_output_t output;
	char * sentence = "$GPGGA,172120.384,5219.0671,N,05117.0926,E,1,9,\r\n";
	pinav_parser_status_t result = parse_pinav_sentence(&output, (uint8_t *)sentence);
	REQUIRE(result == UNKNOWN_ERROR);
	REQUIRE(output.id == GGA);
	REQUIRE(output.data.gga.fix_time_millis == 62480384);
	REQUIRE(output.data.gga.latitude == 52317785);
	REQUIRE(output.data.gga.longitude == 51284876);
	REQUIRE(output.data.gga.fix_quality == FIX_VALID);
	REQUIRE(output.data.gga.sat_count == 9);
}

TEST_CASE("Null output pointer", "[pinav_parser]"){
	pinav_parser_status_t result = parse_pinav_sentence(nullptr, nullptr);
	REQUIRE(result == PN_PARSE_NULL_OUTPUT_PTR);
}

TEST_CASE("Null sentence pointer", "[pinav_parser]"){
	pinav_parse_output_t output;
	pinav_parser_status_t result = parse_pinav_sentence(&output, nullptr);
	REQUIRE(result == PN_PARSE_NULL_SENTENCE_PTR);
	REQUIRE(output.id == NONE);
}

TEST_CASE("Sentence doesn't start with $" "[pinav_parser]") {
	pinav_parse_output_t output;
	char * sentence = "ABCD\r\n";
	pinav_parser_status_t result = parse_pinav_sentence(&output, (uint8_t *) sentence);
	REQUIRE(result == PN_PARSE_SENTENCE_FORMAT_ERROR);
	REQUIRE(output.id == NONE);
}

TEST_CASE("Invalid sentence length", "[pinav_parser]") {
	pinav_parse_output_t output;
	uint8_t sentence[81];
	sentence[0] = '$';
	for (int i = 1; i < 81; ++i) {
		sentence[i] = 'a';
	}
	pinav_parser_status_t result = parse_pinav_sentence(&output, sentence);
	REQUIRE(result == PN_PARSE_IMPROPER_SENTENCE_LENGTH);
	REQUIRE(output.id == NONE);
}

TEST_CASE("Invalid sentence type", "[pinav_parser]") {
	pinav_parse_output_t output;
	char * sentence = "$GPABC,\r\n";
	pinav_parser_status_t result = parse_pinav_sentence(&output, (uint8_t *)sentence);
	REQUIRE(result == PN_PARSE_UNRECOGNIZED_SENTENCE_TYPE);
	REQUIRE(output.id == NONE);
}