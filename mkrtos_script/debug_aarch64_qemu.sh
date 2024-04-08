#!/bin/bash

if [ -z "$1" ]; then
	echo "usage: ./debug.sh [tcp port]"
	exit
fi

# -machine virt,virtualization=on,gic-version=2,highmem=off,secure=off,dumpdtb=virt.dtb
qemu-system-aarch64 \
	-machine virt,virtualization=on,gic-version=2,highmem=off,secure=off\
 	-cpu cortex-a53 \
  	-nographic \
	-m size=512 \
	-smp 4\
   	-kernel $PWD/build/output/bootstrap.elf \
    -S -gdb tcp::$1
