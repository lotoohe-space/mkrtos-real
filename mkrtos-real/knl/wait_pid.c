//
// Created by zz on 2022/2/13.
//

#include <mkrtos/sched.h>
#include <arch/arch.h>
#include <knl_service.h>
#include <sys/wait.h>
/**
 * �������
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
 * �Ƴ��ȴ�������
 * @param queue ��Ҫ�Ƴ����������
 * @param tk �Ƴ�������
 * @param max_prio ���صȴ������������ȼ�������ΪNULL�򲻷���
 * @return
 */
//�Ƴ�һ���ȴ���
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
				//ɾ���ĵ�һ��
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
//�ɹ���������������ӽ���ID��ʧ�ܣ�-1�����ӽ��̣�
//��������ͷ��������
//����pid��
//       >0 ����ָ��ID���ӽ���
//       -1 ���������ӽ��̣��൱��wait��
//       0 ���պ͵�ǰ����waitpidһ����������ӽ���
//       < -1 ����ָ���������ڵ������ӽ���
//����0������3ΪWNOHANG�����ӽ�����������
static void wait_task(struct wait_queue **wait_c) {
	uint32_t t;
	restore_cpu_intr(t);
	struct wait_queue wait = { get_current_task(), NULL };
	//���������̻�û�н���ر�״̬����ȴ��ر��������
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
		if (ls->parent_task == get_current_task()) { //��֤���������ӽ���
			if (pid == -1) { //�ȴ���һ�ӽ���
				if (ls->status == TASK_CLOSED) {
					int32_t res_pid;
					res_pid = shutdown_task(ls);
					restore_cpu_intr(t);
					if (statloc) {
						*statloc = ls->exit_code;
					}
					return res_pid;
				} else {
					//����ӵ�һ�������У�Ȼ���ڽ�β�������ж�
					add_wait_task_q(&close_task, ls);
					//�����е��ӽ�������
					child_run_cn++;
				}
				//���е��ӽ�������
				child_all_cn++;

			} else if (pid > 0) { //�ȴ������ID��pid��ȵ��ӽ���
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
							//������
							return 0;
						} else {
							wait_task(&ls->close_wait);
							goto again;
						}
					}
				}
			} else if (pid == 0 || pid < -1) {
				int wait_pid;
				if (pid == 0) { //�ȴ�����ID���ڵ��ý�����ID����һ�ӽ���
					wait_pid = get_current_task()->pgid;
				} else { //�ȴ�����ID����pid����ֵ����һ�ӽ���
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
						//����ӵ�һ�������У�Ȼ���ڽ�β�������ж�
						add_wait_task_q(&close_task, ls);
						//�����е��ӽ�������
						child_run_cn++;
					}
				} else {
					//���е��ӽ�������
					child_all_cn++;
				}
			}
		}
		ls = ls->next_all;
	}
	restore_cpu_intr(t);
	if (pid == -1 || pid < -1) {
		if (child_all_cn == 0) {
			//û���ӽ��̣�����-1
			return -ECHILD;
		} else {
			//û���ӽ��̱��ر�
			if (child_all_cn == child_run_cn) {
				if (options & WNOHANG) {
					//�������������ֱ�ӷ���
					clear_task_q(&close_task);
					return 0;
				}
				pid_t pid = -1;
				//�ȴ�����
				struct wait_queue wait_c = { get_current_task(), NULL };
				struct task *tmp;
				tmp = close_task;
				while (tmp) {
					//��ӵ��ȴ�����
					add_wait_queue(&tmp->close_wait, &wait_c);
					tmp = tmp->del_wait;
				}

				//��ʼ�ȴ�
				task_suspend();
				//��ǰ�̹߳���
				task_sche();
				uint8_t find = FALSE;
				//˵��ĳ�����̱��ر���
				tmp = close_task;
				while (tmp) {                //�Ƴ����еĵȴ����񣬲��ҵ��Ǹ�closed�Ľ���
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
				//�������
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
 * �ȴ�pid
 * @param pid
 * @param statloc
 * @param options
 * @return
 */
pid_t sys_waitpid(pid_t pid, int32_t *statloc, int32_t options) {
	return do_sys_wait(pid, statloc, options, NULL);
}
