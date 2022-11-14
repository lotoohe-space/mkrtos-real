//
// Created by Administrator on 2022/1/9.
//
#include <type.h>
#include "arch/arch.h"

#include <mkrtos/fs.h>
#include <mkrtos/sched.h>

extern int32_t sp_mkfs(dev_t dev_no, int32_t inode_count);
extern void fs_init(void);

#define IS_MKFS 1

int32_t setup(void) {
	int ret;

#if IS_MKFS
	ret =sp_mkfs(root_dev_no, 30);
	if (ret<0) {
		kfatal("root file system make fail.\n");
	}
#endif
	ret = root_mount(get_current_task());
	if (ret < 0) {
		ret =sp_mkfs(root_dev_no, -1);
		if (ret<0) {
			kfatal("root file system make fail.\n");
		}
		//ret = root_mount(get_current_task());
		//app_init();
		//app2_init();
	}
#if IS_MKFS
	fs_init();
#endif
	devs_init();
#if MKRTOS_USED_NET
#include "bsp/net_init.h"
#include "net/lwiperf_interface.h"
    lwip_comm_init();
    lwiperf_init();
#endif
#if IS_MKFS
	app_init();
	app2_init();
	sys_sync();
#endif
	return 0;
}
int32_t sys_setup(void) {
	return 0;
}
