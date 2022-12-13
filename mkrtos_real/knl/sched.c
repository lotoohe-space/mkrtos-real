//
// Created by Administrator on 2022/1/9.
//

#include <type.h>
#include <errno.h>
#include <string.h>
#include <knl_service.h>
#include <mkrtos/sched.h>
#include <mkrtos.h>

/**
 * @brief 系统任务表
 */
struct sys_tasks sys_tasks_info = { 0 };

/**
 * @brief 获得系统任务信息
 */
struct sys_tasks* get_sys_tasks_info(void) {
	return &sys_tasks_info;
}
/**
 * @brief 系统任务减一，当前任务被删除了
 */
void sys_tasks_cur_dec(void) {
	sys_tasks_info.current_max_task_node->task_ready_count--;
	if (sys_tasks_info.current_max_task_node->task_ready_count == 0) {
		//任务更新
		task_update_cur();
	}
}
void sys_tasks_dec(struct task *tk) {
	tk->parent->task_ready_count--;
	if (tk->parent->task_ready_count == 0) {
		//任务更新
		task_update_cur();
	}
}
void sys_tasks_cur_inc(void)
{
	sys_tasks_info.current_task->parent->task_ready_count++;
	if (sys_tasks_info.current_task->prio
			> sys_tasks_info.current_max_task_node->task_priority) {
			task_update_cur();
	}
}
void sys_tasks_inc(struct task *tk) {
	tk->parent->task_ready_count++;
	if (tk->prio
			> sys_tasks_info.current_max_task_node->task_priority) {
			task_update_cur();
	}
}
/**
 * @brief 通过PID找到任务对象
 */
struct task* task_find(int32_t pid) {
	struct task *pstl;
	if (pid < 0 || pid == 0) {
		return sys_tasks_info.current_task;
	}

	slist_foreach(pstl, &sys_tasks_info.all_tk_list, all_node) {
		if (pstl->pid == pid) {
			return pstl;
		}
	}
	return NULL;
}
//设置任务不可打断
void task_unintr(void) {
	if (CUR_TASK->status != TASK_UNINTR && CUR_TASK->status != TASK_CLOSED) {
		CUR_TASK->status = TASK_UNINTR;
	}
}
/**
 * @brief 挂起进程
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
 * @brief 让任务运行，不更新进程状态
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
 * @brief 让当前任务进入运行状态，并更新任务状态
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
 * 锁调度器
 */
void sche_lock(void) {
	atomic_inc(&(sys_tasks_info.sche_lock));
}
/**
 * 调度器解锁
 */
void sche_unlock(void) {
	atomic_test_dec_nq(&(sys_tasks_info.sche_lock));
}
/**
 * 获取调度器的锁计数
 */
uint32_t sche_lock_cn_get(void) {
	return atomic_read(&(sys_tasks_info.sche_lock));
}

/**
 * @brief 任务调度，如果调度关闭，则调用无效
 */
void task_sche(void) {
	uint32_t t;
	uint32_t need_sw = 0;

	if (sys_tasks_info.current_max_task_node->task_ready_count == 0) {
		return;
	}
	//监测是否可以调度
	if (atomic_test(&(sys_tasks_info.sche_lock), FALSE)) {
		return;
	}
	t = dis_cpu_intr();

	if (sys_tasks_info.is_first == FALSE) {
		struct task *ptb;
		
		slist_foreach(ptb, &sys_tasks_info.current_max_task_node->tasks_links, next_node) {
			if (ptb->status != TASK_RUNNING) {
				continue;
			}

			sys_tasks_info.current_task = ptb;
			sys_tasks_info.is_first = TRUE;
			need_sw = 1;
			break;
		}
	} else {
		sys_tasks_info.last_task = sys_tasks_info.current_task;
		/*当前任务没有处于TASK_UNINTR状态才可以进行切换，不然不能够动*/
		if (CUR_TASK->status != TASK_UNINTR) {
			/*之前分配过，直接找下一个有效的*/
			struct task *ptb;
			slist_head_t *sitem = NULL;

			sitem = sys_tasks_info.current_task->next_node.next;
			do {
				if (sitem == &CUR_TASK->parent->tasks_links) {
					sitem = sys_tasks_info.current_max_task_node->tasks_links.next;
				}
				MKRTOS_ASSERT(!slist_is_empty(&sys_tasks_info.current_max_task_node->tasks_links));
				ptb = slist_entry(sitem, task_t, next_node);
				if (ptb->status == TASK_RUNNING) {
					if (ptb != sys_tasks_info.current_task) {
						need_sw = 1;
					}
					sys_tasks_info.current_task = ptb;
					break;
				}
				sitem = sitem->next;
			} while (1);
		}
	}
	if (need_sw) {
		to_schedule();
	}
	restore_cpu_intr(t);
}
/**
 * @brief 更新就绪的最高优先级到任务节点，更新的链表中就绪任务必须大于0
 * @param pSysTasks 任务管理对象
 */
void task_update_cur(void) {
	struct sys_task_base_links *ptl;
	uint32_t t = dis_cpu_intr();
	/*最大优先级*/
	struct sys_task_base_links *max_prio_task = NULL;

	slist_foreach(ptl, &sys_tasks_info.prio_head_task_list, _next){
		if (ptl->task_ready_count > 0) {
			if (max_prio_task == NULL && ptl->task_ready_count > 0) {
				max_prio_task = ptl;
			} else {
				if (ptl->task_priority > max_prio_task->task_priority
				//	&& ptl->task_ready_count>0
						) {
					/*更新*/
					max_prio_task = ptl;
				}
			}
		}
	}
	sys_tasks_info.current_max_task_node = max_prio_task;

	restore_cpu_intr(t);
}

/**
 * @brief 通过优先级找到任务链
 * @param prio 任务优先级
 * @return 最大优先级任务的表头
 */
static struct sys_task_base_links* find_task_links(uint8_t prio) {
	struct sys_task_base_links *ptl;

	slist_foreach(ptl, &sys_tasks_info.prio_head_task_list, _next) {
		if (ptl->task_priority == prio) {
			return ptl;
		}
	}

	return NULL;
}
/**
 * @brief 通过优先级添加任务头，如果指定优先级不存在，则创建一个优先级的节点
 * @param prio 添加的优先级
 * @return 添加成功的链表头
 */
static struct sys_task_base_links* add_links(uint8_t prio) {

	struct sys_task_base_links *p_sys_task_base_links =
			(struct sys_task_base_links*) malloc(
					sizeof(struct sys_task_base_links));
	if (p_sys_task_base_links == NULL) {
		return NULL;
	}
	slist_init(&p_sys_task_base_links->_next);
	slist_init(&p_sys_task_base_links->tasks_links);
	p_sys_task_base_links->task_count = 0;
	p_sys_task_base_links->task_priority = prio;
	p_sys_task_base_links->task_ready_count = 0;

	/*链表接上*/
	slist_add(&sys_tasks_info.prio_head_task_list, &p_sys_task_base_links->_next);

	return p_sys_task_base_links;
}
void del_task_prio(struct task *del)
{
	struct sys_task_base_links *taskLinks;
	slist_head_t *task_head;
	uint32_t t;

	t = dis_cpu_intr();
	taskLinks = find_task_links(del->prio);
	if (taskLinks == NULL) {
		restore_cpu_intr(t);
		return;
	}
	task_head = &(taskLinks->tasks_links);
	slist_del(&del->next_node);
	restore_cpu_intr(t);
	task_update_cur();
}
void del_task_all(struct task *del)
{
	uint32_t t;

	t = dis_cpu_intr();
	slist_del(&del->all_node);
	// dlist_del(&sys_tasks_info.all_task, &del->all_next);
	restore_cpu_intr(t);
	task_update_cur();
}
/**
 * @brief 通过优先级添加任务，如果这个优先级不存在，则创建该优先级的任务节点
 * @param pSysTasks 任务管理对象
 * @return 添加是否成功
 */
int32_t add_task(struct task *p_task_block, uint32_t into_all_ls) {
	/*关所有中断*/
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
			/*没有内存*/
			return -ENOMEM;
		}
	}
	taskLinks->task_count++;

	//放到同优先级任务链表里面
	slist_add(&taskLinks->tasks_links, &p_task_block->next_node);
	p_task_block->parent = taskLinks;

	if (into_all_ls) {
		//存到所有任务的链表中
		slist_add(&sys_tasks_info.all_tk_list, &p_task_block->all_node);
	}
	restore_cpu_intr(t);

	//更新优先级
	if (sys_tasks_info.current_max_task_node == NULL) {
		task_update_cur();
	}
	return 0;
}

/**
 * @brief 对任务进行检查
 */
void tasks_check(void) {
	struct task *ptb;
	sys_tasks_info.sys_run_count++;
    //检测所有的定时器信号
	slist_foreach(ptb, &sys_tasks_info.all_tk_list, all_node) {
		if(ptb->status==TASK_RUNNING
        && ptb->alarm){
            //时间到了
            if(ptb->alarm<sys_tasks_info.sys_run_count){
                //发送指定信号
                inner_set_sig(SIGALRM);
                ptb->alarm = 0;
            }
        }
        /*else if(ptb->status==TASK_SUSPEND){
            //SIGCONT信号在这里处理
            if (ptb->signalBMap & (1 << (SIGCONT - 1))) {
                //如果收到的信号是SIGCONT，并且此时任务暂停了，则唤醒它
                ptb->status=TASK_RUNNING;
            }
        }
		*/
	}
}
/**
 * @brief 更改优先级
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
		//没有足够的内存了，恢复之前的
		CUR_TASK->prio = old_prio;
		restore_cpu_intr(t);
		return -ENOMEM;
	}
	base_links->task_ready_count--;
	base_links->task_count--;
	CUR_TASK->prio = old_prio;
	del_task_prio(CUR_TASK);
	CUR_TASK->prio = new_prio;
	restore_cpu_intr(t);
	return 0;
}

/**
 * @brief 执行任务调度，返回下一个任务的栈顶
 * @brief SP 当前线程的栈顶地址
 * @brief SPType 使用的是MSP还是PSP 0使用msp 1使用psp
 * @return 返回栈顶
 */
struct stack_info* sys_task_sche(void *psp, void *msp, uint32_t sp_type) {
	if (sys_tasks_info.last_task) {
		/*修改当前线程的栈顶地址*/
		sys_tasks_info.last_task->sk_info.user_stack = psp;
		sys_tasks_info.last_task->sk_info.knl_stack = msp;
		if (sys_tasks_info.last_task->sk_info.stack_type != 2) {
			sys_tasks_info.last_task->sk_info.stack_type = sp_type;
		}
	}
	sys_tasks_info.current_task->run_count++;
	/*返回堆栈的地址*/
	return &(sys_tasks_info.current_task->sk_info);
}

// 系统调用功能 - 设置报警定时时间值(秒)。
// 如果已经设置过alarm 值，则返回旧值，否则返回0。
int32_t sys_alarm(uint32_t seconds) {
	uint32_t old = 0;
	if (sys_tasks_info.current_task->alarm) {
		old = sys_tasks_info.current_task->alarm;
	}
	sys_tasks_info.current_task->alarm = sys_tasks_info.sys_run_count
			+ (seconds * 1000) / (1000 / OS_WORK_HZ);
	return old;
}
// 挂起进程等待信号
int32_t sys_pause(void) {
	task_suspend();
	task_sche();
	return 0;
}

//唤醒队列中所有的任务
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
void set_helper(struct task *helper_task)
{
	struct task *curr_task = get_current_task();
	curr_task->helper_user_ram = curr_task->user_ram;
	curr_task->helper_exec_id = curr_task->exec_id;
	curr_task->helper_mpu = curr_task->mpu;

	//TODO:增加引用计数
	curr_task->user_ram = helper_task->user_ram;
	curr_task->exec_id = helper_task->exec_id;
	curr_task->mpu = helper_task->mpu;
	get_current_task()->helper = helper_task;
}
void clear_helper(void)
{
	struct task *curr_task = get_current_task();

	if (curr_task->helper_user_ram) {
		curr_task->user_ram=curr_task->helper_user_ram;
	}
	curr_task->exec_id=curr_task->helper_exec_id;
	curr_task->mpu=curr_task->helper_mpu;
	//TODO：减少引用计数
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


//添加一个到等待队列中
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
//找到一个queue，并获取最大优先级
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
//移除一个等待的
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
				//删除的第一个
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
 * @brief 调整当前任务优先级
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
 * 结束时系统将会调用这函数
 * 非内核线程时，这个函数属于用户态
 * 内核线程时，这个函数删除线程
 */
void task_end(void) {
	sys_exit(0);
}
/**
 * @brief 创建任务
 * @param tcp 任务创建操作
 * @param progInfo 任务的代码信息
 * @return
 */
int32_t task_create(struct task_create_par *tcp, int to_run) {
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

	//设置task name
	mkrtos_strncpy(p_task_block->task_name, task_name, THREAD_NAME_LEN);
	p_task_block->task_name[THREAD_NAME_LEN - 1] = 0;

	if (user_stack_size) {
		/*申请堆栈的内存*/
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
	/*初始化任务控制表*/
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
	slist_init(&p_task_block->next_node);
	slist_init(&p_task_block->all_node);
	p_task_block->exec_id = tcp->exec_id;
	p_task_block->mpu = NULL;
	p_task_block->thread_exit_func =
			tcp->thread_exit_func ? tcp->thread_exit_func : task_end;

	if (user_stack_size != 0) {
		/*设置栈的初始化寄存器*/
		p_task_block->sk_info.user_stack = os_task_set_reg(
				p_task_block->sk_info.user_stack, task_fun, arg0, arg1, 0,
				p_task_block->thread_exit_func, p_task_block->user_ram);
	} else {
		p_task_block->sk_info.knl_stack = os_task_set_reg(
				p_task_block->sk_info.knl_stack, task_fun, arg0, arg1, 0,
				p_task_block->thread_exit_func, 0);
	}
	if (user_stack_size == 0) {
		//线程在内核模式;
		p_task_block->sk_info.stack_type = 2;
	} else {
		p_task_block->sk_info.stack_type = 1;
	}
	/*通过优先级添加任务*/
	int32_t err = add_task(p_task_block, 1);
	if (err != 0) {
		/*释放申请的内存*/
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
        //打开三个串口输出
        extern int32_t do_open(struct file *files, const char *path, int32_t flags, int32_t mode);
        do_open(p_task_block->files, "/dev/tty", O_RDWR, 0777);
        do_open(p_task_block->files, "/dev/tty", O_RDWR, 0777);
        do_open(p_task_block->files, "/dev/tty", O_RDWR, 0777);
    }
#endif
	atomic_inc(&sys_tasks_info->pid_temp);
	if (to_run) {
		p_task_block->status = TASK_RUNNING;
		sys_tasks_inc(p_task_block);
	} else {
		p_task_block->status = TASK_SUSPEND;
	}
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
	ls->parent->task_count--;
	del_task_prio(ls);
	del_task_all(ls);
	res_pid = ls->pid;
	free(ls);
	//knl_mem_trace();
	return res_pid;
}
void thread_idle(void *arg) {
	int last_r_cn = 0;
	while (1) {
		//TODO:
		if (sys_tasks_info.wait_r) {
			struct task *all;

			sys_tasks_info.wait_r--;
			sche_lock();
			again:
			slist_foreach(all, &sys_tasks_info.all_tk_list, all_node) {
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
// 调度程序的初始化子程序。
void init_sche(void) {
	struct task_create_par tcp = {0};
	int32_t pid;
	slist_init(&sys_tasks_info.all_tk_list);
	slist_init(&sys_tasks_info.prio_head_task_list);
	/*OS是否调度初始化*/
	atomic_set(&sys_tasks_info.sche_lock, 1);
	/*进程pid分配变量*/
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

	pid = task_create(&tcp, TRUE);
	if (pid < 0) {
		kfatal("idle thread create error.\n");
	}
}
