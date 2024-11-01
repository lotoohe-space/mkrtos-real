#!/bin/bash

# export TOOLCHAIN=/home/zhangzheng/gcc-arm-none-eabi-5_4-2016q3/bin/
# export TOOLCHAIN_LIB=/home/zhangzheng/gcc-arm-none-eabi-5_4-2016q3/lib/gcc/arm-none-eabi/5.4.1/armv7e-m/fpu

# export TOOLCHAIN=/Users/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/bin/
# export TOOLCHAIN_LIB=/Users/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/lib/gcc/arm-none-eabi/10.3.1/thumb/v7e-m+fp/hard

export TOOLCHAIN=/d/GNUArmEmbeddedToolchain/102021.10/bin/
export TOOLCHAIN_LIB=/d/GNUArmEmbeddedToolchain/102021.10/lib/gcc/arm-none-eabi/10.3.1/thumb/v7e-m+fp/hard

# export TOOLCHAIN=/Users/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/bin/
# export TOOLCHAIN_LIB=/Users/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-m/nofp

# export TOOLCHAIN=/home/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/bin/
# export TOOLCHAIN_LIB=/home/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/lib/gcc/arm-none-eabi/10.3.1/thumb/v7e-m+fp/hard

# export TOOLCHAIN=/home/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/bin/
# export TOOLCHAIN_LIB=/home/zhangzheng/gcc-arm-none-eabi-10.3-2021.10/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-m/nofp

export BOARD=ATSURFF437
export CROSS_COMPILE_NAME=arm-none-eabi-

if [ -z "$1" ]; then
    export MKRTOS_TEST_MODE="normal"
else
    export MKRTOS_TEST_MODE=$1
fi

set -e
cmake -G Ninja -B build/$KNL .
cd build/$KNL && ninja
