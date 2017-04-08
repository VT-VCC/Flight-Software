#include <catch/catch.hpp>
#include "pinav_parser.h"

// @TODO: incomplete
TEST_CASE("Valid GGA sentence", "[pinav_parser]") {
	pinav_parse_output_t output;
	char * sentence = "$GPGGA,172120.384,5219.0671,N,05117.0926,E,1,9,0.9,371262.1,M,0,M,,,*54\r\n";
	pinav_parser_status_t result = parse_pinav_sentence(&output, (uint8_t *)sentence);
	REQUIRE(result == PN_PARSE_OK);
	REQUIRE(output.id == GGA);
	REQUIRE(output.data.gga.fix_time_millis == 62480384);
	REQUIRE(output.data.gga.latitude == 52317785);
	REQUIRE(output.data.gga.longitude == 51284876);
	REQUIRE(output.data.gga.fix_quality == FIX_VALID);
	REQUIRE(output.data.gga.sat_count == 9);
	REQUIRE(output.data.gga.hdop == 0b0000000011100110); //0.9 in 8.8 fixed-point
	REQUIRE(((output.data.gga.altitude >= 37126209) && (output.data.gga.altitude <= 37126211))); // Some leniency for binary/decimal conversion error
}

TEST_CASE("Valid LSP sentence", "[pinav_parser]") {
	pinav_parse_output_t output;
	char * sentence = "$PSLSP,193772.250,780,3963889.500,1001383.750,4879428.125,5,4.5*72\r\n";
	pinav_parser_status_t result = parse_pinav_sentence(&output, (uint8_t *)sentence);
	REQUIRE(result == PN_PARSE_OK);
	REQUIRE(output.id == LSP);
	int64_t expected_val = 193772;
	expected_val <<= 32;
	uint32_t fraction = 0b01000000 << 24;
	expected_val |= fraction;
	REQUIRE(output.data.lsp.gps_time_seconds == expected_val);
	REQUIRE(output.data.lsp.gps_week == 780);
	expected_val = 3963889;
	expected_val <<= 32;
	fraction = 0b10000000 << 24;
	expected_val |= fraction;
	REQUIRE(output.data.lsp.wgs_x == expected_val);
	expected_val = 1001383;
	expected_val <<= 32;
	fraction = 0b11000000 << 24;
	expected_val |= fraction;
	REQUIRE(output.data.lsp.wgs_y == expected_val);
	expected_val = 4879428;
	expected_val <<= 32;
	fraction = 0b00100000 << 24;
	expected_val |= fraction;
	REQUIRE(output.data.lsp.wgs_z == expected_val);
	REQUIRE(output.data.lsp.sat_count == 5);
	REQUIRE(output.data.lsp.pdop == 0x0480);
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