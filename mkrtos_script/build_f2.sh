#!/bin/bash

# export TOOLCHAIN=/home/zhangzheng/mkrtos-tools/gcc/gcc-arm-none-eabi-5_4-2016q3/bin/
# export TOOLCHAIN_LIB=/home/zhangzheng/mkrtos-tools/gcc/gcc-arm-none-eabi-5_4-2016q3/lib/gcc/arm-none-eabi/5.4.1/armv7-m/
export TOOLCHAIN=/Users/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/bin/
export TOOLCHAIN_LIB=/Users/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-m/nofp
export BOARD=STM32F205

set -e
cmake -G Ninja -B build/$KNL .
cd build/$KNL && ninja
