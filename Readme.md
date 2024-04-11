## What is MKRTOS

- The full name of MKRTOS is Micro Kernel Real Time Operating System, and its Chinese name is Micro Kernel Real Time Operating System. MKRTOS is the first true micro kernel open-source operating system that supports MCU. The kernel only includes functions such as thread, task, and memory management, while other file systems and drivers are implemented in user mode. User mode supports dynamic loading of applications and is compatible with the Posix standard operating system, making it suitable for various embedded scenarios. The architecture diagram of MKRTOS is as follows:
 ![image](./mkrtos_doc/架构图.png) 
### The goals of MKRTOS
- 1. Real microkernel design, with high stability, high anti-failure design, suitable for industrial control, Internet of things and other fields.
- 2. Out of the box, supports multiple platforms.
- 3. Porting is convenient and fast, the kernel only needs 2 driver support (serial port and systick).
- 4. Simple design, support for multi-process (MPU process protection), multi-threading, support for MCU.
- 5. RTOS design, high real-time performance.
- 6. User mode drivers, which are also applications.
### MKRTOS supported processor architecture
| Processor architecture | Supported boards | Progress|
| -------- | -------- | -------- |
| Cortex-m3| renode/stm32f103,qemu/stm32f2 | done |
| Cortex-m4 | renode/stm32f4 | done |
| Cortex-m33|swm34s|done|
| ARMv8 | Qemu | done |
| risc-v | Qemu | X |

### Design principles
- 1. The data exchange between services does not go through third-party components.
- 2. The principle of the simplest interface between services.
- 3. The principle of minimum kernel functionality.
- 4. Microkernel design.
### MKRTOS module support
#### Completed
- 1. Processor: Cortex-M3 processor is supported by default, and more will be supported in the future
- 2. Inter process communication adopts synchronous communication mechanism, and the kernel does not cache any data.
- 3. The kernel only needs to support systick and serial ports to start the system.
- 4. The kernel is written in C language and uses object-oriented programming, with all functions abstracted as kernel objects.
- 5. Kernel support objects: thread objects, process objects, factory objects, memory management objects, Log objects, IPC objects, Futex objects.
- 4. Multi threading: Supports multi-threaded management, separates threads from tasks, and is designed with rationality.
- 5. Multi process (MPU achieves process isolation): Supports memory space and object space, and processes only manage resources.
- 6. irq object support, used for forwarding kernel interrupts to user mode.
- 7. Executable file: Supports bin executable files.
- 8. Musl libc library support.
- 9. Init process, process manager support, path manager support
- 10. Shell Services
- 11. kconfig support.
- 12. File system service support (temporary support for FATFS file system).
- 13. Cortex-m4 support, hardware floating-point support.
- 14. Cortex-m33 support, hardware floating-point support.
- 15. AARCH64 support.
#### Doing
- 1. Improve the kernel
- 2. DRV manager service
- 3. Ext and LittleFs series file system support
#### Future support
- 1. Cortex-M processors from other series, RISC-V, and support from Loongson.
- 2. Multi core MCU support
- 3. Virtualization support
- 4. Support MMU to achieve process isolation.
### MKRTOS user ecosystem is under development
- 1. Toybox commonly used command support
- 2. OTA support
- 3. ymodem support
- 4. GUI support.
- 5. LWIP support.
- 6. Modubs support.
- 7. CAN communication protocol support.
- 8. AT protocol support.
- 9. Others.

###H ow to use it?
- The project is managed using CMake and developed under Linux. It is recommended to use Ubuntu 18.04 for development.
- The GCC compiler uses gcc arm none eabi-5_4-2016q3, and can also be developed using new versions.
- Qemu simulates STM32F2. There is a bug in the simulation of coretx m3 in older versions of Qemu. Please use the latest version Qemu 8.0 or higher.
Usage steps:
1. Install CMake
```
sudo apt install cmake
```
2. Install ninja
```
sudo apt update
sudo apt install ninja-build
sudo apt install cpio srecord
sudo apt install python3
sudo apt install srecord
```
3. Download gcc
- The gcc arm installed directly using the ubuntu command will not have gdb, so you can download it directly from the repository I provided
```
https://gitee.com/IsYourGod/mkrtos-tools.git
A 64 bit system uses 32-bit gcc and requires installation
sudo apt install libc6-i386
```
4. Qemu
- 1. Qemu can directly download the official version for compilation
- 2. You can also download from the link I provided
```
https://gitee.com/IsYourGod/mkrtos-tools.git
```
5. Modify the GCC path and GCC library path in the build.sh script
```
export TOOLCHAIN=/home/zhangzheng/gcc-arm-none-eabi-5_4-2016q3/bin/
export TOOLCHAIN_LIB=/home/zhangzheng/gcc-arm-none-eabi-5_4-2016q3/lib/gcc/arm-none-eabi/5.4.1/armv7-m
```
6. Modify the path of qemu in run.sh and debug.sh
```
qemu-system-arm -machine\
 	netduino2 -cpu cortex-m3 \
  	-nographic -m size=2\
   	-kernel $PWD/build/output/kernel.img \
    -S -gdb tcp::$1
```
7. Execute run.sh
The output is as follows:
```
st:0x20000000 re:0xf sub:0x400
 region:[xxxxoooo]
st:0x20002000 re:0xf8 sub:0x400
 region:[oooxxxxx]
task alloc size is 6720, base is 0x20001000
exc_regs:8010001 20001db8 20001000
mkrtos init done..
mkrtos running..
 _____ ______   ___  __    ________  _________  ________  ________      
|\   _ \  _   \|\  \|\  \ |\   __  \|\___   ___\\   __  \|\   ____\     
\ \  \\\__\ \  \ \  \/  /|\ \  \|\  \|___ \  \_\ \  \|\  \ \  \___|_    
 \ \  \\|__| \  \ \   ___  \ \   _  _\   \ \  \ \ \  \\\  \ \_____  \   
  \ \  \    \ \  \ \  \\ \  \ \  \\  \|   \ \  \ \ \  \\\  \|____|\  \  
   \ \__\    \ \__\ \__\\ \__\ \__\\ _\    \ \__\ \ \_______\____\_\  \ 
    \|__|     \|__|\|__| \|__|\|__|\|__|    \|__|  \|_______|\_________\
                                                            \|_________|
Complie Time:Dec  2 2023 23:22:30
init..
ns svr init...
pm runing..
create thread 0x20002c00
exc_regs:8010b55 20001fb8 20001000
thread bind to 3
cons svr init...
fatfs addr is [0x8036048]
create task is 0x20002a5c
create thread 0x20003400
st:0x20000000 re:0x1 sub:0x8000
 region:[xooooooo]
st:0x20040000 re:0xff sub:0x8000
 region:[xxxxxxxx]
task alloc size is 140132, base is 0x20008000
thread bind to 12
exc_regs:8036049 20008d00 20008000
sh addr is [0x8076664]
create task is 0x20003b84
create thread 0x20004000
st:0x20004000 re:0x1 sub:0x800
 region:[xooooooo]
st:0x20008000 re:0xff sub:0x800
 region:[xxxxxxxx]
task alloc size is 9268, base is 0x20004800
thread bind to 14
exc_regs:8076665 20005f60 20004800
run app num is 2.
register svr, name is /mnt, hd is 16
register [/mnt] success.
mount success

mkrtos:/$ knl main run..

mkrtos:/$ 
```
### How to communicate
- Add group WeChat or QQ communication. QQ group: 419833232
### Joining developers
- Submit the code once and contact me.