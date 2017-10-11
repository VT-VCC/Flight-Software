#!/bin/bash
set -euo pipefail

cd $(dirname $0)

mkdir -p build-host
cd build-host
cmake -G 'Ninja' ../
ninja
cd ../

for i in data-board dev-board sensor-board
do
  mkdir -p build-$i
  cd build-$i
  case $i in
    data-board)
      cmake_args='-DUSIP_DATA_BOARD=TRUE -DMSP430_MCU=msp430f5438a'
      ;;
    dev-board)
      cmake_args='-DUSIP_DEV_BOARD=TRUE -DMSP430_MCU=msp430fr5969'
      ;;
    sensor-board)
      cmake_args='-DUSIP_SENSOR_BOARD=TRUE -DMSP430_MCU=msp430fr5849'
  esac
  toolchain_file=$(pwd)/../cmake/custom_toolchains/msp430.cmake
  echo
  echo "********************************************************************************"
  echo "    Building using toolchain ${toolchain_file}"
  echo "********************************************************************************"
  echo

  cmake -G 'Ninja' -DCMAKE_TOOLCHAIN_FILE=${toolchain_file} ${cmake_args} ../
  ninja
  cd ../
done
