#include <catch/catch.hpp>

#include "factorial.h"

TEST_CASE("factorials are computed", "[factorial]") {
  REQUIRE(factorial(0) == 1);
  REQUIRE(factorial(1) == 1);
  REQUIRE(factorial(2) == 2);
  REQUIRE(factorial(10) == 3628800);
}
