## What is MKRTOS
--MKRTOS full name is Micro-Kernel Real-Time Operating System, Chinese name is Micro-kernel real-time operating system. MKRTOS is the first open source operating system with true micro kernel supporting MCU. The kernel only contains thread, task, memory management and other functions, and other file systems and drivers are implemented in user mode. The MKRTOS architecture diagram is as follows:
! [image] (./mkrtos_doc/架构图.png)
### The goals of MKRTOS
-1 True microkernel design, suitable for industrial control, Internet of Things and other fields.
-2 Ready to use out of the box, supports multiple platforms.
-3 Porting is convenient and fast, and the kernel only requires support from two drivers (serial port and sysstick).
-4 Featuring high stability and high resistance to failure design.
-5 Minimalist design, supports multiple processes and threads, and supports microcontrollers.
-6 RTOS design with high real-time performance.
### Processor architectures supported by MKRTOS
| | processor architecture to support the board | | progress
| -------- | -------- | -------- |
| Cortex-m3| renode/stm32f103,qemu/stm32f2 | done |
| Cortex-m4 | renode/stm32f4 | done |
| Cortex-m33 | swm34s | done |
| risc-v | Qemu | X |
| ARMv8|Qemu|X
### Design principles
-1. The data exchange between services does not go through third-party components.
-2. The principle of the simplest interface between services.
-3. The principle of minimum kernel functionality.
-4. Class L4 kernel (third-generation microkernel).
### MKRTOS module support
#### Completed
- 1.Processors: Cortex-M3 processors are supported by default, more will be supported in the future
2.The kernel does not cache any data for inter-process communication by means of synchronous communication.
The kernel only needs to support systick and a serial port to boot the system.
The kernel is written in C and uses object-oriented programming, and functions are abstracted into kernel objects.
- 5. Kernel objects: thread object, process object, factory object, memory management object, Log object, IPC object, Futex object.
- 4. Multi-thread: support multi-thread management, thread and task are separated, and the design is reasonable.
- 5. Multi-process (MPU implementation process isolation) : Support memory space and object space, processes only manage resources.
irq object support for forwarding kernel interrupts to user mode.
- 7. Executables: bin executables are supported.
- 8.musl libc library support.
- 9.init process, process manager support, path manager support
- 10.shell services
- 11.kconfig support.
- 12. Filesystem service support (fatfs filesystem is temporarily supported).
- 13.Cortex-m4 support, hardware floating-point support.
#### in the works
- 1.Improve the kernel.
- 2.drv manager service
- 3.Ext and LittleFs filesystem support
#### Future support
- 1.Cortex-M other series processors, RISC-V, Godson support.
- 2.Multi-core MCU support
- 3. Virtualization support
- 4. Support MMU for process isolation.
### The MKRTOS user-mode ecosystem is in progress
- 1.toybox common command support
- 2.ota support
- 3.ymodem support
- 4.GUI support.
- 5.lwip support.
- 6.modubs support.
-7.can communication protocol support.
- 8.AT protocol support.
- 9. Others.
### How to use it?
-The project is managed using CMake and developed under Linux. It is recommended to use Ubuntu 18.04 for development.
-The GCC compiler uses gcc arm none eabi-5_ 4-2016q3, new versions can also be used for development.
-Qemu simulates STM32F2. There is a bug in the simulation of coretx m3 in older versions of Qemu. Please use the latest version Qemu 8.0 or higher.
Usage steps:
1. Install CMake
```
Sudo aptinstall cmake
```
2. Install ninja
```
Sudo apt. update
Sudo apt. install ninja build
```
3. Download gcc
-The gcc arm installed directly using the ubuntu command will not have gdb, so you can download it directly from the repository I provided
```
https://gitee.com/isyourgod/mkrtos-tools.git
```
4. Qemu
-1. Qemu can directly download the official version for compilation
-2. You can also download from the link I provided
```
https://gitee.com/isyourgod/mkrtos-tools.git
```
5. Modify the GCC path and GCC library path in the build.sh script
```
Export TOOLCHAIN=/home/zhangzheng/gcc arm none eabi-5_ 4-2016q3/bin/
Export TOOLCHAIN_ LIB=/home/zhangzheng/gcc arm none eabi-5_ 4-2016q3/lib/gcc/arm none eabi/5.4.1/armv7-m
```
6. Modify the path of qemu in run.sh and debug.sh
```
Qemu system arm machine\
Netduino2 CPU Cortex m3\
-Nographic - m size=2\
-Kernel $PWD/build/output/kernel. img\
-S gdb TCP:: $1
```
7. Execute run.sh
The output is as follows:
```
root@vm-4-4-ubuntu :/home/mkrtos real test/mkrtos real #/ Run.sh
St: 0x20000000 re: 0x3 sub: 0x1000
Region: [xxoooooooo]
St: 0x20008000 re: 0xff sub: 0x1000
Region: [xxxxxxxx]
Task alloc size is 17720, base is 0x20002000
Exc_ Regs: 8010001 20003a0 20002000
Mkrtos init done
Mkrtos running
 _____ ______   ___  __    ________  _________  ________  ________      
|\   _ \  _   \|\  \|\  \ |\   __  \|\___   ___\\   __  \|\   ____\     
\ \  \\\__\ \  \ \  \/  /|\ \  \|\  \|___ \  \_\ \  \|\  \ \  \___|_    
 \ \  \\|__| \  \ \   ___  \ \   _  _\   \ \  \ \ \  \\\  \ \_____  \   
  \ \  \    \ \  \ \  \\ \  \ \  \\  \|   \ \  \ \ \  \\\  \|____|\  \  
   \ \__\    \ \__\ \__\\ \__\ \__\\ _\    \ \__\ \ \_______\____\_\  \ 
    \|__|     \|__|\|__| \|__|\|__|\|__|    \|__|  \|_______|\_________\
                                                            \|_________|
Complie Time: Sep 16 2023 23:50:51
Init
```
### How to communicate

- Add group wechat or QQ communication. QQ group: 419833232

### Join the developers

- Submit the code once and contact me.