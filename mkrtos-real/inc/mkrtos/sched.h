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
#include <mkrtos/cap.h>
#include <mm.h>
#include <mkrtos/mpu.h>
#define NR_FILE 8
#define THREAD_NAME_LEN 16

/**
 * @brief �����״̬
 */
enum task_status {
	TASK_RUNNING, //!<��������
	TASK_SUSPEND, //!<�������
	TASK_CLOSED, //!<�����Ѿ����ر���
	TASK_UNINTR, //!<���񲻿ɱ��ж�
};

/**
 * @brief ������
 */
typedef void (*task_fun)(void *arg0, void *arg1);

/**
 * @brief �������������б�
 */
struct task_create_par {
	task_fun task_fun; 			//!<������
	void *arg0; 				//!<����0
	void *arg1; 				//!<����1
	uint8_t prio; 				//!<���ȼ�
	uint16_t user_stack_size; 	//!<�û�ջ��С
	uint16_t kernel_stack_size; //!<�ں�ջ��С
	int32_t exec_id; 			//!<��ִ���ļ���id
	const char *task_name; 		//!<��������
	void (*thread_exit_func)(void); //!<�߳�ִ����ɺ�ִ�еĺ���
	int32_t err; //!<���صĴ���
};

/**
 * @brief ջ����
 */
struct stack_info {
	ptr_t user_stack;		//!<��ջ��ջ��ָ��
	ptr_t knl_stack;		//!<�ں˵�ջ��ָ��
	uint16_t stack_type; 	//!<ʹ�õ���MSP����PSP 0ʹ��knl stack 1ʹ��user 2�ں��߳�ģʽ
};

struct sys_task_base_links;
struct sigaction;
struct mem_struct;
//struct tty_struct;
/**
 * @brief	������ƿ�
 */
typedef struct task {
//	struct object obj; 					//!<�̳�obj
	struct sys_task_base_links *parent;	//!<�������ȼ�����ڵ�
	struct task *parent_task; 			//!<������
	struct task *next; 					//!<ͬ���ȼ�����
	struct task *next_all; 				//!<�������������
	struct task *del_wait; 				//!<ɾ���ȴ�����
	struct wait_queue *close_wait; 		//!<�رյȴ�����

	void *mem_low_stack; 				//!<�Ñ���ջ��ջ��ָ��
	void *knl_low_stack; 				//!<�ں�ջ
	uint32_t user_stack_size; 			//!<�û�ջ��С
	uint32_t kernel_stack_size; 		//!<�ں�ջ��С
	struct stack_info sk_info; 			//!<�洢��ջ��Ϣ

	uint32_t run_count; 				//!<����ʱ�����
	enum task_status status; 			//!<��ǰ״̬
	uint8_t prio; 						//!<�������ȼ�
	char task_name[THREAD_NAME_LEN]; 	// ��������

	uint32_t sig_bmp[_NSIG_WORDS]; 		//�źŵ�λͼ
	struct sigaction signals[_NSIG]; 	//�źŴ���
	uint32_t sig_mask[_NSIG_WORDS]; 	//�ź�mask

	uint32_t alarm; 					//!<��ʱ����ms

	int32_t exit_code; 					//!<�˳���

	pid_t pid; 							//!<����id
	int32_t exec_id; 					//!<��ִ���ļ���Ϣ
	int32_t link_deep_cn; 				//!<�ļ�������ȼ���
	void *thread_exit_func;				//!<�߳��˳��ĺ���
	void *user_ram;						//!<�û�̬ʹ�õ��ڴ�
	uint32_t user_ram_size;				//!<�û��ڴ��С
	mem_t user_head_alloc;				//!<�û����ڴ������

	mpu_t *mpu;							//!<mpu�����Ϣ

	int parent_ram_offset;				//!<������ram������ӽ��̵�ƫ��
//	struct object object_list; 			//!<�û���obj list

//	struct capability caps[32]; 		//!<�û���capbitlities
	uid_t ruid; 						//!<��ʵuser id
	uid_t euid; 						//!<��Чuser id
	uid_t suid; 						//!<�����user id
	uid_t rgid; 						//!<��ʵgroup id
	int32_t pgid; 						//!<��ID
	uid_t egid; 						//!<��Ч��id
	uid_t sgid; 						//!<�������id

	int clone_flag; 					//!<��¡��־

	mode_t mask; 						//!<�����ļ�ʱ���õ�mask
	struct file files[NR_FILE]; 		//!<�ļ�
	struct inode *root_inode; 			//!<��inode
	struct inode *pwd_inode; 			//!<��ǰĿ¼
} task_t;

//��ǰ���̸�Ŀ¼INODE
#define ROOT_INODE ((struct inode*)(get_current_task()->root_inode))
//��ǰ���̹���Ŀ¼INODE
#define PWD_INODE ((struct inode*)(get_current_task()->pwd_inode))
/**
 * @brief ϵͳ������������洢��ͬ���ȼ�������ͷ
 */
struct sys_task_base_links {
	/**
	 * @brief ͬ���ȼ���������
	 */
	struct task *pSysTaskLinks;
	/**
	 * @brief �������
	 */
	uint16_t task_count;
	/**
	 * @brief �����������
	 */
	uint16_t task_ready_count;
	/**
	 * @brief ��������ȼ�
	 */
	uint8_t task_priority;
	/**
	 * @brief ��һ������ͷ�ڵ�
	 */
	struct sys_task_base_links *next;

};

/**
 * @brief ϵͳ����
 */
struct sys_tasks {
	/**
	 * @brief ϵͳ����
	 */
	struct sys_task_base_links sys_task_base_links;
	/**
	 * @brief �����߳�
	 */
	struct task *idle_task;
	/**
	 * @brief ����ʱ��
	 */
	uint32_t sys_run_count;

	/**
	 * @brief ��ǰ������ȼ�����ڵ�
	 */
	struct sys_task_base_links *current_max_task_node;

	/**
	 * @brief ��ǰ����������ڵ�
	 */
	struct task *current_task;
	/**
	 * @brief �ϴ����е��߳�
	 */
	struct task *last_task;
	/**
	 * @brief �Ƿ�����ϵͳ�����������
	 */
	atomic_t sche_lock;

	/**
	 * @brief �������������
	 */
	struct task *all_task_list;
	/**
	 * @brief ���������������IDʹ��
	 */
	atomic_t pid_temp;
	/**
	 * @brief ϵͳ������
	 */
	uint16_t tasks_count;
	/**
	 * @brief �Ƿ��״�,��Ϊ0������Ϊ1
	 */
	uint8_t is_first;
	uint8_t wait_r; //!<�Ƿ��н��̵ȴ��ͷ�
};

//�ȴ�����
struct wait_queue {
	struct task *task;
	struct wait_queue *next;
};

//sched.c
extern struct sys_tasks sys_tasks_info;
/**
 * @brief ��ȡ��ǰ�߳�
 */
static inline struct task* get_current_task(void) {
	return sys_tasks_info.current_task;
}
struct sys_tasks* get_sys_tasks_info(void);
struct task* get_current_task(void);
void sys_tasks_cur_dec(void);
struct task* task_find(int32_t pid);
void task_unintr(void);
void task_suspend(void);
void task_run(void);
void task_run_1(struct task *tk);
void sche_lock(void);
void sche_unlock(void);
uint32_t get_sche_lock_cn(void);
void task_sche(void);
void task_update_cur(void);
void del_task(struct task **task_ls, struct task *del, int flag);
int32_t add_task(struct task *p_task_block, uint32_t into_all_ls);
void tasks_check(void);
int32_t task_change_prio(struct task *tk, int32_t new_prio);
struct stack_info* sys_task_sche(void *psp, void *msp, uint32_t sp_type);
int32_t sys_alarm(uint32_t seconds);
int32_t sys_pause(void);
void wake_up(struct wait_queue *queue);
int32_t task_create(struct task_create_par *tcp);
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
#endif //UNTITLED1_TASK_H
