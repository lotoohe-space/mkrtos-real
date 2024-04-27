#!/bin/bash

# -machine virt,virtualization=on,gic-version=2,highmem=off,secure=off,dumpdtb=virt.dtb
# -display gtk,gl=on
# -drive file=$XXX,if=none,format=raw,id=hd0 -device virtio-blk-device,drive=hd0
# -device virtio-net-device,netdev=net0 -netdev user,id=net0,hostfwd=tcp::$2-:6666 -object filter-dump,id=f1, netdev=net0,file=dump.dat
qemu-system-aarch64 \
	-machine virt,virtualization=on,gic-version=2,highmem=off,secure=off \
	-device virtio-gpu-pci \
 	-cpu cortex-a53 \
  	-nographic \
	-m size=512 \
	-smp 4\
   	-kernel $PWD/build/output/bootstrap.elf
