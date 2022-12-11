#!/bin/bash

# if [-z "$1"]; then
# 	echo "usage: ./debug.sh [tcp port]"
# 	exit
# fi

export MYDIR=$PWD

$PWD/tools/qemu-7.2.0-rc1/qemu-system-arm -machine\
 	netduino2 -cpu cortex-m3\
  	-nographic -m size=2\
   	-kernel $PWD/build/bin/kernel.img\
    -S -gdb tcp::$1
