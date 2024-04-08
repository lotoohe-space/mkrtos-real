#!/bin/bash

# -machine virt,virtualization=on,gic-version=2,highmem=off,secure=off,dumpdtb=virt.dtb
qemu-system-aarch64 \
	-machine virt,virtualization=on,gic-version=2,highmem=off,secure=off\
 	-cpu cortex-a53 \
  	-nographic \
	-m size=512 \
	-smp 4\
   	-kernel $PWD/build/output/bootstrap.elf
