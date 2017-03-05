# Based on the KuBOS MSP 430 target file
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")

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

# target build environment root directory
set(CMAKE_FIND_ROOT_PATH ${MSP430_TOOLCHAIN_ROOT})

# expose the MCU as a cmake variable
if(NOT MSP430_MCU)
  set(MSP430_MCU "msp430fr5969" CACHE STRING "MCU to target")
endif()

# Disable C/C++ features that are inefficient or impossible to implement on a microcontroller
set(_DISABLE_EXCEPTIONS_FLAGS "-fno-exceptions -fno-unwind-tables ")
set(_C_FAMILY_FLAGS_INIT "${DISABLE_EXCEPTIONS_FLAGS} -ffunction-sections -fdata-sections -Wextra -gstrict-dwarf -mmcu=${MSP430_MCU}")

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(_C_FAMILY_FLAGS_INIT "${_C_FAMILY_FLAGS_INIT} -Wall")
endif()

# set some default flags
set(CMAKE_C_FLAGS_INIT   "--std=gnu99 ${_C_FAMILY_FLAGS_INIT}")
set(CMAKE_ASM_FLAGS_INIT "-fno-exceptions -fno-unwind-tables -x assembler-with-cpp")
set(CMAKE_CXX_FLAGS_INIT "--std=gnu++11 ${_C_FAMILY_FLAGS_INIT} -fno-rtti -fno-threadsafe-statics")
set(CMAKE_MODULE_LINKER_FLAGS_INIT
  "${DISABLE_EXCEPTIONS_FLAGS} -Wl,--gc-sections -Wl,--sort-common -Wl,--sort-section=alignment"
)
set(CMAKE_EXE_LINKER_FLAGS_INIT "-L ${MSP430_TOOLCHAIN_ROOT}/include" CACHE STRING "")

link_directories("${MSP430_TOOLCHAIN_ROOT}/include")

# and add the system include directory for the compiler
include_directories(SYSTEM "${MSP430_TOOLCHAIN_ROOT}/include")

# force the C/C++ compilers
if(CMAKE_VERSION VERSION_LESS "3.6.0")
  include(CMakeForceCompiler)
  cmake_force_c_compiler("${K_GCC}" GNU)
  cmake_force_cxx_Compiler("${K_GPP}" GNU)
else()
  set(CMAKE_C_COMPILER ${K_GCC})
  set(CMAKE_CXX_COMPILER ${K_GPP})
endif()
