#include "factorial.h"

uint64_t factorial(uint64_t n) {
  uint64_t tr = 1;
  for (int i = 2; i <= n; ++i) {
    tr *= i;
  }
  return tr;
}
