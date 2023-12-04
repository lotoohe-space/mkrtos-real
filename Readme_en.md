##What is MKRTOS
-The full name of MKRTOS is Micro Kernel Real Time Operating System, and its Chinese name is Micro Kernel Real Time Operating System. MKRTOS is designed as an extremely streamlined kernel that only includes functions such as thread, task, and memory management. All other functions are implemented in user mode and are compatible with the Posix standard operating system, making it fully suitable for various embedded scenarios. The architecture diagram of MKRTOS is as follows:
! [image] (./mkrtos_doc/架构图.png)
###The goals of MKRTOS
-1 True microkernel design, suitable for industrial control, Internet of Things and other fields.
-2 Ready to use out of the box, supports multiple platforms.
-3 Porting is convenient and fast, and the kernel only requires support from two drivers (serial port and sysstick).
-4 Featuring high stability and high resistance to failure design.
-5 Minimalist design, supports multiple processes and threads, and supports microcontrollers.
-6 RTOS design with high real-time performance.
###Development background
-Microcontrollers are widely used in industries and the Internet of Things, and are suitable for operating systems such as rtthread and freertos. However, their functions are very simple, and using such RTOS comes with a certain learning cost (I have also written one, you can take a look at xTinyRTOS, which is very simple). The original intention of developing MKRTOS was to be compatible with POSIX to reduce learning costs, and during development, memory usage was also considered and optimized. Note: rtthread supports software packages, but there are also issues with architecture design and scalability.
###Design principles
-1. The data exchange between services does not go through third-party components.
-2. The principle of the simplest interface between services.
-3. The principle of minimum kernel functionality.
-4. Class L4 kernel (third-generation microkernel).
###MKRTOS kernel
####Done
-1. Processor: Supports Cortex-M3 processors by default
-2. Inter process communication adopts synchronous communication mechanism, and the kernel does not cache any data.
-3. The kernel only needs to support systick and serial ports to start the system.
-4. The kernel is written in C language and uses object-oriented programming, with all functions abstracted as kernel objects.
-5. Kernel support objects: thread objects, process objects, factory objects, memory management objects, Log objects, IPC objects.
-4. Multi threading: Supports multi-threaded management, separates threads from tasks, and is designed with rationality.
-5. Multi process (MPU achieves process isolation): Supports memory space and object space, and processes only manage resources.
-6. irq object support, used for forwarding kernel interrupts to user mode.
-7. IPC functions are complete.
####Todo
-1. Cortex-M processors from other series, RISC-V, and support from Loongson.
-2. Hardware floating-point support.
-3. Support MMU to achieve process isolation.
-4. Kernel futex support (used to implement user mode locks).
####Doing
-1. Improve the kernel
###Basic support for MKRTOS user mode
####Done
-1. Executable file: Supports bin executable files.
####Doing
-1. Musl libc library support.
-2. init process
-3. Shell services
-4. Path manager service
####Todo
-1. kconfig support.
-2. Support for Fat, Ext, and LittleFs series file systems
-3. DRV manager service
-4. Process Manager Services
-5. Character driven, block driven, display driven, and network driven support
###MKRTOS user ecosystem
-1. Toybox commonly used command support
-2. OTA support
-3. ymodem support
-4. GUI support.
-5. LWIP support.
-6. Modubs support.
-7. CAN communication protocol support.
-8. AT protocol support.
-9. Others.
###How to use it?
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
###My Blog
-Welcome to the blog for communication (domain name has not yet been applied for ^ - ^): [MKRTOS Blog]（ http://124.222.90.143/ ）
-Add groupQQ communication(QQ:1358745329).
###Logs
*A log from a long time ago
1. Dietlibc porting instructions, remove unnecessary CPU related folders in syscalls. h__ ARGS_ Change mmap to 1 and modify mmap S file.
2. Modify setjump S and other documents.
3. All The S file needs to add a compiled header.
4. LWIP porting
1. The implementation of the clone function needs to be completed.
2. It is necessary to complete semaphores, mutexes, and message mailboxes.
3. It is necessary to add a reference count to the struct socket to prevent problems when multiple processes occur. The socket, accept, and close functions need to be processed.
4. Some other configurations.
5. Other extensive modifications (I can't recall them).
6. Fixed some usage bugs on April 4, 2022, such as modifying privilege mode registers in user threads, resulting in direct exceptions.
*November 10, 2022
1. The test version of mkrtos has a relatively stable file system and added support for mpu protection. It isolates multiple applications and removes the previous elf execution method, directly executing the bin file format.
2. Ported the latest version of dietlbc.
3. Fixed many previous bugs.
*November 13, 2022
1. A prototype of a fast IPC communication mechanism without context switching, very fast!!!
2. Add fork_ The exec system call combines fork and exec, which is very suitable for microcontrollers and can save memory.
*November 14, 2022
1. Multiple applications directly reference mkrtos_ SDK project, remove duplicate files.
2. Fix the bug in the file system truncate.
*November 9, 2022
1. Change the project to Cmake management.
2. Increase QEMU support and use stm32f205rft6.
3. Increase bootstart support.
4. Increase support for the cpio file system.
5. Increase software floating-point support.
6. Add bidirectional linked lists and single linked lists
*August 31, 2023
1. System redesign, kernel updated to microkernel.