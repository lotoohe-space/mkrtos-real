#!/bin/bash

# export TOOLCHAIN=/home/zhangzheng/gcc-arm-none-eabi-5_4-2016q3/bin/
# export TOOLCHAIN_LIB=/home/zhangzheng/gcc-arm-none-eabi-5_4-2016q3/lib/gcc/arm-none-eabi/5.4.1/armv7-m
export TOOLCHAIN=/opt/homebrew/bin/
export TOOLCHAIN_LIB=/opt/homebrew/lib/gcc/arm-none-eabi/13.2.0/thumb
export KNL=mkrtos
export KEN_OFFSET=0x2000
export INIT_OFFSET=0x10000
export BOOTFS_ADDR_OFFSET=0x20000
export KNL_TEXT=0x8000000
export KNL_DATA=0x20000000
export KNL_DATA_SIZE=64K
export BOARD=STM32F2x

set -e
cmake -G Ninja -B build/$KNL .
cd build/$KNL && ninja
