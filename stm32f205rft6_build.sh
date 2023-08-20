#!/bin/bash

export KNL=mkrtos
export KEN_OFFSET=0x2000
export INIT_OFFSET=0x14000
export KNL_TEXT=0x8000000
export KNL_DATA=0x20000000
export KNL_DATA_SIZE=16K

set -e
cmake -G Ninja -B build/$KNL .
cd build/$KNL && ninja
