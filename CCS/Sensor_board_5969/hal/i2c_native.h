#ifndef _BOARD_COMMON_NATIVE_I2C_H_
#define _BOARD_COMMON_NATIVE_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "usci_includes.h"


#if defined(USING_USCI )
#   include "i2c_usci_native.h"
#elif defined(USING_EUSCI )
#   include "i2c_eusci_native.h"
#else
#   error "No MSP class defined"
#endif


#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_NATIVE_I2C_H_
