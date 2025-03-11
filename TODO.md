

## TODO list
### high prio
* [x] APPFS 在flash中执行，能够支持app直接运行的文件系统。
* [x] 完善mpu缺页模拟支持，红黑树管理，支持整块或者分块缺页支持。
* [x] 内存管理对象mpu支持完善（在缺页时自动映射）。
* [x] 一种新的ipc机制（暂时取名fastipc），不切上下文，只切内存，流程更加简单，解决原来ipc优先级问题，以及并发性问题。
* [x] cpu占用率支持
* [x] 进程管理机制完善，进程状态订阅，进程间信号发送。
* [x] TTY 支持自动切换前后台
* [x] 内核信号量对象完善,支持超时（已完成）。
* [x] 重构路径管理（nsfs）
* [x] 去除原来的ipc机制（使用fastipc机制），并单独实现sleep接口，目前的ipc有概率卡死问题
* [x] TTY驱动支持
* [x] 内核二值信号量支持优先级反转（优先级继承协议）。
* [ ] FPU完整支持，fastipc FPU支持
* [ ] 文件系统&网络协议栈完善自动删除支持(文件描述符自动管理库)
* [ ] 线程占用率统计
* [ ] procfs支持
* [ ] 新进程中env支持
* [ ] dup, dup2等接口支持
* [ ] posix mq支持
* [ ] posix sig支持
* [ ] posix shm支持
* [ ] posix sema支持
* [ ] vfork + exec实现
* [ ] 几大组件稳定性测试
### mid prio
* [x] net server support
* [x] block driver
* [x] display driver
* [x] eth driver
* [x] i2c driver
* [x] pca9555 driver
* [x] pin drvier
* [x] snd drvier
* [ ] ymodem support
* [x] vi support
### low prio
- [ ] toybox support
- [ ] ota support
- [ ] lvgl support
- [ ] modbus support
- [ ] poll/select实现
- [ ] 零星调度支持

#### must low prio
- [ ] AT proctol support
- [ ] can proctol support
- [ ] dyn so support
- [ ] mrpoid 适配
