//
// Created by Administrator on 2022/1/9.
//

#include <type.h>
#include <errno.h>
#include <string.h>
#include "arch/arch.h"
#include <mkrtos/task.h>
#include <mkrtos/mem.h>

/**
* @brief 系统任务表
*/
struct sys_tasks sys_tasks_info={0};

/**
* @brief 通过PID找到任务对象
*/
struct task* find_task(int32_t PID){
    struct task* pstl;
    if(PID<0||PID==0){
        return sys_tasks_info.current_task;
    }
    pstl=sys_tasks_info.all_task_list;
    while(pstl){
        if(pstl->PID==PID){
            return pstl;
        }
        pstl=pstl->nextAll;
    }
    return NULL;
}
//设置任务不可打断
void task_unintr(void){
    if(CUR_TASK->status!=TASK_UNINTR
       &&CUR_TASK->status!=TASK_CLOSED
            ) {
        CUR_TASK->status = TASK_UNINTR;
    }
}
//让任务挂起
void task_suspend(void){
    if(CUR_TASK->status!=TASK_SUSPEND
     &&CUR_TASK->status!=TASK_CLOSED
     &&CUR_TASK->status!=TASK_UNINTR
    ) {
        sys_tasks_info.current_max_task_node->task_ready_count--;
        if(sys_tasks_info.current_max_task_node->task_ready_count==0){
            //任务更新
            update_cur_task();
        }
        CUR_TASK->status = TASK_SUSPEND;
    }
}
//让任务运行
void task_run(void){
    if(CUR_TASK->status!=TASK_RUNNING
       &&CUR_TASK->status!=TASK_CLOSED
    ){
        if(CUR_TASK->status==TASK_SUSPEND) {
            sys_tasks_info.current_max_task_node->task_ready_count++;
        }
        CUR_TASK->status = TASK_RUNNING;
    }
}
void task_run_1(struct task* tk){
    if(!tk){
        return ;
    }
    if(tk->status!=TASK_RUNNING
       &&tk->status!=TASK_CLOSED
            ){
        if(tk->status==TASK_SUSPEND) {
            tk->parent->task_ready_count++;
            if (tk->prio > sys_tasks_info.current_max_task_node->task_priority) {
                update_cur_task();
            }
        }
        tk->status = TASK_RUNNING;
    }
}

/**
* @brief 任务调度，如果任务调度关闭，则调用无效
*/
void task_sche(void){
    if(sys_tasks_info.current_max_task_node->task_ready_count==0){
        return ;
    }
    //监测是否可以调度
    if(atomic_test(&(sys_tasks_info.is_sche),TRUE)){
        _TaskSchedule();
    }
}
/**
* @brief 更新就绪的最高优先级到任务节点，更新的链表中就绪任务必须大于0
* @param pSysTasks 任务管理对象
*/
void update_cur_task(void){

	struct sys_task_base_links* ptl;
    uint32_t t=dis_cpu_intr();

    ptl=sys_tasks_info.sysTaskBaseLinks.next;

    /*最大优先级*/
    struct sys_task_base_links* max_prio_task=NULL;

    while(ptl){
        if(ptl->task_ready_count>0){
            if(max_prio_task==NULL
               && ptl->task_ready_count>0
                    ){
                max_prio_task=ptl;
            }else{
                if(ptl->task_priority > max_prio_task->task_priority
                    //	&& ptl->task_ready_count>0
                        ){
                    /*更新*/
                    max_prio_task=ptl;
                }
            }
        }
        ptl=ptl->next;
    }
    sys_tasks_info.current_max_task_node=max_prio_task;

    restore_cpu_inter(t);
}

/**
* @brief 通过优先级找到任务链
* @param prio 任务优先级
* @return 最大优先级任务的表头
*/
static struct sys_task_base_links* find_task_links(uint8_t prio){
	struct sys_task_base_links* ptl;
    ptl=sys_tasks_info.sysTaskBaseLinks.next;
    while(ptl){
        if(ptl->task_priority == prio){
            return ptl;
        }
        ptl=ptl->next;
    }
    return NULL;
}
/**
* @brief 通过优先级添加任务头，如果指定优先级不存在，则创建一个优先级的节点
* @param prio 添加的优先级
 * @return 添加成功的链表头
*/
static struct sys_task_base_links* add_links(uint8_t prio){

	struct sys_task_base_links* p_sys_task_base_links
            =(struct sys_task_base_links*)os_malloc(sizeof(struct sys_task_base_links));
    if(p_sys_task_base_links == NULL){
        return NULL;
    }
    p_sys_task_base_links->next=NULL;
    p_sys_task_base_links->pSysTaskLinks=NULL;
    p_sys_task_base_links->task_count=0;
    p_sys_task_base_links->task_priority=prio;
    p_sys_task_base_links->task_ready_count=0;

    /*链表接上*/
    p_sys_task_base_links->next=sys_tasks_info.sysTaskBaseLinks.next;
    sys_tasks_info.sysTaskBaseLinks.next=p_sys_task_base_links;

    return p_sys_task_base_links;
}
/**
 * 删除任务
 * @param del
 */
void del_task(struct task** task_ls, struct task* del,int flag){
	struct sys_task_base_links* taskLinks;
    uint32_t t;
    t=dis_cpu_intr();
    if(!task_ls){
        taskLinks = FindTaskLinks(del->prio);
        if(taskLinks==NULL){
            restore_cpu_inter(t);
            return ;
        }
        task_ls=&(taskLinks->pSysTaskLinks);
    }
    struct task* pTemp=*task_ls;//taskLinks->pSysTaskLinks;
    struct task* lastP=NULL;
    while(pTemp){
        if(del==pTemp){
            if(lastP==NULL){
                if(!flag) {
                    *task_ls = pTemp->next;
                }else {
                    *task_ls = pTemp->nextAll;
                }
                break;
            }else{
                if(!flag) {
                    lastP->next = pTemp->next;
                }else{
                    lastP->nextAll = pTemp->nextAll;
                }
                break;
            }
        }
        lastP=pTemp;
        pTemp=pTemp->next;
    }
    restore_cpu_inter(t);
    update_cur_task();
}
/**
* @brief 通过优先级添加任务，如果这个优先级不存在，则创建该优先级的任务节点
* @param pSysTasks 任务管理对象
* @return 添加是否成功
*/
int32_t add_task(struct task* pTaskBlock,uint32_t into_all_ls){

    if(pTaskBlock==NULL){
        return -1;
    }
    /*关所有中断*/
    uint32_t t;
    t=dis_cpu_intr();
    struct sys_task_base_links* taskLinks;

    taskLinks = find_task_links(pTaskBlock->prio);
    if(taskLinks == NULL){
        taskLinks=add_links(pTaskBlock->prio);
        if(taskLinks==NULL){
            restore_cpu_inter(t);
            /*没有内存*/
            return -ENOMEM;
        }
    }
    taskLinks->task_count++;
    taskLinks->task_ready_count++;

    //放到同优先级任务链表里面
    struct task* pstl=taskLinks->pSysTaskLinks;
    if(pstl == NULL){
        pTaskBlock->next=NULL;
        taskLinks->pSysTaskLinks=pTaskBlock;
    }else{
        /*放在链表最开头*/
        pTaskBlock->next=pstl;
        taskLinks->pSysTaskLinks=pTaskBlock;
    }
    pTaskBlock->parent=taskLinks;

    if(into_all_ls) {
        //存到所有任务的链表中
        pstl = sys_tasks_info.all_task_list;
        if (pstl == NULL) {
            pTaskBlock->nextAll = NULL;
            sys_tasks_info.all_task_list = pTaskBlock;
        } else {
            /*放在链表最开头*/
            pTaskBlock->nextAll = pstl;
            sys_tasks_info.all_task_list = pTaskBlock;
        }
    }
    restore_cpu_inter(t);

    //更新优先级
    if(
        sys_tasks_info.current_max_task_node!=NULL
        &&
        pTaskBlock->prio>sys_tasks_info.current_max_task_node->task_priority
        ) {
        update_cur_task();
    }else if(sys_tasks_info.current_max_task_node==NULL){
        update_cur_task();
    }
    return 0;
}

/**
* @brief 对任务进行检查
*/
void tasks_check(void){
    struct task* ptb;
    sys_tasks_info.sys_run_count++;
    //检测所有的定时器信号
    ptb=sys_tasks_info.all_task_list;
    while(ptb){
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
nextAll:
        ptb=ptb->nextAll;
    }
}

int32_t task_change_prio(struct task *tk,int32_t new_prio){
    uint32_t t;
    uint32_t old_prio;
    struct sys_task_base_links*base_links;
    if(!tk){
        return -1;
    }
    if(tk->prio==new_prio){
        return 0;
    }
    t = dis_cpu_intr();
    base_links=CUR_TASK->parent;
    old_prio=CUR_TASK->prio;
    CUR_TASK->prio = new_prio;
    if (add_task(CUR_TASK,0) < 0) {
        //没有足够的内存了，恢复之前的
        CUR_TASK->prio = old_prio;
        restore_cpu_intr(t);
        return -ENOMEM;
    }
    base_links->task_ready_count--;
    base_links->task_count--;
    CUR_TASK->prio = old_prio;
    del_task(NULL,CUR_TASK,0);
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
struct _stackInfo* sys_task_sche(void* psp,void* msp,uint32_t spType){
    uint16_t svc_status=FALSE;
    volatile uint32_t *svcPendReg=(uint32_t*)(0xE000ED24);
    //保存svc状态
    if((*svcPendReg)&0x80L){
        (*svcPendReg)&=~(0x80L);
        svc_status=TRUE;
    }
    if(sys_tasks_info.isFirst==FALSE){
        /*第一次未分配，则分配一个*/
    	struct task* ptb=sys_tasks_info.current_max_task_node->pSysTaskLinks;
        while(ptb){
            if(ptb->status!=TASK_RUNNING
                //|| ptb->delayCount>0
                    ){
                ptb=ptb->next;
                continue;
            }

            sys_tasks_info.current_task=ptb;
            sys_tasks_info.isFirst=TRUE;
            break;
        }
    }else{
        /*修改当前线程的栈顶地址*/
        sys_tasks_info.current_task->sk_info.psp_stack = psp;
        sys_tasks_info.current_task->sk_info.msp_stack = msp;
        if (sys_tasks_info.current_task->sk_info.stack_type != 2) {
            sys_tasks_info.current_task->sk_info.stack_type = spType;
        }
        sys_tasks_info.current_task->sk_info.svc_status = svc_status;
        /*当前任务没有处于TASK_UNINTR状态才可以进行切换，不然不能够动*/
        if(CUR_TASK->status!=TASK_UNINTR) {
            /*之前分配过，直接找下一个有效的*/
        	struct task* ptb;
            ptb = sys_tasks_info.current_task->next;
            do {
                if (ptb == NULL) {
                    ptb = sys_tasks_info.current_max_task_node->pSysTaskLinks;
                }
                if (ptb->status == TASK_RUNNING
                    /*|| ptb->delayCount>0*/
                        ) {
                    sys_tasks_info.current_task = ptb;
                    break;
                }

                ptb = ptb->next;
            } while (1);
        }
    }
    //恢复任务的svc状态
    if(sys_tasks_info.current_task->sk_info.svc_status==TRUE){
        (*svcPendReg)|=0x80L;
        (*svcPendReg)&=~(0x100L);
    }
    sys_tasks_info.current_task->runCount++;
    /*返回堆栈的地址*/
    return &(sys_tasks_info.current_task->sk_info);
}

// 系统调用功能 - 设置报警定时时间值(秒)。
// 如果已经设置过alarm 值，则返回旧值，否则返回0。
int32_t sys_alarm (uint32_t seconds){
    uint32_t old = 0;
    if(sys_tasks_info.current_task->alarm){
        old=sys_tasks_info.current_task->alarm;
    }
    sys_tasks_info.current_task->alarm = sys_tasks_info.sys_run_count+(seconds*1000)/(1000/OS_WORK_HZ);
    return old;
}
// 挂起进程等待信号
int32_t sys_pause(void){
    task_suspend();
//    CUR_TASK->status=TASK_SUSPEND;
    task_sche();
    return -1;
}

//唤醒队列中所有的任务
void wake_up(struct wait_queue *queue){
    uint32_t t;
    t=dis_cpu_intr();
    while(queue){
        if(queue->task){
            if(queue->task->status==TASK_SUSPEND
            ||queue->task->status==TASK_UNINTR
            ){
                task_run_1(queue->task);
//                queue->task->status=TASK_RUNNING;
            }
        }
        queue=queue->next;
    }
    restore_cpu_inter(t);
}

//添加一个到等待队列中
void add_wait_queue(struct wait_queue ** queue,struct wait_queue* add_queue){
    uint32_t t;
    t=dis_cpu_intr();
    if(*queue==NULL){
        *queue=add_queue;
    }else{
       add_queue->next = (*queue);
       *queue=add_queue;
    }
    restore_cpu_inter(t);
}
//找到一个queue，并获取最大优先级
struct wait_queue * find_wait_queue(struct wait_queue ** queue, struct task* tk,uint32_t *max_prio){
    struct wait_queue *temp=*queue;
    struct wait_queue *res=NULL;
    uint32_t t;
    t=dis_cpu_intr();
    if(max_prio) {
        *max_prio = 0;
    }
    while(temp){
        if ( temp->task->status != TASK_CLOSED
                ) {
            if(temp->task==tk){
                res=temp;
            }
            if (max_prio) {
                if (temp->task->prio > *max_prio) {
                    *max_prio = temp->task->prio;
                }
            }
        }
        temp=temp->next;
    }
    restore_cpu_inter(t);
    return res;
}
//移除一个等待的
void remove_wait_queue(struct wait_queue ** queue,struct wait_queue* add_queue){
    struct wait_queue *temp=*queue;
    struct wait_queue *prev=NULL;
    uint32_t t;
    if(!add_queue){
        return ;
    }
    t=dis_cpu_intr();
    while(temp){
        if(temp==add_queue) {
            if (prev==NULL) {
                //删除的第一个
                *queue=temp->next;
                break;
            }else{
                prev->next=temp->next;
                break;
            }
        }
        prev=temp;
        temp=temp->next;
    }
    restore_cpu_inter(t);
}

/**
 * @brief 调整当前任务优先级
 * @param increment
 * @return
 */
int32_t sys_nice (int32_t increment)
{
    int new_prio;
    new_prio=CUR_TASK->prio;
    if (new_prio- increment > 0)
        new_prio -= increment;

    if(task_change_prio(CUR_TASK,new_prio)<=0){
        return -ESRCH;
    }
    return 0;
}
extern void kernel_task_init(void);
// 调度程序的初始化子程序。
void init_sche(void)
{
    init_mem();
    /*OS是否调度初始化*/
    atomic_set(&sys_tasks_info.is_sche,1);
    /*进程pid分配变量*/
    atomic_set(&sys_tasks_info.pid_temp,1);
    sys_tasks_info.current_max_task_node=NULL;
    sys_tasks_info.tasks_count=0;
    kernel_task_init();
}
