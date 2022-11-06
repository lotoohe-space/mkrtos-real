//
// Created by Administrator on 2022/1/9.
//

#ifndef UNTITLED1_TASK_H
#define UNTITLED1_TASK_H
#include "type.h"
#include "arch/atomic.h"
#include "mkrtos/signal.h"
#include "ipc/spin_lock.h"
#include <mkrtos/fs.h>


#define NR_FILE 8
#define CUR_TASK sys_tasks_info.current_task

//struct file;

/**
* @brief �����״̬
*/
enum task_status{

    TASK_RUNNING,//��������
    TASK_SUSPEND,//�������
    TASK_CLOSED,// �����Ѿ����ر���
    TASK_UNINTR,//���񲻿ɱ��ж�
};

/**
* @brief ������
*/
typedef void (*task_fun)(void *arg0,void *arg1);


/**
* @brief �������������б�
*/
struct task_create_par{
    /**
    * @brief ������
    */
	task_fun task_fun;
    /**
    * @brief ����0
    */
    void *arg0;
    /**
    * @brief ����1
    */
    void *arg1;
    /**
    * @brief ���ȼ�
    */
    uint8_t prio;
    /**
    * @brief �û�ջ��С
    */
    uint16_t user_stack_size;
    /**
    * @brief �ں�ջ��С
    */
    uint16_t kernel_stack_size;
    /**
    * @brief ��������
    */
    const char* task_name;
    /**
     * @brief ���صĴ���
     */
    int32_t err;
};


/**
* @brief ջ����
*/
struct stack_info{
    /**
    * @brief ��ջ��ջ��ָ��
    */
    void *psp_stack;
    /**
     * @brief �ں˵�ջ��ָ��
     */
    void *msp_stack;
    /**
    * @brief ʹ�õ���MSP����PSP 0ʹ��msp 1ʹ��psp 2�ں��߳�ģʽ
    */
    uint16_t stack_type;
    /**
    * @brief svc�ж��Ƿ����,pendsv����Ƕ��svc�ж�,Ϊ1��������svc����
    */
    uint16_t svc_status;
};

struct sys_task_base_links;
struct sigaction;
struct mem_struct;
/**
* @brief	������ƿ�
*/
struct task{
	struct sys_task_base_links *parent;//���ڵ�
    struct task *parent_task;//������
    struct task *next;//ͬ���ȼ�����
    struct task *next_all;//�������������
    struct task *del_wait;//ɾ���ȴ�����
    struct wait_queue *close_wait;//�رյȴ�����

    void *mem_low_stack;//�Ñ���ջ��ջ��ָ�룬�������ս�ʱ�����ڴ��ͷ�
    void *knl_low_stack;//�ں�ջ
    uint32_t user_stack_size;//�û�ջ��С
    uint32_t kernel_stack_size;//�ں�ջ��С
    struct stack_info sk_info;//�洢��ջ��Ϣ

    uint32_t run_count;//����ʱ�����
    enum task_status status;//��ǰ״̬
    uint8_t prio;//�������ȼ�
    const char*	task_name;// ��������

    uint32_t sig_bmp[_NSIG_WORDS];//�źŵ�λͼ
    struct sigaction signals[_NSIG];//�źŴ���
    uint32_t sig_mask[_NSIG_WORDS];//�ź�mask

    uint32_t alarm;//��ʱ����ms


    int32_t exit_code;//�˳���
    struct mem_struct *mems;//��������ڴ��������������ر�ʱ������������������ڴ潫���ͷ�

    uint8_t is_s_user; //�Ƿ񳬼��û�

    pid_t PID;//����id
//
//
//    struct file files[NR_FILE];//�ļ�
//    struct inode* root_inode; //��inode
//    struct inode* pwd_inode;//��ǰĿ¼
//    char pwd_path[128];//��ǰ�ľ���·��
};

/**
* @brief ϵͳ������������洢��ͬ���ȼ�������ͷ
*/
struct sys_task_base_links{
    /**
    * @brief ͬ���ȼ���������
    */
    struct task* pSysTaskLinks;
    /**
    * @brief ��������ȼ�
    */
    uint8_t task_priority;
    /**
    * @brief �������
    */
    uint16_t task_count;
    /**
    * @brief �����������
    */
    uint16_t task_ready_count;
    /**
    * @brief ��һ������ͷ�ڵ�
    */
    struct sys_task_base_links *next;

};

/**
* @brief ϵͳ����
*/
struct sys_tasks{

    /**
    * @brief ϵͳ����
    */
	struct sys_task_base_links sysTaskBaseLinks;
    /**
    * @brief ���б���������������
    */
    struct task* pBlockedLinks;
    struct task* init_task;
    /**
    * @brief ����ʱ��
    */
    uint32_t sys_run_count;

    /**
    * @brief ��ǰ������ȼ�����ڵ�
    */
    struct sys_task_base_links* current_max_task_node;
    /**
    * @brief �Ƿ��״�,��Ϊ0������Ϊ1
    */
    uint8_t isFirst;
    /**
    * @brief ��ǰ����������ڵ�
    */
    struct task* current_task;
    /**
    * @brief ϵͳ������
    */
    uint32_t tasks_count;

    /**
    * @brief �Ƿ�����ϵͳ�����������
    */
    Atomic_t is_sche;
    /**
    * @brief �������������
    */
    struct task* all_task_list;
    /**
    * @brief ���������������IDʹ��
    */
    Atomic_t pid_temp;
};

extern struct sys_tasks sys_tasks_info;

struct task* find_task(int32_t PID);
void    task_sche(void);
int32_t add_task(struct task *add,uint32_t into_all_ls);
void    del_task(struct task** task_ls, struct task* del,int flag);
int32_t task_create(struct task_create_par* tcp);

//�ȴ�����
struct wait_queue{
    struct task* task;
    struct wait_queue *next;
};
//sched.c
void wake_up(struct wait_queue *queue);
void add_wait_queue(struct wait_queue ** queue,struct wait_queue* add_queue);
struct wait_queue * find_wait_queue(struct wait_queue ** queue, struct task* tk,uint32_t *max_prio);
void remove_wait_queue(struct wait_queue ** queue,struct wait_queue* add_queue);
int32_t task_change_prio(struct task *tk,int32_t new_prio);
void update_cur_task(void);
void task_suspend(void);
void task_unintr(void);
void task_run(void);
void task_run_1(struct task* tk);
int32_t sys_pause(void);

//printk.c
void printk(const char *fmt, ...);
void fatalk(const char* fmt, ...);

#endif //UNTITLED1_TASK_H
