//
// Created by Administrator on 2022/1/9.
//
#define __LIBRARY__
#include <unistd.h>
#include <arch/arch.h>
//#include "stdlib.h"
#include <mkrtos/sched.h>
#include <knl_service.h>
#include <mkrtos/fs.h>
#include <mkrtos/exec.h>
//#include <loader.h>

/**
 * @brief ��ϵͳ��ɾ����ǰִ�е����񣬸�ɾ��ֻ������Ϊ��ʬ����
 */
void do_exit(int32_t exitCode) {
	//�������ж�
	uint32_t t;
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
	//��ǰ���̽����ˣ�Ӧ�ðɵ�ǰ���̵��ӽ���ȫ���ƽ�����ʼ������
	struct task *tmp = get_sys_tasks_info()->all_task_list;
	while (tmp) {
		if (tmp->parent_task == get_current_task()) {
			tmp->parent_task = get_sys_tasks_info()->idle_task;
		}
		tmp = tmp->next_all;
	}
	//����chld��������
	sig_chld(get_current_task());
	//���ѵȴ�������йرյ�
	wake_up(get_current_task()->close_wait);
	//���������Ѿ��ر���
	get_current_task()->status = TASK_CLOSED;
	//����һ����������
	sys_tasks_cur_dec();
#if 0
    if(get_current_task()->exec) {
        (*(get_current_task()->exec->used_count))--;
        if(!((*get_current_task()->exec->used_count))) {
            unload_elf(get_current_task()->exec);
        }else{
//            if(!CUR_TASK->exec->clone_vm) {
                if(get_current_task()->clone_flag&CLONE_VM) {
                    //û���븸����ʹ����ͬ���ڴ�ռ䣬����Ҫ�ͷ�����ռ�
                    free(get_current_task()->exec->data.data);
                    free(get_current_task()->exec->bss.data);
                }
                //fork���execҲ���µ�
                free(get_current_task()->exec);
//            }
        }
        get_current_task()->exec=NULL;
    }
#endif
	if (get_current_task()->user_ram) {
		//�ͷ�ջ�ռ�
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
// * @brief �������ʱ����øú���������ִ�н����������������������
// */
//void TaskToEnd(int32_t exitCode) {
//	/*������Ҫͨ��ϵͳ���ã�����������û�����õ�*/
//	_exit(exitCode);
//}

int sys_exit(int exitCode) {
	/*ɾ����ǰ����*/
	inner_set_sig(SIGKILL);
	return 0;
}
