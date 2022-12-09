#!/bin/bash

# if [-z "$1"]; then
# 	echo "usage: ./debug.sh [tcp port]"
# 	exit
# fi

/home/zhangzheng/qemu-7.2.0-rc1/build/qemu-system-arm -machine\
 	netduino2 -cpu cortex-m3\
  	-nographic -m size=2\
   	-kernel /home/zhangzheng/mkrtos/build/bin/kernel.img\
    -S -gdb tcp::$1
