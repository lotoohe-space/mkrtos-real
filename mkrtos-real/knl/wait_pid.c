//
// Created by zz on 2022/2/13.
//

#include <mkrtos/sched.h>
#include <arch/arch.h>
#include <knl_service.h>
#include <sys/wait.h>
/**
 * 清除队列
 * @param wake_tasks
 */
void clear_task_q(struct task **wake_tasks) {
	uint32_t t;
	t = dis_cpu_intr();
	while (*wake_tasks) {
		*wake_tasks = (*wake_tasks)->del_wait;
	}
	restore_cpu_intr(t);
}
void add_wait_task_q(struct task **queue, struct task *add_queue) {
	uint32_t t;
	t = dis_cpu_intr();
	if (*queue == NULL) {
		*queue = add_queue;
	} else {
		add_queue->del_wait = (*queue);
		*queue = add_queue;
	}
	restore_cpu_intr(t);
}
/**
 * 移除等待的任务
 * @param queue 需要移除的任务队列
 * @param tk 移除的任务
 * @param max_prio 返回等待任务的最大优先级，设置为NULL则不返回
 * @return
 */
//移除一个等待的
void remove_wait_task_q(struct task **queue, struct task *add_queue) {
	struct task *temp = *queue;
	struct task *prev = NULL;
	uint32_t t;
	if (!add_queue) {
		return;
	}
	t = dis_cpu_intr();
	while (temp) {
		if (temp == add_queue) {
			if (prev == NULL) {
				//删除的第一个
				*queue = temp->del_wait;
				break;
			} else {
				prev->del_wait = temp->del_wait;
				break;
			}
		}
		prev = temp;
		temp = temp->del_wait;
	}
	restore_cpu_intr(t);
}

//pid_t waitpid(pid_t pid,int *status,int options);
//成功：返回清理掉的子进程ID；失败：-1（无子进程）
//特殊参数和返回情况：
//参数pid：
//       >0 回收指定ID的子进程
//       -1 回收任意子进程（相当于wait）
//       0 回收和当前调用waitpid一个组的所有子进程
//       < -1 回收指定进程组内的任意子进程
//返回0：参数3为WNOHANG，且子进程正在运行
static void wait_task(struct wait_queue **wait_c) {
	uint32_t t;
	restore_cpu_intr(t);
	struct wait_queue wait = { get_current_task(), NULL };
	//如果这个进程还没有进入关闭状态，则等待关闭这个进程
	add_wait_queue(wait_c, &wait);
	task_suspend();
	task_sche();
	remove_wait_queue(wait_c, &wait);
	task_run();
}

pid_t do_sys_wait(pid_t pid, int32_t *statloc, int32_t options,
		struct rusage *rusage) {
	uint32_t t;
	struct task *ls;
	struct task *close_task = 0;
	uint32_t child_run_cn = 0;
	uint32_t child_all_cn = 0;
	again: child_run_cn = 0;
	child_all_cn = 0;
	t = dis_cpu_intr();
	ls = get_sys_tasks_info()->all_task_list;
	while (ls) {
		if (ls->parent_task == get_current_task()) { //保证必须是其子进程
			if (pid == -1) { //等待任一子进程
				if (ls->status == TASK_CLOSED) {
					int32_t res_pid;
					res_pid = shutdown_task(ls);
					restore_cpu_intr(t);
					if (statloc) {
						*statloc = ls->exit_code;
					}
					return res_pid;
				} else {
					//先添加到一个链表中，然后在结尾处进行判断
					add_wait_task_q(&close_task, ls);
					//运行中的子进程数量
					child_run_cn++;
				}
				//所有的子进程熟练
				child_all_cn++;

			} else if (pid > 0) { //等待其进程ID与pid相等的子进程
				if (ls->pid == pid) {
					if (ls->status == TASK_CLOSED) {
						int32_t res_pid;
						res_pid = shutdown_task(ls);
						if (statloc) {
							*statloc = ls->exit_code;
						}
						restore_cpu_intr(t);
						return res_pid;
					} else {
						if (options & WNOHANG) {
							//非阻塞
							return 0;
						} else {
							wait_task(&ls->close_wait);
							goto again;
						}
					}
				}
			} else if (pid == 0 || pid < -1) {
				int wait_pid;
				if (pid == 0) { //等待其组ID等于调用进程组ID的任一子进程
					wait_pid = get_current_task()->pgid;
				} else { //等待其组ID等于pid绝对值的任一子进程
					wait_pid = -pid;
				}
				if (ls->pgid == wait_pid) {
					if (ls->status == TASK_CLOSED) {
						int32_t res_pid;
						res_pid = shutdown_task(ls);
						if (statloc) {
							*statloc = ls->exit_code;
						}
						restore_cpu_intr(t);
						return res_pid;
					} else {
						//先添加到一个链表中，然后在结尾处进行判断
						add_wait_task_q(&close_task, ls);
						//运行中的子进程数量
						child_run_cn++;
					}
				} else {
					//所有的子进程熟练
					child_all_cn++;
				}
			}
		}
		ls = ls->next_all;
	}
	restore_cpu_intr(t);
	if (pid == -1 || pid < -1) {
		if (child_all_cn == 0) {
			//没有子进程，返回-1
			return -ECHILD;
		} else {
			//没有子进程被关闭
			if (child_all_cn == child_run_cn) {
				if (options & WNOHANG) {
					//非阻塞，清楚后，直接返回
					clear_task_q(&close_task);
					return 0;
				}
				pid_t pid = -1;
				//等待进程
				struct wait_queue wait_c = { get_current_task(), NULL };
				struct task *tmp;
				tmp = close_task;
				while (tmp) {
					//添加到等待队列
					add_wait_queue(&tmp->close_wait, &wait_c);
					tmp = tmp->del_wait;
				}

				//开始等待
				task_suspend();
				//当前线程挂起
				task_sche();
				uint8_t find = FALSE;
				//说明某个进程被关闭了
				tmp = close_task;
				while (tmp) {                //移除所有的等待任务，并找到那个closed的进程
					struct task *del_wait_next;
					struct wait_queue **close_wait;
					del_wait_next = tmp->del_wait;
					close_wait = &tmp->close_wait;
					if (tmp->status == TASK_CLOSED && !find) {
						pid = tmp->pid;
						if (statloc) {
							*statloc = tmp->exit_code;
						}
						shutdown_task(tmp);
						find = TRUE;
					}
					remove_wait_queue(close_wait, &wait_c);
					tmp = del_wait_next;
				}
//                restore_cpu_intr(t);
				//清除队列
				clear_task_q(&close_task);
				task_run();
				return pid;
			}
		}
	}

	return -1;
}
pid_t sys_wait4(pid_t pid, int32_t *statloc, int32_t options,
		struct rusage *rusage) {
	return do_sys_wait(pid, statloc, options, rusage);
}
/**
 * 等待pid
 * @param pid
 * @param statloc
 * @param options
 * @return
 */
pid_t sys_waitpid(pid_t pid, int32_t *statloc, int32_t options) {
	return do_sys_wait(pid, statloc, options, NULL);
}
