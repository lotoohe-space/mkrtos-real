#!/bin/bash

# if [-z "$1"]; then
# 	echo "usage: ./debug.sh [tcp port]"
# 	exit
# fi

# qemu-system-aarch64 -machine none -cpu cortex-r52 \
#   	-nographic -m size=2\
#     -device;loader,file=$PWD/build/output/kernel.img,cpu-num=4
#     -S -gdb tcp::$1
   	# -kernel $PWD/build/output/kernel.img \
# qemu-system-aarch64 -nographic -M arm-generic-fdt -dtb zcu102-arm.dtb \
# -device loader,file=./zcu102_FSBL.elf,cpu-num=0 \
# -device loader,addr=0xfd1a0104,data=0x8000000e,data-len=4 \
# -drive file=$PWD/build/output/kernel.img,if=sd,format=raw,index=0 -boot mode=5 -D log

# qemu-system-arm -machine\
#  	netduinoplus2 -cpu cortex-m4 \
#   	-nographic -m size=2\
#    	-kernel $PWD/build/output/bootstrap.elf \
#     -S -gdb tcp::$1


qemu-system-arm -nographic -M none -cpu cortex-r52 \
-nographic -m size=2 \
-device loader,file=$PWD/build/output/bootstrap.elf,cpu-num=0 \
-S -gdb tcp::$1
