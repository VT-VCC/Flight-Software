#ifndef _FACTORIAL_H_
#define _FACTORIAL_H_

#include <stdint.h>

// All flight code will be in C, but the test library is technically C++.
// This means that all flight code must declare itself to have C linkage when
// compiled in C++ mode.
#ifdef __cplusplus
extern "C" {
#endif

/// A silly test function that doesn't do anything other than compute a
/// factorial
///
/// @param n The number to compute the factorial of.
extern uint64_t factorial(uint64_t n);

#ifdef __cplusplus
}
#endif

#endif
