//
// Created by Administrator on 2022/1/9.
//
#include <type.h>
#include <errno.h>
#include <string.h>
#include "arch/arch.h"
#include <mkrtos/mem.h>
#include <mkrtos/task.h>
#include "fcntl.h"

/**
 * @brief 创建任务
 * @param tcp 任务创建操作
 * @param progInfo 任务的代码信息
 * @return
 */
int32_t task_create(struct task_create_par* tcp){
    task_fun task_fun;
    void *arg0;
    void *arg1;
    uint8_t prio;
    uint16_t user_stack_size;
    uint16_t kernel_size;
    const char* task_name;

    if(tcp == NULL){
        return -1;
    }

    task_fun=tcp->task_fun;
    arg0=tcp->arg0;
    arg1=tcp->arg1;
    prio=tcp->prio;
    user_stack_size=tcp->user_stack_size;
    kernel_size=tcp->kernel_stack_size;
    task_name=tcp->task_name;

    if(task_fun == NULL){
        errno=EINVAL;
        return -1;
    }
    void *mem_stack = NULL;
    void *knl_mem_stack = NULL;
    struct task* p_task_block=NULL;
    p_task_block=(struct task*)os_malloc(sizeof(struct task));
    if(p_task_block == NULL){
        errno=ENOMEM;
        return -1;
    }
    memset(p_task_block,0,sizeof(struct task));
    p_task_block->task_name = task_name;

    /*申请堆栈的内存*/
    mem_stack = (void *)os_malloc(sizeof(uint32_t)*(user_stack_size));
    if(mem_stack == NULL){
        os_free(p_task_block);
        errno=ENOMEM;
        return -1;
    }
    knl_mem_stack= (void *)os_malloc(sizeof(uint32_t)*(kernel_size));
    if(knl_mem_stack == NULL){
        os_free(p_task_block);
        os_free(mem_stack);
        errno=ENOMEM;
        return -1;
    }
    /*初始化任务控制表*/
    p_task_block->runCount=0;
    p_task_block->PID = (pid_t)atomic_read(&sys_tasks_info.pid_temp);
    p_task_block->tpid=-1;
    if(atomic_test(&(sys_tasks_info.pid_temp),1)) {
        sys_tasks_info.init_task=p_task_block;
    }

    p_task_block->mem_low_stack=mem_stack;
    if(user_stack_size!=0){
        p_task_block->sk_info.psp_stack=(void*)(&(((uint32_t*)mem_stack)[user_stack_size-1]));
    }else{
        p_task_block->sk_info.psp_stack=(void*)(~(0L));
    }
    p_task_block->knl_low_stack=knl_mem_stack;
    p_task_block->sk_info.msp_stack=(void*)(&(((uint32_t*)knl_mem_stack)[kernel_size-1]));
    p_task_block->sk_info.stack_type=1;
    p_task_block->prio=prio;
    p_task_block->user_stack_size=user_stack_size;
    p_task_block->kernel_stack_size=kernel_size;
    p_task_block->status=TASK_SUSPEND;
    p_task_block->parent_task=p_task_block;
    p_task_block->is_s_user=1;
    p_task_block->next=NULL;
    strncpy(p_task_block->pwd_path,"/",sizeof(p_task_block->pwd_path));
//    p_task_block->nextBk=NULL;
    p_task_block->nextAll=NULL;
    if(user_stack_size!=0){
        /*设置栈的初始化寄存器*/
        p_task_block->sk_info.psp_stack=
                os_task_set_reg(p_task_block->sk_info.psp_stack,task_fun,arg0,arg1,0);
    }else{
        p_task_block->sk_info.msp_stack=
                os_task_set_reg(p_task_block->sk_info.msp_stack,task_fun,arg0,arg1,0);
    }
    if(user_stack_size==0){
        //线程在内核模式;
        p_task_block->sk_info.stack_type = 2;
    }else{
        p_task_block->sk_info.stack_type=1;
    }
    p_task_block->sk_info.svc_status=0;
    /*通过优先级添加任务*/
    int32_t err = add_task(p_task_block,1);
    if(err != 0){
        //	RestoreCpuInter(t);
        /*释放申请的内存*/
        os_free(p_task_block);
        os_free(mem_stack);
        os_free(knl_mem_stack);
        return -1;
    }

    for(int32_t i=0;i<NR_FILE;i++){
        p_task_block->files[i].used=0;
    }
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
    atomic_inc(&sys_tasks_info.pid_temp);
    p_task_block->status=TASK_RUNNING;
    err= 0;
    return p_task_block->PID;
}
