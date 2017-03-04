#include <catch/catch.hpp>

#include "sensor_board/gcd.h"

TEST_CASE("GCDs are computed", "[sensor_board]") {
  REQUIRE(gcd(1, 2) == 1);
  REQUIRE(gcd(100, 100) == 100);
  REQUIRE(gcd(100, 10) == 10);
  REQUIRE(gcd(10, 100) == 10);
  REQUIRE(gcd(8, 10) == 2);
  REQUIRE(gcd(10, 8) == 2);
}
