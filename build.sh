#!/bin/bash
set -euo pipefail

cd $(dirname $0)

for i in dev_board sensor_board data_board
do
  cd $i
  ./build.sh
  cd ../
done
