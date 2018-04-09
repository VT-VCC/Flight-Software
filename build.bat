@echo off
setlocal enabledelayedexpansion

if "%*" == "" (
    set "boards_to_build=data-board dev-board sensor-board"
) else (
    set "boards_to_build=%*"
)

:: Setup the native build environment

echo ================================================================================
echo Building native test binaries 
echo ================================================================================

pushd \
call vcvarsall.bat x64
popd

mkdir build-host
pushd build-host

cmake -G Ninja ..\
ninja

popd

:: Setup the MSP430 build environment

for %%i in (%boards_to_build%) do (
    echo ================================================================================
    echo Building cross-compiled MSP430 binaries for target %%i
    echo ================================================================================

    mkdir build-%%i
    pushd build-%%i

    if %%i==data-board set "cmake_args=-DUSIP_DATA_BOARD=TRUE -DMSP430_MCU=msp430f5438a"
    if %%i==dev-board set "cmake_args=-DUSIP_DEV_BOARD=TRUE -DMSP430_MCU=msp430fr5994"
    if %%i==sensor-board set "cmake_args=-DUSIP_SENSOR_BOARD=TRUE -DMSP430_MCU=msp430fr5849"

    cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=..\cmake\custom_toolchains\msp430.cmake !cmake_args! ..\
    ninja

    popd
)
