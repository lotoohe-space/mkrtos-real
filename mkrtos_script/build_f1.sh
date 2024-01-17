#!/bin/bash

# export TOOLCHAIN=/home/ATShining/gcc-arm-none-eabi-5_4-2016q3/bin/
# export TOOLCHAIN_LIB=/home/ATShining/gcc-arm-none-eabi-5_4-2016q3/lib/gcc/arm-none-eabi/5.4.1/armv7-m
export TOOLCHAIN=/Users/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/bin/
export TOOLCHAIN_LIB=/Users/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-m/nofp
export KEN_OFFSET=0x2000
export INIT_OFFSET=0x10000
export BOOTFS_ADDR_OFFSET=0x22000
export KNL_DATA=0x20000000
export CPU_TYPE=STM32
export BOARD=STM32F1x
export ARCH=cortex-m3

set -e
cmake -G Ninja -B build/$KNL .
cd build/$KNL && ninja
