#!/bin/bash

# export TOOLCHAIN=/Applications/ArmGNUToolchain/11.3.rel1/aarch64-none-elf/bin/
# export TOOLCHAIN_LIB=/Applications/ArmGNUToolchain/11.3.rel1/aarch64-none-elf/lib/gcc/aarch64-none-elf/11.3.1
# export TOOLCHAIN=/home/toolchains/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin/
# export TOOLCHAIN_LIB=/home/toolchains/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/lib/gcc/aarch64-none-elf/10.3.1

export TOOLCHAIN=/home/zhangzheng/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin/
export TOOLCHAIN_LIB=/home/zhangzheng/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/lib/gcc/aarch64-none-elf/10.3.1

export BOARD=aarch64_qemu
export CROSS_COMPILE_NAME=aarch64-none-elf-
if [ -z "$1" ]; then
    export MKRTOS_TEST_MODE="normal"
else
    export MKRTOS_TEST_MODE=$1
fi

set -e -x
cmake -G Ninja -B build/$KNL .
cd build/$KNL && ninja
