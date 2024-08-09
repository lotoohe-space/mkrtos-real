## MKRTOS 是什么

- MKRTOS 是第三代微内核操作系统，支持Cortex-A&Cortex-M核，支持动态加载app执行（MPU保护），支持ELF支持（MMU保护），多线程，进程隔离等功能。提供标准化的posix接口支持，用户态驱动，未来还将在支持虚拟化。MKRTOS适用于对安全与稳定性要求较高的场合。MKRTOS架构图如下：
 ![image](./mkrtos_doc/架构图.png) 
### 设计原则
- 1.服务间数据交互不经过第三方组件。
- 2.服务间最简接口原则。
- 3.内核最小功能原则。
- 4.微内核设计。
### MKRTOS 的目标
- 1.微内核设计的RTOS，用户态驱动、文件系统、网络协议栈。
- 2.移植方便快捷，内核只需要2个驱动程序支持（串口和systick）。
- 3.MCU&MPU处理器进程保护支持。
- 4.面向对象的系统设计。
### MKRTOS支持的处理器架构
| 处理器架构     | 支持的板子     | 进度     |
| -------- | -------- | -------- |
| Cortex-m3| renode/stm32f103,qemu/stm32f2 | done |
| Cortex-m4 | renode/stm32f4 | done |
| Cortex-m33|swm34s|done|
| ARMv8 | Qemu | done |
| risc-v | Qemu | X |
### MKRTOS特性支持
- 多ARCH支持（Cortex-M & Cortex-A)
- 多核支持
- 多核多进程支持，IPC支持
- 虚拟化支持
- musl lib（posix支持）
- path manager、process manager、init、fat server、shell
- MPU & MMU内存保护支持
- 可执行文件支持（bin&ELF）


### MKRTOS 用户态生态正在做
- 1.toybox常用命令支持
- 2.ota支持
- 3.ymodem支持
- 4.GUI支持。
- 5.lwip支持。
- 6.modubs支持。
- 7.can通信协议支持。
- 8.AT协议支持。
- 9.其它。

### 怎么使用？

- 工程采用CMake进行管理，并在Linux下进行开发，建议使用Ubuntu18.04开发。
- GCC编译器采用gcc-arm-none-eabi-5_4-2016q3，也可以使用新版本进行开发。
- Qemu模拟STM32F2，老版本的Qemu对coretx-m3的模拟存在bug，请使用最新版本Qemu8.0及以上。
使用步骤：
1. 安装CMake
```
sudo apt install cmake
```
2. 安装ninja
```
sudo apt update
sudo apt install ninja-build
sudo apt install cpio srecord
sudo apt install python3
sudo apt install srecord
```
3. 下载gcc
- 直接使用ubuntu命令安装的gcc arm会没有gdb，可以直接到我给定的仓库下载
```
https://gitee.com/IsYourGod/mkrtos-tools.git
64位系统使用32位gcc，需要安装
sudo apt install libc6-i386
```
4. Qemu
- 1.Qemu可以直接下载官方的进行编译
- 2.也可以从我给定的链接下载
```
https://gitee.com/IsYourGod/mkrtos-tools.git
```
5. 修改build.sh脚本中GCC路径和GCC库路径
```
export TOOLCHAIN=/home/zhangzheng/gcc-arm-none-eabi-5_4-2016q3/bin/
export TOOLCHAIN_LIB=/home/zhangzheng/gcc-arm-none-eabi-5_4-2016q3/lib/gcc/arm-none-eabi/5.4.1/armv7-m
```
6. 修改run.sh 和debug.sh中qemu的路径
```
qemu-system-arm -machine\
 	netduino2 -cpu cortex-m3 \
  	-nographic -m size=2\
   	-kernel $PWD/build/output/kernel.img \
    -S -gdb tcp::$1
```
7. 执行run.sh
输出如下：
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
### 如何交流

- 加群微信或者QQ交流。QQ群：419833232 

### 加入开发者

- 提交一次代码，并联系我。
