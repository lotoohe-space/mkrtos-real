//
// Created by Administrator on 2022/1/9.
//

#ifndef UNTITLED1_TASK_H
#define UNTITLED1_TASK_H
#include <type.h>
#include <arch/atomic.h>
#include "mkrtos/signal.h"
#include <mkrtos/object.h>
#include <mkrtos/fs.h>
#include <mkrtos/list.h>
#include <mkrtos/sched.h>
#include <mm.h>
#include <mkrtos/mpu.h>
#include <mkrtos/dlist.h>
#include <mkrtos/slist.h>

#define NR_FILE 8	//!<最大文件描述符数量
#define THREAD_NAME_LEN 8 //!<最大线程名长度

/**
 * @brief 任务的状态
 */
enum task_status {
	TASK_RUNNING, //!<任务运行
	TASK_SUSPEND, //!<任务挂起
	TASK_CLOSED, //!<任务已经被关闭了
	TASK_UNINTR, //!<任务不可被中断
};

/**
 * @brief 任务函数
 */
typedef void (*task_fun)(void *arg0, void *arg1);

/**
 * @brief 函数创建传参列表
 */
struct task_create_par {
	task_fun task_fun; 			//!<任务函数
	void *arg0; 				//!<参数0
	void *arg1; 				//!<参数1
	uint8_t prio; 				//!<优先级
	uint16_t user_stack_size; 	//!<用户栈大小
	uint16_t kernel_stack_size; //!<内核栈大小
	int32_t exec_id; 			//!<可执行文件的id
	const char *task_name; 		//!<任务名字
	void (*thread_exit_func)(void); //!<线程执行完成后执行的函数
	int32_t err; //!<返回的错误
};

/**
 * @brief 栈类型
 */
struct stack_info {
	ptr_t user_stack;		//!<堆栈的栈顶指针
	ptr_t knl_stack;		//!<内核的栈顶指针
	uint16_t stack_type; 	//!<使用的是MSP还是PSP 0使用knl stack 1使用user
};

struct sys_task_base_links;
struct sigaction;
// struct mem_struct;
//struct tty_struct;
/**
 * @brief	任务控制块
 */
typedef struct task {
	struct sys_task_base_links *parent;	//!<父优优先级链表节点
	struct task *parent_task; 			//!<父进程
	slist_head_t next_node;					//!<同优先级链表
	slist_head_t all_node;// all_next;				//!<所有任务的链表节点
	struct task *del_wait; 				//!<删除等待队列
	struct wait_queue *close_wait; 		//!<关闭等待队列

	void *mem_low_stack; 				//!<用戶堆栈的栈低指针
	void *knl_low_stack; 				//!<内核栈
	uint32_t user_stack_size; 			//!<用户栈大小
	uint32_t kernel_stack_size; 		//!<内核栈大小
	struct stack_info sk_info; 			//!<存储堆栈信息

	uint32_t run_count; 				//!<运行时间计数
	enum task_status status; 			//!<当前状态
	uint8_t prio; 						//!<任务优先级
	char task_name[THREAD_NAME_LEN]; 	//!<任务名称

	uint32_t sig_bmp[_NSIG_WORDS]; 		//信号的位图
	struct sigaction signals[_NSIG]; 	//信号处理
	uint32_t sig_mask[_NSIG_WORDS]; 	//信号mask

	uint32_t alarm; 					//!<定时多少ms

	int32_t exit_code; 					//!<退出码

	pid_t pid; 							//!<进程id
	int16_t exec_id; 					//!<可执行文件信息
	int16_t link_deep_cn; 				//!<文件连接深度计数
	void *thread_exit_func;				//!<线程退出的函数
	void *user_ram;						//!<用户态使用的内存
	uint32_t user_ram_size;				//!<用户内存大小
	mem_t user_head_alloc;				//!<用户堆内存分配器

	mpu_t *mpu;							//!<mpu相关信息

	int parent_ram_offset;				//!<父进程ram相对于子进程的偏移

	uid_t ruid; 						//!<真实user id
	uid_t euid; 						//!<有效user id
	uid_t suid; 						//!<保存的user id
	uid_t rgid; 						//!<真实group id
	uid_t pgid; 						//!<组ID
	uid_t egid; 						//!<有效组id
	uid_t sgid; 						//!<保存的组id

	int clone_flag; 					//!<克隆标志
	struct task *helper;				//!<

	void *helper_user_ram;				//!<用户态使用的内存
	int32_t helper_exec_id;
	mpu_t *helper_mpu;
	void *helper_stack;
	void *saving_user_stack;

	mode_t mask; 						//!<创建文件时设置的mask
	struct file files[NR_FILE]; 		//!<文件
	struct inode *root_inode; 			//!<根inode
	struct inode *pwd_inode; 			//!<当前目录
} task_t;

#define TASK_USER_RAM_ADDR(task) ((mword_t)((task)->user_ram)) //!< 用户内存地址
#define TASK_USER_RAM_SIZE(task) ((mword_t)((task)->user_ram_size)) //!< 用户态内存大小

//当前进程根目录INODE
#define ROOT_INODE ((struct inode*)(get_current_task()->root_inode))
//当前进程工作目录INODE
#define PWD_INODE ((struct inode*)(get_current_task()->pwd_inode))
/**
 * @brief 系统任务基础链表，存储不同优先级的链表头
 */
struct sys_task_base_links {
	slist_head_t tasks_links;
	uint16_t task_count; //!< 任务个数
	uint16_t task_ready_count; //!< 就绪任务个数
	uint8_t task_priority; //!< 任务的优先级
	slist_head_t _next;
};

/**
 * @brief 系统任务
 */
struct sys_tasks {
	slist_head_t prio_head_task_list; //!< 任务优先级链表
	struct task *idle_task; //!< 空闲线程
	uint32_t sys_run_count; //!< 运行时间
	struct sys_task_base_links *current_max_task_node; //!< 当前最高优先级任务节点
	struct task *current_task; //!< 当前工作的任务节点
	struct task *last_task; //!< 上次运行的线程
	atomic_t sche_lock; //!< 是否允许系统进行任务调度
	slist_head_t all_tk_list; //!< 所有的任务链表
	atomic_t pid_temp; //!< 创建任务分配任务ID使用
	uint16_t tasks_count; //!< 系统任务数
	atomic_t wait_r; //!< 是否有进程等待释放
	uint8_t is_first; //!< 是否首次,是为0，不是为1
	uint8_t is_run; //!< 系统是否启动完成
};

//等待链表
struct wait_queue {
	struct task *task;
	struct wait_queue *next;
};

//sched.c
extern struct sys_tasks sys_tasks_info;

//! 获取当前的进程
#define CUR_TASK sys_tasks_info.current_task

/**
 * @brief Get the current task object
 * 
 * @return struct task* 
 */
static inline struct task* get_current_task(void)
{
	return sys_tasks_info.current_task;
}
/**
 * @brief Set the current task
 * 
 * @param tk 
 */
static inline void set_current_task(struct task* tk)
{
	sys_tasks_info.current_task = tk;
}
struct sys_tasks* get_sys_tasks_info(void);
struct task* get_current_task(void);
void sys_tasks_cur_dec(void);
void sys_tasks_dec(struct task *tk);
void sys_tasks_cur_inc(void);
void sys_tasks_inc(struct task *tk);
struct task* task_find(int32_t pid);
void task_unintr(void);
void task_suspend(void);
void task_run(void);
void task_run_1(struct task *tk);
void sche_lock(void);
void sche_unlock(void);
uint32_t sche_lock_cn_get(void);
void task_sche(void);
void task_update_cur(void);
int32_t add_task(struct task *p_task_block, uint32_t into_all_ls);
void tasks_check(void);
void set_helper(struct task *helper_task);
void clear_helper(void);
int32_t task_change_prio(struct task *tk, int32_t new_prio);
struct stack_info* sys_task_sche(void *psp, void *msp, uint32_t sp_type);
int32_t sys_alarm(uint32_t seconds);
int32_t sys_pause(void);
void wake_up(struct wait_queue *queue);
int32_t task_create(struct task_create_par *tcp, int to_run);
void sche_start(void);
pid_t shutdown_task(struct task *ls);
void init_sche(void);

void add_wait_queue(struct wait_queue **queue, struct wait_queue *add_queue);
struct wait_queue* find_wait_queue(struct wait_queue **queue, struct task *tk,
		uint32_t *max_prio);
void remove_wait_queue(struct wait_queue **queue, struct wait_queue *add_queue);

//sleep.c
struct sleep_time_queue {
	struct task *task;
	struct sleep_time_queue *next;
	uint32_t slp_ms;
	uint32_t cur_ms;
};
int do_nanosleep(struct sleep_time_queue **slp_ls, const struct timespec *req,
		struct timespec *rem, struct atomic *lock, int check_val);
void sleep_ms(int ms);
void wake_up_sleep(struct sleep_time_queue **temp);
void _do_check_sleep_tim(struct sleep_time_queue **slp_ls);
void do_check_sleep_tim(void);

//signal.c
void sig_chld(struct task *tk);
int32_t inner_set_task_sig(struct task *tk, uint32_t signum);

//exit.c
void do_exit(int32_t exit_code);

//printk.c
void kprint(const char *fmt, ...);
void kfatal(const char *fmt, ...);
void ktrace(const char *fmt, ...);

//exec.c
int sys_do_execve(struct task *s_task, const char *filename, char *const argv[], char *const envp[]);

#endif //UNTITLED1_TASK_H
