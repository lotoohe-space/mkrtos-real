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
//΢�ں�
//�ں˹��ܣ����̹����ڴ������MPU�������̼�ͨ��(һ��Ҫţ�ƣ��û�̬����Ҫ�õ�)

//���̹����ܣ�
//1.��������O(1)�������ں˽��̺��û����̡�
//2.ϵͳ���ã��ں�̬mspָ�룬�û�̬pspָ�롣
//3.�����mpu��elf��̬����
//4.Ӧ�ó���ֱ�Ӽ���bin�ļ�ִ��
extern int app_init(void);
extern int app2_init(void);
void thread_main(void *arg0, void *arg1) {
	int ret;
	setup();
	const char * const argv[] = {
			"/bin/zh.bin"
			,0
	};
	sys_execve("/bin/zh.bin",argv);
	kprint("done!\n");
	while(1);
}
static void delay(void)
{
	for (int i=0;i<2000;i++) {
		for (int j=0;j<10000;j++);
	}
}
int main(void) {
	delay();
	extern int32_t bk_flash_init(void);
	extern int32_t sp_mkfs(dev_t dev_no, int32_t inode_count);

//	//TODO:����ջ8�ֽڶ���
 	((uint8_t *)(0xE000E008))[0] |= 0x6;
// 	//TODO:�ر�ָ����ˮ�ߣ�
//	((uint8_t *)(0xE000ED14))[0] |= 0x200;
	//��ʼ��Ĭ�ϵĴ����豸
	root_dev_no = bk_flash_init();

//	//�������ʽ���ļ�ϵͳ
//	if (sp_mkfs(root_dev_no, 30) < 0) {
//		kfatal("���ļ�ϵͳ����ʧ�ܣ�\r\n");
//	}

	static struct task_create_par tcp;
	int32_t pid;

	init_sche();

	tcp.task_fun = thread_main;
	tcp.arg0 = (void*) 0;
	tcp.arg1 = 0;
	tcp.prio = 6;
	tcp.exec_id = -1;
	tcp.user_stack_size = 0;
	tcp.kernel_stack_size = 256;
	tcp.task_name = "main";

	pid = task_create(&tcp, TRUE);
	MKRTOS_ASSERT(pid >= 0);

	sche_start();
	while (1)
		;
}

