//
// Created by Administrator on 2022/1/9.
//
#define __LIBRARY__
#include <unistd.h>
#include <arch/arch.h>
#include <mkrtos/sched.h>
#include <knl_service.h>
#include <mkrtos/fs.h>
#include <mkrtos/exec.h>

/**
 * @brief 在系统中删除当前执行的任务，该删除只是设置为僵尸进程
 */
void do_exit(int32_t exitCode) {
	//关所有中断
	uint32_t t;
	kprint("kill pid is %d.\n", get_current_task()->pid);
	if (get_current_task()->status == TASK_CLOSED) {
		return;
	}
	for (int i = 0; i < NR_FILE; i++) {
		if (get_current_task()->files[i].used) {
			sys_close(i);
		}
	}
	// mem_clear();
	do_remove_sleep_tim(get_current_task());

	t = dis_cpu_intr();
	//当前进程结束了，应该吧当前进程的子进程全部移交给初始化进程
	struct task *tmp;
	
	slist_foreach(tmp, &get_sys_tasks_info()->all_tk_list, all_node) {
		if (tmp->parent_task == get_current_task()) {
			tmp->parent_task = get_sys_tasks_info()->idle_task;
		}
	}
	//发送chld给父进程
	sig_chld(get_current_task());
	//唤醒等待这个队列关闭的
	wake_up(get_current_task()->close_wait);
	//设置任务已经关闭了
	get_current_task()->status = TASK_CLOSED;
	//减少一个就绪任务
	sys_tasks_cur_dec();
#if 0
    if(get_current_task()->exec) {
        (*(get_current_task()->exec->used_count))--;
        if(!((*get_current_task()->exec->used_count))) {
            unload_elf(get_current_task()->exec);
        }else{
//            if(!CUR_TASK->exec->clone_vm) {
                if(get_current_task()->clone_flag&CLONE_VM) {
                    //没有与父进程使用相同的内存空间，则需要释放这个空间
                    free(get_current_task()->exec->data.data);
                    free(get_current_task()->exec->bss.data);
                }
                //fork后的exec也是新的
                free(get_current_task()->exec);
//            }
        }
        get_current_task()->exec=NULL;
    }
#endif
	if (get_current_task()->user_ram) {
		//释放栈空间
		free_align(get_current_task()->user_ram);
	}
	if (get_current_task()->mpu) {
		free(get_current_task()->mpu);
	}
	if (get_current_task()->exec_id >= 0) {
		exec_put(get_current_task()->exec_id);
	}
	free_align(get_current_task()->knl_low_stack);
	get_current_task()->mem_low_stack = NULL;
	get_current_task()->knl_low_stack = NULL;
	get_current_task()->user_ram = NULL;
	task_sche();
//	#if MEM_TRACE
//	knl_mem_trace();
//	#endif
	get_sys_tasks_info()->wait_r++;
	restore_cpu_intr(t);
}
///**
// * @brief 任务结束时会调用该函数，任务执行结束，在这里销毁这个任务
// */
//void TaskToEnd(int32_t exitCode) {
//	/*这里需要通过系统调用，这个函数是用户层调用的*/
//	_exit(exitCode);
//}

int sys_exit(int exitCode) {
	/*删除当前任务*/
	inner_set_sig(SIGKILL);
	return 0;
}
