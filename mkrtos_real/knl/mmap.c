/*
 * mmap.c
 *
 *  Created on: 2022年10月11日
 *      Author: zhangzheng
 */
#include <type.h>
#include <mm.h>
#include <mkrtos/sched.h>

void* sys_mmap(void *start, size_t length, int prot, int flags,int fd, off_t offset){
    struct inode *inode=NULL;
    int alloc_len=length;
    task_t *cur_task = get_current_task();

    if(start){
        return -1;
    }
    if(fd!=-1) {
        return -1;
    }
    void* res_mem = mem_alloc(&cur_task->user_head_alloc, alloc_len);

    if (!res_mem) {
    	return -1;
    }
    return res_mem;
}
void *sys_mmap2(void *start, size_t length, int prot, int flags,int fd, off_t offset) {
	return sys_mmap(start,length,prot,flags,fd, offset<<8);
}

void sys_munmap(void *start, size_t length){
    mem_free(&get_current_task()->user_head_alloc, start);
}

