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
* @brief 任务的状态
*/
enum task_status{

    TASK_RUNNING,//任务运行
    TASK_SUSPEND,//任务挂起
    TASK_CLOSED,// 任务已经被关闭了
    TASK_UNINTR,//任务不可被中断
};

/**
* @brief 任务函数
*/
typedef void (*task_fun)(void *arg0,void *arg1);


/**
* @brief 函数创建传参列表
*/
struct task_create_par{
    /**
    * @brief 任务函数
    */
	task_fun task_fun;
    /**
    * @brief 参数0
    */
    void *arg0;
    /**
    * @brief 参数1
    */
    void *arg1;
    /**
    * @brief 优先级
    */
    uint8_t prio;
    /**
    * @brief 用户栈大小
    */
    uint16_t user_stack_size;
    /**
    * @brief 内核栈大小
    */
    uint16_t kernel_stack_size;
    /**
    * @brief 任务名字
    */
    const char* task_name;
    /**
     * @brief 返回的错误
     */
    int32_t err;
};


/**
* @brief 栈类型
*/
struct stack_info{
    /**
    * @brief 堆栈的栈顶指针
    */
    void *psp_stack;
    /**
     * @brief 内核的栈顶指针
     */
    void *msp_stack;
    /**
    * @brief 使用的是MSP还是PSP 0使用msp 1使用psp 2内核线程模式
    */
    uint16_t stack_type;
    /**
    * @brief svc中断是否产生,pendsv可能嵌套svc中断,为1代表正在svc调用
    */
    uint16_t svc_status;
};

struct sys_task_base_links;
struct sigaction;
struct mem_struct;
/**
* @brief	任务控制块
*/
struct task{
	struct sys_task_base_links *parent;//父节点
    struct task *parent_task;//父进程
    struct task *next;//同优先级链表
    struct task *next_all;//所有任务的链表
    struct task *del_wait;//删除等待队列
    struct wait_queue *close_wait;//关闭等待队列

    void *mem_low_stack;//用戶堆栈的栈低指针，当任务终结时用于内存释放
    void *knl_low_stack;//内核栈
    uint32_t user_stack_size;//用户栈大小
    uint32_t kernel_stack_size;//内核栈大小
    struct stack_info sk_info;//存储堆栈信息

    uint32_t run_count;//运行时间计数
    enum task_status status;//当前状态
    uint8_t prio;//任务优先级
    const char*	task_name;// 任务名称

    uint32_t sig_bmp[_NSIG_WORDS];//信号的位图
    struct sigaction signals[_NSIG];//信号处理
    uint32_t sig_mask[_NSIG_WORDS];//信号mask

    uint32_t alarm;//定时多少ms


    int32_t exit_code;//退出码
    struct mem_struct *mems;//管理进程内存申请的链表，任务关闭时，链表中所有申请的内存将被释放

    uint8_t is_s_user; //是否超级用户

    pid_t PID;//进程id
//
//
//    struct file files[NR_FILE];//文件
//    struct inode* root_inode; //根inode
//    struct inode* pwd_inode;//当前目录
//    char pwd_path[128];//当前的绝对路径
};

/**
* @brief 系统任务基础链表，存储不同优先级的链表头
*/
struct sys_task_base_links{
    /**
    * @brief 同优先级的任务链
    */
    struct task* pSysTaskLinks;
    /**
    * @brief 任务的优先级
    */
    uint8_t task_priority;
    /**
    * @brief 任务个数
    */
    uint16_t task_count;
    /**
    * @brief 就绪任务个数
    */
    uint16_t task_ready_count;
    /**
    * @brief 下一个任务头节点
    */
    struct sys_task_base_links *next;

};

/**
* @brief 系统任务
*/
struct sys_tasks{

    /**
    * @brief 系统任务
    */
	struct sys_task_base_links sysTaskBaseLinks;
    /**
    * @brief 所有被阻塞的任务链表
    */
    struct task* pBlockedLinks;
    struct task* init_task;
    /**
    * @brief 运行时间
    */
    uint32_t sys_run_count;

    /**
    * @brief 当前最高优先级任务节点
    */
    struct sys_task_base_links* current_max_task_node;
    /**
    * @brief 是否首次,是为0，不是为1
    */
    uint8_t isFirst;
    /**
    * @brief 当前工作的任务节点
    */
    struct task* current_task;
    /**
    * @brief 系统任务数
    */
    uint32_t tasks_count;

    /**
    * @brief 是否允许系统进行任务调度
    */
    Atomic_t is_sche;
    /**
    * @brief 所有任务的链表
    */
    struct task* all_task_list;
    /**
    * @brief 创建任务分配任务ID使用
    */
    Atomic_t pid_temp;
};

extern struct sys_tasks sys_tasks_info;

struct task* find_task(int32_t PID);
void    task_sche(void);
int32_t add_task(struct task *add,uint32_t into_all_ls);
void    del_task(struct task** task_ls, struct task* del,int flag);
int32_t task_create(struct task_create_par* tcp);

//等待链表
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
