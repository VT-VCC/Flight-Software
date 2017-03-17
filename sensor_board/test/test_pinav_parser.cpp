#include <catch/catch.hpp>

#include "pinav_parser.h"

TEST_CASE("Parser Test", "[pinav_parser]")
{
	pinav_parser_status_t result = parse_pinav_sentence(nullptr, nullptr);
	REQUIRE(result == OK);
}