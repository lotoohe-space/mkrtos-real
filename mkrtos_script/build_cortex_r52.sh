#!/bin/bash

# export TOOLCHAIN=/home/zhangzheng/gcc-arm-none-eabi-5_4-2016q3/bin/
# export TOOLCHAIN_LIB=/home/zhangzheng/gcc-arm-none-eabi-5_4-2016q3/lib/gcc/arm-none-eabi/5.4.1/armv7-m
export TOOLCHAIN=/Users/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/bin/
export TOOLCHAIN_LIB=/Users/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-m/nofp
export KEN_OFFSET=0x2000
export INIT_OFFSET=0x10000
export BOOTFS_ADDR_OFFSET=0x20000
export KNL_TEXT=0x8000000
export KNL_DATA=0x20000000
export KNL_DATA_SIZE=64K
export BOARD=STM32F2x
export ARCH=cortex-r52
export PYTHON_EXECUTABLE=python3

set -e
cmake -G Ninja -B build/$KNL .
cd build/$KNL && ninja
