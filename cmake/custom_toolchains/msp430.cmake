include(CMakeForceCompiler)
# Based on the KuBOS MSP 430 target file
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR msp430)

# get the root of the toolchain
find_program(K_GCC "msp430-elf-gcc")
if (NOT K_GCC)
  message("================================================================================")
  message(" ERROR: Unable to find the MSP430 toolchain")
  message("================================================================================")
endif()

# compute the toolchain root from the location of the compiler
get_filename_component(_MSP430_TOOLCHAIN_ROOT ${K_GCC} DIRECTORY)
get_filename_component(MSP430_TOOLCHAIN_ROOT ${_MSP430_TOOLCHAIN_ROOT} DIRECTORY CACHE STRING "MSP430 GCC toolchain root")

# assume that mspgcc is setup like a normal GCC install
set(K_GPP "${MSP430_TOOLCHAIN_ROOT}/bin/msp430-elf-g++")
set(K_OBJCOPY "${MSP430_TOOLCHAIN_ROOT}/bin/msp430-elf-objcopy")

# force the C/C++ compilers
set(CMAKE_C_COMPILER ${K_GCC})
set(CMAKE_CXX_COMPILER ${K_GPP})

# target build environment root directory
set(CMAKE_FIND_ROOT_PATH ${MSP430_TOOLCHAIN_ROOT})

# Disable C/C++ features that are inefficient or impossible to implement on a microcontroller
if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(_C_FAMILY_FLAGS_INIT "-fno-exceptions -fno-unwind-tables -ffunction-sections -fdata-sections -Wall -Wextra -gstrict-dwarf")
else()
  set(_C_FAMILY_FLAGS_INIT "-fno-exceptions -fno-unwind-tables -ffunction-sections -fdata-sections -Wextra -gstrict-dwarf")
endif()

# set some default flags
set(CMAKE_C_FLAGS_INIT   "--std=gnu99 ${_C_FAMILY_FLAGS_INIT}")
set(CMAKE_ASM_FLAGS_INIT "-fno-exceptions -fno-unwind-tables -x assembler-with-cpp")
set(CMAKE_CXX_FLAGS_INIT "--std=gnu++11 ${_C_FAMILY_FLAGS_INIT} -fno-rtti -fno-threadsafe-statics")
set(CMAKE_MODULE_LINKER_FLAGS_INIT
    "-fno-exceptions -fno-unwind-tables -Wl,--gc-sections -Wl,--sort-common -Wl,--sort-section=alignment"
)
