#!/bin/bash

export TOOLCHAIN=/home/zhangzheng/gcc-arm-10.3-2021.07-aarch64-aarch64-none-elf/bin/
export TOOLCHAIN_LIB=/home/zhangzheng/gcc-arm-10.3-2021.07-aarch64-aarch64-none-elf/lib/gcc/aarch64-none-elf/10.3.1
export BOARD=aarch64_qemu
export CROSS_COMPILER=aarch64-none-elf-

set -e
cmake -G Ninja -B build/$KNL .
cd build/$KNL && ninja
