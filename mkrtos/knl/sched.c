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
* @brief ϵͳ�����
*/
struct sys_tasks sys_tasks_info={0};

/**
* @brief ͨ��PID�ҵ��������
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
//�������񲻿ɴ��
void task_unintr(void){
    if(CUR_TASK->status!=TASK_UNINTR
       &&CUR_TASK->status!=TASK_CLOSED
            ) {
        CUR_TASK->status = TASK_UNINTR;
    }
}
//���������
void task_suspend(void){
    if(CUR_TASK->status!=TASK_SUSPEND
     &&CUR_TASK->status!=TASK_CLOSED
     &&CUR_TASK->status!=TASK_UNINTR
    ) {
        sys_tasks_info.current_max_task_node->task_ready_count--;
        if(sys_tasks_info.current_max_task_node->task_ready_count==0){
            //�������
            update_cur_task();
        }
        CUR_TASK->status = TASK_SUSPEND;
    }
}
//����������
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
* @brief ������ȣ����������ȹرգ��������Ч
*/
void task_sche(void){
    if(sys_tasks_info.current_max_task_node->task_ready_count==0){
        return ;
    }
    //����Ƿ���Ե���
    if(atomic_test(&(sys_tasks_info.is_sche),TRUE)){
        _TaskSchedule();
    }
}
/**
* @brief ���¾�����������ȼ�������ڵ㣬���µ������о�������������0
* @param pSysTasks ����������
*/
void update_cur_task(void){

	struct sys_task_base_links* ptl;
    uint32_t t=dis_cpu_intr();

    ptl=sys_tasks_info.sysTaskBaseLinks.next;

    /*������ȼ�*/
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
                    /*����*/
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
* @brief ͨ�����ȼ��ҵ�������
* @param prio �������ȼ�
* @return ������ȼ�����ı�ͷ
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
* @brief ͨ�����ȼ��������ͷ�����ָ�����ȼ������ڣ��򴴽�һ�����ȼ��Ľڵ�
* @param prio ��ӵ����ȼ�
 * @return ��ӳɹ�������ͷ
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

    /*�������*/
    p_sys_task_base_links->next=sys_tasks_info.sysTaskBaseLinks.next;
    sys_tasks_info.sysTaskBaseLinks.next=p_sys_task_base_links;

    return p_sys_task_base_links;
}
/**
 * ɾ������
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
* @brief ͨ�����ȼ�����������������ȼ������ڣ��򴴽������ȼ�������ڵ�
* @param pSysTasks ����������
* @return ����Ƿ�ɹ�
*/
int32_t add_task(struct task* pTaskBlock,uint32_t into_all_ls){

    if(pTaskBlock==NULL){
        return -1;
    }
    /*�������ж�*/
    uint32_t t;
    t=dis_cpu_intr();
    struct sys_task_base_links* taskLinks;

    taskLinks = find_task_links(pTaskBlock->prio);
    if(taskLinks == NULL){
        taskLinks=add_links(pTaskBlock->prio);
        if(taskLinks==NULL){
            restore_cpu_inter(t);
            /*û���ڴ�*/
            return -ENOMEM;
        }
    }
    taskLinks->task_count++;
    taskLinks->task_ready_count++;

    //�ŵ�ͬ���ȼ�������������
    struct task* pstl=taskLinks->pSysTaskLinks;
    if(pstl == NULL){
        pTaskBlock->next=NULL;
        taskLinks->pSysTaskLinks=pTaskBlock;
    }else{
        /*���������ͷ*/
        pTaskBlock->next=pstl;
        taskLinks->pSysTaskLinks=pTaskBlock;
    }
    pTaskBlock->parent=taskLinks;

    if(into_all_ls) {
        //�浽���������������
        pstl = sys_tasks_info.all_task_list;
        if (pstl == NULL) {
            pTaskBlock->nextAll = NULL;
            sys_tasks_info.all_task_list = pTaskBlock;
        } else {
            /*���������ͷ*/
            pTaskBlock->nextAll = pstl;
            sys_tasks_info.all_task_list = pTaskBlock;
        }
    }
    restore_cpu_inter(t);

    //�������ȼ�
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
* @brief ��������м��
*/
void tasks_check(void){
    struct task* ptb;
    sys_tasks_info.sys_run_count++;
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
        //û���㹻���ڴ��ˣ��ָ�֮ǰ��
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
* @brief ִ��������ȣ�������һ�������ջ��
* @brief SP ��ǰ�̵߳�ջ����ַ
* @brief SPType ʹ�õ���MSP����PSP 0ʹ��msp 1ʹ��psp
* @return ����ջ��
*/
struct _stackInfo* sys_task_sche(void* psp,void* msp,uint32_t spType){
    uint16_t svc_status=FALSE;
    volatile uint32_t *svcPendReg=(uint32_t*)(0xE000ED24);
    //����svc״̬
    if((*svcPendReg)&0x80L){
        (*svcPendReg)&=~(0x80L);
        svc_status=TRUE;
    }
    if(sys_tasks_info.isFirst==FALSE){
        /*��һ��δ���䣬�����һ��*/
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
        /*�޸ĵ�ǰ�̵߳�ջ����ַ*/
        sys_tasks_info.current_task->sk_info.psp_stack = psp;
        sys_tasks_info.current_task->sk_info.msp_stack = msp;
        if (sys_tasks_info.current_task->sk_info.stack_type != 2) {
            sys_tasks_info.current_task->sk_info.stack_type = spType;
        }
        sys_tasks_info.current_task->sk_info.svc_status = svc_status;
        /*��ǰ����û�д���TASK_UNINTR״̬�ſ��Խ����л�����Ȼ���ܹ���*/
        if(CUR_TASK->status!=TASK_UNINTR) {
            /*֮ǰ�������ֱ������һ����Ч��*/
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
    //�ָ������svc״̬
    if(sys_tasks_info.current_task->sk_info.svc_status==TRUE){
        (*svcPendReg)|=0x80L;
        (*svcPendReg)&=~(0x100L);
    }
    sys_tasks_info.current_task->runCount++;
    /*���ض�ջ�ĵ�ַ*/
    return &(sys_tasks_info.current_task->sk_info);
}

// ϵͳ���ù��� - ���ñ�����ʱʱ��ֵ(��)��
// ����Ѿ����ù�alarm ֵ���򷵻ؾ�ֵ�����򷵻�0��
int32_t sys_alarm (uint32_t seconds){
    uint32_t old = 0;
    if(sys_tasks_info.current_task->alarm){
        old=sys_tasks_info.current_task->alarm;
    }
    sys_tasks_info.current_task->alarm = sys_tasks_info.sys_run_count+(seconds*1000)/(1000/OS_WORK_HZ);
    return old;
}
// ������̵ȴ��ź�
int32_t sys_pause(void){
    task_suspend();
//    CUR_TASK->status=TASK_SUSPEND;
    task_sche();
    return -1;
}

//���Ѷ��������е�����
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

//���һ�����ȴ�������
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
//�ҵ�һ��queue������ȡ������ȼ�
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
//�Ƴ�һ���ȴ���
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
                //ɾ���ĵ�һ��
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
 * @brief ������ǰ�������ȼ�
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
// ���ȳ���ĳ�ʼ���ӳ���
void init_sche(void)
{
    init_mem();
    /*OS�Ƿ���ȳ�ʼ��*/
    atomic_set(&sys_tasks_info.is_sche,1);
    /*����pid�������*/
    atomic_set(&sys_tasks_info.pid_temp,1);
    sys_tasks_info.current_max_task_node=NULL;
    sys_tasks_info.tasks_count=0;
    kernel_task_init();
}
