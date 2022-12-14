/*
 * main.c
 *
 *  Created on:
 *      Author: Administrator
 */

#include <mkrtos/sched.h>
#include <assert.h>
#include <mkrtos/fs.h>
#include <fcntl.h>
//微内核
//内核功能：进程管理，内存管理（带MPU），进程间通信(一定要牛逼，用户态驱动要用到)

//进程管理功能：
//1.调度器：O(1)，区分内核进程和用户进程。
//2.系统调用：内核态msp指针，用户态psp指针。
//3.明天搞mpu，elf动态加载
//4.应用程序直接加载bin文件执行
void thread_main(void *arg0, void *arg1) {
	int ret;
	sys_tasks_info.is_run = TRUE;
	mkrtos_test();
	// #if 0
	setup();
 	const char * const argv[] = {
			"/bin/zh"
			,0
	};
	sys_execve("/bin/zh",argv);
	// #endif
	kprint("done!\n");
	while(1) {
		sleep_ms(1000);
	}
}
// static void delay(void)
// {
// 	for (int i=0;i<1000;i++) {
// 		for (int j=0;j<10000;j++);
// 	}
// }
#include <stm32f2xx.h>
int main(void) {
	static struct task_create_par tcp;
	int32_t pid;
	extern int32_t bk_flash_init(void); 
	__enable_irq();
//	//TODO:增加栈8字节对齐
 	((uint8_t *)(0xE000E008))[0] |= 0x6;
// 	//TODO:关闭指令流水线？
//	((uint8_t *)(0xE000ED14))[0] |= 0x200;
	//初始化默认的磁盘设备
	root_dev_no = bk_flash_init();

	init_sche();

	tcp.task_fun = thread_main;
	tcp.arg0 = (void*) 0;
	tcp.arg1 = 0;
	tcp.prio = 6;
	tcp.exec_id = -1;
	tcp.user_stack_size = 0;
	tcp.kernel_stack_size = 384;
	tcp.task_name = "main";

	pid = task_create(&tcp, TRUE);
	MKRTOS_ASSERT(pid >= 0);

	sche_start();
	while (1)
		;
}

