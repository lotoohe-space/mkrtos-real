//
// Created by Administrator on 2022/1/9.
//

#include <type.h>
#include <errno.h>
#include <string.h>
#include <knl_service.h>
#include <mkrtos/sched.h>
#include <mkrtos.h>

#define CUR_TASK sys_tasks_info.current_task

/**
 * @brief ϵͳ�����
 */
struct sys_tasks sys_tasks_info = { 0 };

/**
 * @brief ���ϵͳ������Ϣ
 */
struct sys_tasks* get_sys_tasks_info(void) {
	return &sys_tasks_info;
}
/**
 * @brief ϵͳ�����һ����ǰ����ɾ����
 */
void sys_tasks_cur_dec(void) {
	sys_tasks_info.current_max_task_node->task_ready_count--;
	if (sys_tasks_info.current_max_task_node->task_ready_count == 0) {
		//�������
		task_update_cur();
	}
}
/**
 * @brief ͨ��PID�ҵ��������
 */
struct task* task_find(int32_t pid) {
	struct task *pstl;
	if (pid < 0 || pid == 0) {
		return sys_tasks_info.current_task;
	}
	pstl = sys_tasks_info.all_task_list;
	while (pstl) {
		if (pstl->pid == pid) {
			return pstl;
		}
		pstl = pstl->next_all;
	}
	return NULL;
}
//�������񲻿ɴ��
void task_unintr(void) {
	if (CUR_TASK->status != TASK_UNINTR && CUR_TASK->status != TASK_CLOSED) {
		CUR_TASK->status = TASK_UNINTR;
	}
}
/**
 * @brief �������
 */
void task_suspend(void) {
	if (
	CUR_TASK->status != TASK_SUSPEND && CUR_TASK->status != TASK_CLOSED
			&& CUR_TASK->status != TASK_UNINTR) {
		sys_tasks_cur_dec();
		CUR_TASK->status = TASK_SUSPEND;
	}
}
/**
 * @brief ���������У������½���״̬
 */
void task_run(void) {
	if (CUR_TASK->status != TASK_RUNNING && CUR_TASK->status != TASK_CLOSED) {
		if (CUR_TASK->status == TASK_SUSPEND) {
			sys_tasks_info.current_max_task_node->task_ready_count++;
		}
		CUR_TASK->status = TASK_RUNNING;
	}
}
/**
 * @brief �õ�ǰ�����������״̬������������״̬
 */
void task_run_1(struct task *tk) {
	if (!tk) {
		return;
	}
	if (tk->status != TASK_RUNNING && tk->status != TASK_CLOSED) {
		if (tk->status == TASK_SUSPEND) {
			tk->parent->task_ready_count++;
			if (tk->prio
					> sys_tasks_info.current_max_task_node->task_priority) {
				sys_tasks_info.current_max_task_node = tk->parent;
				//task_update_cur();
			}
		}
		tk->status = TASK_RUNNING;
	}
}
/**
 * ��������
 */
void sche_lock(void) {
	atomic_inc(&(sys_tasks_info.sche_lock));
}
/**
 * ����������
 */
void sche_unlock(void) {
	atomic_test_dec_nq(&(sys_tasks_info.sche_lock));
}
/**
 * ��ȡ��������������
 */
uint32_t get_sche_lock_cn(void) {
	return atomic_read(&(sys_tasks_info.sche_lock));
}

/**
 * @brief ������ȣ�������ȹرգ��������Ч
 */
void task_sche(void) {
	uint32_t t;
	uint32_t need_sw = 0;

	if (sys_tasks_info.current_max_task_node->task_ready_count == 0) {
		return;
	}
	//����Ƿ���Ե���
	if (atomic_test(&(sys_tasks_info.sche_lock), FALSE)) {
		return;
	}
	t = dis_cpu_intr();

	if (sys_tasks_info.is_first == FALSE) {
		struct task *ptb = sys_tasks_info.current_max_task_node->pSysTaskLinks;
		while (ptb) {
			if (ptb->status != TASK_RUNNING) {
				ptb = ptb->next;
				continue;
			}

			sys_tasks_info.current_task = ptb;
			sys_tasks_info.is_first = TRUE;
			need_sw = 1;
			break;
		}
	} else {
		sys_tasks_info.last_task = sys_tasks_info.current_task;
		/*��ǰ����û�д���TASK_UNINTR״̬�ſ��Խ����л�����Ȼ���ܹ���*/
		if (CUR_TASK->status != TASK_UNINTR) {
			/*֮ǰ�������ֱ������һ����Ч��*/
			struct task *ptb;
			ptb = sys_tasks_info.current_task->next;
			do {
				if (ptb == NULL) {
					ptb = sys_tasks_info.current_max_task_node->pSysTaskLinks;
				}
				if (ptb->status == TASK_RUNNING) {
					if (ptb != sys_tasks_info.current_task) {
						need_sw = 1;
					}
					sys_tasks_info.current_task = ptb;
					break;
				}
				ptb = ptb->next;
			} while (1);
		}
	}
	if (need_sw) {
		to_schedule();
	}
	restore_cpu_intr(t);
}
/**
 * @brief ���¾�����������ȼ�������ڵ㣬���µ������о�������������0
 * @param pSysTasks ����������
 */
void task_update_cur(void) {
	struct sys_task_base_links *ptl;
	uint32_t t = dis_cpu_intr();

	ptl = sys_tasks_info.sys_task_base_links.next;

	/*������ȼ�*/
	struct sys_task_base_links *max_prio_task = NULL;

	while (ptl) {
		if (ptl->task_ready_count > 0) {
			if (max_prio_task == NULL && ptl->task_ready_count > 0) {
				max_prio_task = ptl;
			} else {
				if (ptl->task_priority > max_prio_task->task_priority
				//	&& ptl->task_ready_count>0
						) {
					/*����*/
					max_prio_task = ptl;
				}
			}
		}
		ptl = ptl->next;
	}
	sys_tasks_info.current_max_task_node = max_prio_task;

	restore_cpu_intr(t);
}

/**
 * @brief ͨ�����ȼ��ҵ�������
 * @param prio �������ȼ�
 * @return ������ȼ�����ı�ͷ
 */
static struct sys_task_base_links* find_task_links(uint8_t prio) {
	struct sys_task_base_links *ptl;
	ptl = sys_tasks_info.sys_task_base_links.next;
	while (ptl) {
		if (ptl->task_priority == prio) {
			return ptl;
		}
		ptl = ptl->next;
	}
	return NULL;
}
/**
 * @brief ͨ�����ȼ��������ͷ�����ָ�����ȼ������ڣ��򴴽�һ�����ȼ��Ľڵ�
 * @param prio ��ӵ����ȼ�
 * @return ��ӳɹ�������ͷ
 */
static struct sys_task_base_links* add_links(uint8_t prio) {

	struct sys_task_base_links *p_sys_task_base_links =
			(struct sys_task_base_links*) malloc(
					sizeof(struct sys_task_base_links));
	if (p_sys_task_base_links == NULL) {
		return NULL;
	}
	p_sys_task_base_links->next = NULL;
	p_sys_task_base_links->pSysTaskLinks = NULL;
	p_sys_task_base_links->task_count = 0;
	p_sys_task_base_links->task_priority = prio;
	p_sys_task_base_links->task_ready_count = 0;

	/*�������*/
	p_sys_task_base_links->next = sys_tasks_info.sys_task_base_links.next;
	sys_tasks_info.sys_task_base_links.next = p_sys_task_base_links;

	return p_sys_task_base_links;
}
/**
 * ɾ������
 * @param del
 */
void del_task(struct task **task_ls, struct task *del, int flag) {
	struct sys_task_base_links *taskLinks;
	uint32_t t;
	t = dis_cpu_intr();
	if (!task_ls) {
		taskLinks = find_task_links(del->prio);
		if (taskLinks == NULL) {
			restore_cpu_intr(t);
			return;
		}
		task_ls = &(taskLinks->pSysTaskLinks);
	}
	struct task *pTemp = *task_ls;
	struct task *lastP = NULL;
	while (pTemp) {
		if (del == pTemp) {
			if (lastP == NULL) {
				if (!flag) {
					*task_ls = pTemp->next;
				} else {
					*task_ls = pTemp->next_all;
				}
				break;
			} else {
				if (!flag) {
					lastP->next = pTemp->next;
				} else {
					lastP->next_all = pTemp->next_all;
				}
				break;
			}
		}
		lastP = pTemp;
		pTemp = pTemp->next;
	}
	restore_cpu_intr(t);
	task_update_cur();
}
/**
 * @brief ͨ�����ȼ�����������������ȼ������ڣ��򴴽������ȼ�������ڵ�
 * @param pSysTasks ����������
 * @return ����Ƿ�ɹ�
 */
int32_t add_task(struct task *p_task_block, uint32_t into_all_ls) {
	/*�������ж�*/
	uint32_t t;
	struct sys_task_base_links *taskLinks;
	if (p_task_block == NULL) {
		return -1;
	}

	t = dis_cpu_intr();
	taskLinks = find_task_links(p_task_block->prio);
	if (taskLinks == NULL) {
		taskLinks = add_links(p_task_block->prio);
		if (taskLinks == NULL) {
			restore_cpu_intr(t);
			/*û���ڴ�*/
			return -ENOMEM;
		}
	}
	taskLinks->task_count++;
	taskLinks->task_ready_count++;

	//�ŵ�ͬ���ȼ�������������
	struct task *pstl = taskLinks->pSysTaskLinks;
	if (pstl == NULL) {
		p_task_block->next = NULL;
		taskLinks->pSysTaskLinks = p_task_block;
	} else {
		/*���������ͷ*/
		p_task_block->next = pstl;
		taskLinks->pSysTaskLinks = p_task_block;
	}
	p_task_block->parent = taskLinks;

	if (into_all_ls) {
		//�浽���������������
		pstl = sys_tasks_info.all_task_list;
		if (pstl == NULL) {
			p_task_block->next_all = NULL;
			sys_tasks_info.all_task_list = p_task_block;
		} else {
			/*���������ͷ*/
			p_task_block->next_all = pstl;
			sys_tasks_info.all_task_list = p_task_block;
		}
	}
	restore_cpu_intr(t);

	//�������ȼ�
	if (sys_tasks_info.current_max_task_node != NULL
			&& p_task_block->prio
					> sys_tasks_info.current_max_task_node->task_priority) {
		task_update_cur();
	} else if (sys_tasks_info.current_max_task_node == NULL) {
		task_update_cur();
	}
	return 0;
}

/**
 * @brief ��������м��
 */
void tasks_check(void) {
	struct task *ptb;
	sys_tasks_info.sys_run_count++;
#if 1
    //������еĶ�ʱ���ź�
    ptb=sys_tasks_info.all_task_list;
    while(ptb){
        if(ptb->status==TASK_RUNNING
        && ptb->alarm){
            //ʱ�䵽��
            if(ptb->alarm<sys_tasks_info.sys_run_count){
                //����ָ���ź�
                inner_set_sig(SIGALRM);
                ptb->alarm = 0;
            }
        }
        /*else if(ptb->status==TASK_SUSPEND){
            //SIGCONT�ź������ﴦ��
            if (ptb->signalBMap & (1 << (SIGCONT - 1))) {
                //����յ����ź���SIGCONT�����Ҵ�ʱ������ͣ�ˣ�������
                ptb->status=TASK_RUNNING;
            }
        }
         */
//next_all:
        ptb=ptb->next_all;
    }
#endif
}
/**
 * @brief �������ȼ�
 */
int32_t task_change_prio(struct task *tk, int32_t new_prio) {
	uint32_t t;
	uint32_t old_prio;
	struct sys_task_base_links *base_links;
	if (!tk) {
		return -1;
	}
	if (tk->prio == new_prio) {
		return 0;
	}
	t = dis_cpu_intr();
	base_links = CUR_TASK->parent;
	old_prio = CUR_TASK->prio;
	CUR_TASK->prio = new_prio;
	if (add_task(CUR_TASK, 0) < 0) {
		//û���㹻���ڴ��ˣ��ָ�֮ǰ��
		CUR_TASK->prio = old_prio;
		restore_cpu_intr(t);
		return -ENOMEM;
	}
	base_links->task_ready_count--;
	base_links->task_count--;
	CUR_TASK->prio = old_prio;
	del_task(NULL, CUR_TASK, 0);
	CUR_TASK->prio = new_prio;
	restore_cpu_intr(t);
	return 0;
}

/**
 * @brief ִ��������ȣ�������һ�������ջ��
 * @brief SP ��ǰ�̵߳�ջ����ַ
 * @brief SPType ʹ�õ���MSP����PSP 0ʹ��msp 1ʹ��psp
 * @return ����ջ��
 */
struct stack_info* sys_task_sche(void *psp, void *msp, uint32_t sp_type) {
	if (sys_tasks_info.last_task) {
		/*�޸ĵ�ǰ�̵߳�ջ����ַ*/
		sys_tasks_info.last_task->sk_info.user_stack = psp;
		sys_tasks_info.last_task->sk_info.knl_stack = msp;
		if (sys_tasks_info.last_task->sk_info.stack_type != 2) {
			sys_tasks_info.last_task->sk_info.stack_type = sp_type;
		}
	}
	sys_tasks_info.current_task->run_count++;
	/*���ض�ջ�ĵ�ַ*/
	return &(sys_tasks_info.current_task->sk_info);
}

// ϵͳ���ù��� - ���ñ�����ʱʱ��ֵ(��)��
// ����Ѿ����ù�alarm ֵ���򷵻ؾ�ֵ�����򷵻�0��
int32_t sys_alarm(uint32_t seconds) {
	uint32_t old = 0;
	if (sys_tasks_info.current_task->alarm) {
		old = sys_tasks_info.current_task->alarm;
	}
	sys_tasks_info.current_task->alarm = sys_tasks_info.sys_run_count
			+ (seconds * 1000) / (1000 / OS_WORK_HZ);
	return old;
}
// ������̵ȴ��ź�
int32_t sys_pause(void) {
	task_suspend();
	task_sche();
	return 0;
}

//���Ѷ��������е�����
void wake_up(struct wait_queue *queue) {
	uint32_t t;
	t = dis_cpu_intr();
	while (queue) {
		if (queue->task) {
			if (queue->task->status == TASK_SUSPEND
					|| queue->task->status == TASK_UNINTR) {
				task_run_1(queue->task);
			}
		}
		queue = queue->next;
	}
	restore_cpu_intr(t);
}
void wake_up_dont_lock(struct wait_queue *queue) {
	while (queue) {
		if (queue->task) {
			if (queue->task->status == TASK_SUSPEND
					|| queue->task->status == TASK_UNINTR) {
				task_run_1(queue->task);
			}
		}
		queue = queue->next;
	}
}


//���һ�����ȴ�������
void add_wait_queue(struct wait_queue **queue, struct wait_queue *add_queue) {
	uint32_t t;
	t = dis_cpu_intr();
	if (*queue == NULL) {
		*queue = add_queue;
	} else {
		add_queue->next = (*queue);
		*queue = add_queue;
	}
	restore_cpu_intr(t);
}
//�ҵ�һ��queue������ȡ������ȼ�
struct wait_queue* find_wait_queue(struct wait_queue **queue, struct task *tk,
		uint32_t *max_prio) {
	struct wait_queue *temp = *queue;
	struct wait_queue *res = NULL;
	uint32_t t;
	t = dis_cpu_intr();
	if (max_prio) {
		*max_prio = 0;
	}
	while (temp) {
		if (temp->task->status != TASK_CLOSED) {
			if (temp->task == tk) {
				res = temp;
			}
			if (max_prio) {
				if (temp->task->prio > *max_prio) {
					*max_prio = temp->task->prio;
				}
			}
		}
		temp = temp->next;
	}
	restore_cpu_intr(t);
	return res;
}
//�Ƴ�һ���ȴ���
void remove_wait_queue(struct wait_queue **queue, struct wait_queue *add_queue) {
	struct wait_queue *temp = *queue;
	struct wait_queue *prev = NULL;
	uint32_t t;
	if (!add_queue) {
		return;
	}
	t = dis_cpu_intr();
	while (temp) {
		if (temp == add_queue) {
			if (prev == NULL) {
				//ɾ���ĵ�һ��
				*queue = temp->next;
				break;
			} else {
				prev->next = temp->next;
				break;
			}
		}
		prev = temp;
		temp = temp->next;
	}
	restore_cpu_intr(t);
}

/**
 * @brief ������ǰ�������ȼ�
 * @param increment
 * @return
 */
int32_t sys_nice(int32_t increment) {
	int new_prio;
	new_prio = CUR_TASK->prio;
	if (new_prio - increment > 0)
		new_prio -= increment;

	if (task_change_prio(CUR_TASK, new_prio) <= 0) {
		return -ESRCH;
	}
	return 0;
}

/**
 * ����ʱϵͳ��������⺯��
 * ���ں��߳�ʱ��������������û�̬
 * �ں��߳�ʱ���������ɾ���߳�
 */
void task_end(void) {
	sys_exit(0);
}
/**
 * @brief ��������
 * @param tcp ���񴴽�����
 * @param progInfo ����Ĵ�����Ϣ
 * @return
 */
int32_t task_create(struct task_create_par *tcp) {
	task_fun task_fun;
	void *arg0;
	void *arg1;
	uint8_t prio;
	uint16_t user_stack_size;
	uint16_t kernel_size;
	const char *task_name;
	struct sys_tasks *sys_tasks_info;

	if (tcp == NULL) {
		return -1;
	}

	task_fun = tcp->task_fun;
	arg0 = tcp->arg0;
	arg1 = tcp->arg1;
	prio = tcp->prio;
	user_stack_size = tcp->user_stack_size;
	kernel_size = tcp->kernel_stack_size;
	task_name = tcp->task_name;

	if (task_fun == NULL || kernel_size == 0) {
		return -EINVAL;
	}
	void *mem_stack = NULL;
	void *knl_mem_stack = NULL;
	struct task *p_task_block = NULL;
	p_task_block = (struct task*) malloc(sizeof(struct task));
	if (p_task_block == NULL) {
		return -ENOMEM;
	}
	mkrtos_memset(p_task_block, 0, sizeof(struct task));

	//����task name
	mkrtos_strncpy(p_task_block->task_name, task_name, THREAD_NAME_LEN);
	p_task_block->task_name[THREAD_NAME_LEN - 1] = 0;

	if (user_stack_size) {
		/*�����ջ���ڴ�*/
		mem_stack = (void*) malloc(sizeof(uint32_t) * (user_stack_size));
		if (mem_stack == NULL) {
			free(p_task_block);
			return -ENOMEM;
		}
	} else {
		user_stack_size = 0;
	}
	knl_mem_stack = (void*) malloc(sizeof(uint32_t) * (kernel_size));
	if (knl_mem_stack == NULL) {
		free(p_task_block);
		free(mem_stack);
		return -ENOMEM;
	}

	sys_tasks_info = get_sys_tasks_info();
	/*��ʼ��������Ʊ�*/
	p_task_block->run_count = 0;
	p_task_block->pid = (pid_t) atomic_read(&sys_tasks_info->pid_temp);
//    p_task_block->tpid=-1;
	if (atomic_test(&sys_tasks_info->pid_temp, 1)) {
		sys_tasks_info->idle_task = p_task_block;
	}

	p_task_block->mem_low_stack = mem_stack;
	if (user_stack_size != 0) {
		p_task_block->sk_info.user_stack =
				(ptr_t) (&(((uint32_t*) mem_stack)[user_stack_size - 1]))
						& (~0x7);
	} else {
		p_task_block->sk_info.user_stack = (void*) (~(0L));
	}
	p_task_block->knl_low_stack = knl_mem_stack;
	p_task_block->sk_info.knl_stack =
			((ptr_t) (&(((uint32_t*) knl_mem_stack)[kernel_size - 1]))) & (~0x7);
	p_task_block->sk_info.stack_type = 1;
	p_task_block->prio = prio;
	p_task_block->user_stack_size = user_stack_size;
	p_task_block->kernel_stack_size = kernel_size;
	p_task_block->status = TASK_SUSPEND;
	p_task_block->parent_task = p_task_block;
	p_task_block->next = NULL;
	p_task_block->next_all = NULL;
	p_task_block->exec_id = tcp->exec_id;
	p_task_block->mpu = NULL;
	p_task_block->thread_exit_func =
			tcp->thread_exit_func ? tcp->thread_exit_func : task_end;

	if (user_stack_size != 0) {
		/*����ջ�ĳ�ʼ���Ĵ���*/
		p_task_block->sk_info.user_stack = os_task_set_reg(
				p_task_block->sk_info.user_stack, task_fun, arg0, arg1, 0,
				p_task_block->thread_exit_func, p_task_block->user_ram);
	} else {
		p_task_block->sk_info.knl_stack = os_task_set_reg(
				p_task_block->sk_info.knl_stack, task_fun, arg0, arg1, 0,
				p_task_block->thread_exit_func, 0);
	}
	if (user_stack_size == 0) {
		//�߳����ں�ģʽ;
		p_task_block->sk_info.stack_type = 2;
	} else {
		p_task_block->sk_info.stack_type = 1;
	}
	/*ͨ�����ȼ��������*/
	int32_t err = add_task(p_task_block, 1);
	if (err != 0) {
		/*�ͷ�������ڴ�*/
		free(p_task_block);
		free(mem_stack);
		free(knl_mem_stack);
		return -ENOMEM;
	}
#if 0
    for(int32_t i=0;i<NR_FILE;i++){
        p_task_block->files[i].used=0;
    }
#endif
#if 0

    root_mount(p_task_block);
    if(CUR_TASK) {
        //�������������
        extern int32_t do_open(struct file *files, const char *path, int32_t flags, int32_t mode);
        do_open(p_task_block->files, "/dev/tty", O_RDWR, 0777);
        do_open(p_task_block->files, "/dev/tty", O_RDWR, 0777);
        do_open(p_task_block->files, "/dev/tty", O_RDWR, 0777);
    }
#endif
	atomic_inc(&sys_tasks_info->pid_temp);
	p_task_block->status = TASK_RUNNING;
	err = 0;
	return p_task_block->pid;
}

#define PRIO_PROCESS	0
#define PRIO_PGRP	1
#define PRIO_USER	2
int32_t sys_getpriority(int which, int who) {
	uint32_t t;
	int32_t ret = -ESRCH;
	struct task *tk;
	t = dis_cpu_intr();
	switch (which) {
	case PRIO_PROCESS:
		tk = task_find(who);
		ret = tk->prio;
		goto end;
	case PRIO_PGRP:
		ret = -ENOSYS;
		goto end;
	case PRIO_USER:
		ret = -ENOSYS;
		goto end;
	default:
		restore_cpu_intr(t);
		return -EINVAL;
	}
	end: restore_cpu_intr(t);
	return ret;
}
int sys_setpriority(int which, int who, int prio) {
	uint32_t t;
	int32_t ret = 0;
	struct task *tk;
//    if(!CUR_TASK->is_s_user){
//        return -EPERM;
//    }
	t = dis_cpu_intr();
	switch (which) {
	case PRIO_PROCESS:
		tk = task_find(who);
		if (task_change_prio(tk, prio) <= 0) {
			ret = -ESRCH;
		}
		goto end;
	case PRIO_PGRP:
		//TODO:
		ret = -ENOSYS;
		goto end;
	case PRIO_USER:
		//TODO:
		ret = -ENOSYS;
		goto end;
	default:
		restore_cpu_intr(t);
		return -EINVAL;
	}
	end: restore_cpu_intr(t);
	return ret;
}
pid_t shutdown_task(struct task *ls) {
	int32_t res_pid;
	del_task(NULL, ls, 0);
	del_task(&get_sys_tasks_info()->all_task_list, ls, 1);
	res_pid = ls->pid;
	free(ls);
	knl_mem_trace();
	return res_pid;
}
void thread_idle(void *arg) {
	int last_r_cn = 0;
	while (1) {
		//TODO:
		if (sys_tasks_info.wait_r) {
			sys_tasks_info.wait_r--;
			sche_lock();
			struct task *all;
			again:
			all= sys_tasks_info.all_task_list;
			for (;all;all = all->next_all) {
				if (all->status == TASK_CLOSED) {
					shutdown_task(all);
					goto again;
				}
			}
			sche_unlock();
		}

		if (sys_tasks_info.sys_run_count
				- last_r_cn
				> 1000) {
			//sys_sync();
			last_r_cn = sys_tasks_info.sys_run_count;
		}
	}
}
void sche_start(void) {
	extern void init(void);
	uint32_t t;

	t = dis_cpu_intr();
	task_sche();
	ARCH_INIT();
	init();
	restore_cpu_intr(t);
}
// ���ȳ���ĳ�ʼ���ӳ���
void init_sche(void) {
	static struct task_create_par tcp;
	int32_t pid;

	/*OS�Ƿ���ȳ�ʼ��*/
	atomic_set(&sys_tasks_info.sche_lock, 1);
	/*����pid�������*/
	atomic_set(&sys_tasks_info.pid_temp, 1);
	sys_tasks_info.current_max_task_node = NULL;
	sys_tasks_info.tasks_count = 0;

	tcp.task_fun = thread_idle;
	tcp.arg0 = (void*) 0;
	tcp.arg1 = 0;
	tcp.prio = 1;
	tcp.user_stack_size = 0;
	tcp.kernel_stack_size = 128;
	tcp.task_name = "idle";

	pid = task_create(&tcp);
	if (pid < 0) {
		kfatal("idle thread create error.\n");
	}
}
