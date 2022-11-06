//
// Created by Administrator on 2022/2/27.
//

#include <type.h>
#include <errno.h>
#include <mkrtos.h>
#include <mkrtos/sched.h>
#include <ipc/spin_lock.h>
#include <arch/arch.h>
#include <arch/atomic.h>

///ȫ��sleep�б�
static struct sleep_time_queue *slp_tim_ls=NULL;

static void add_sleep(struct sleep_time_queue **queue,struct sleep_time_queue *add){
    if(!queue){
        return ;
    }
    uint32_t t;
    t=dis_cpu_intr();
    if(!(*queue)){
        *queue=add;
    }else{
        add->next=*queue;
        *queue=add;
    }
    restore_cpu_intr(t);
}
static void remove_sleep(struct sleep_time_queue **queue,struct sleep_time_queue *rm){
    struct sleep_time_queue *temp;
    struct sleep_time_queue *prev=NULL;
    if(!queue){
        return ;
    }
    uint32_t t;
    t=dis_cpu_intr();
    temp=*queue;
    while(temp){
        if(temp==rm) {
            if (prev==NULL) {
                //ɾ���ĵ�һ��
                (*queue) = temp->next;
                break;
            }else{
                prev->next = temp->next;
                break;
            }
        }
        prev=temp;
        temp = temp->next;
    }
    restore_cpu_intr(t);
}
//ɾ��ĳ������ģ���exit.c�е���
void do_remove_sleep_tim(struct task* tk) {
    struct sleep_time_queue *temp;
    struct sleep_time_queue *prev=NULL;
    uint32_t t;
    t=dis_cpu_intr();
    temp=slp_tim_ls;
    while(temp){
        if(temp->task ==tk) {
            if (prev==NULL) {
                //ɾ���ĵ�һ��
                (slp_tim_ls) = temp->next;
                break;
            }else{
                prev->next = temp->next;
                break;
            }
        }
        prev=temp;
        temp = temp->next;
    }
    restore_cpu_intr(t);
}
void wake_up_sleep(struct sleep_time_queue **temp){
    uint32_t t;
    struct sleep_time_queue *queue;
    t=dis_cpu_intr();
    queue=*temp;
    while(queue){
        if(queue->task){
            if(queue->task->status==TASK_SUSPEND
            ||queue->task->status==TASK_UNINTR
            ){
                task_run_1(queue->task);
            }
        }
        queue=queue->next;
    }
    restore_cpu_intr(t);
}
//���tim�Ƿ�ʱ���ˣ�������ˣ�����ָ���Ľ���
//��������ڵ���ʱ��������е���
void _do_check_sleep_tim(struct sleep_time_queue **slp_ls){
    uint32_t t;
    struct sleep_time_queue *tmp;
    t=dis_cpu_intr();
    tmp=*slp_ls;
    while(tmp){
        tmp->cur_ms += OS_WORK_PERIOD_MS;
        if(tmp->cur_ms>=tmp->slp_ms){
            task_run_1(tmp->task);
        }
        tmp=tmp->next;
    }
    restore_cpu_intr(t);
}
void do_check_sleep_tim(void){
    _do_check_sleep_tim(&slp_tim_ls);
}
/**
 * ִ������
 */
int do_nanosleep(
		struct sleep_time_queue **slp_ls,
		const struct timespec *req,
		struct timespec *rem,
		struct atomic *lock,
		int check_val
        ){
    if(!req){
        return -EINVAL;
    }
    if(req->tv_nsec==0 && req->tv_sec==0){
        if(rem){
            rem->tv_sec=0;
            rem->tv_nsec=0;
        }
        return 0;
    }

    struct sleep_time_queue stq={
            .next=NULL,
            .task=get_current_task(),
            //���滻���ms������ȡ��
            .slp_ms=req->tv_sec*1000+ ROUND(ROUND(req->tv_nsec,1000),1000),
            .cur_ms=0
    };
    uint32_t t;
    add_sleep(slp_ls,&stq);
    if(lock) {
        t = dis_cpu_intr();
        if(atomic_test(lock,check_val)) {
            task_suspend();
            task_sche();
        }
        restore_cpu_intr(t);
    }else{
        task_suspend();
        task_sche();
    }
    remove_sleep(slp_ls,&stq);
    if(rem){
        uint32_t rems;
        rems=stq.slp_ms-stq.cur_ms;
        rem->tv_nsec=(rems%1000)*1000*1000;
        rem->tv_sec=rems/1000;
    }
#if 0
    if(CUR_TASK->sig_bmp[0]
    ||CUR_TASK->sig_bmp[1]
    ){
        return -EINTR;
    }
#endif
    return 0;
}

void sleep_ms(int ms) {
	struct timespec req={
			.tv_nsec = (ms%1000)*1000*1000,
			.tv_sec = ms/1000
	};
	while(req.tv_nsec != 0 || req.tv_sec != 0) {
		do_nanosleep(&slp_tim_ls,&req,&req,0,0);
	}
}
//��������req��ʱ�䣬������ź��жϣ���ʣ���ʱ��ŵ�rem�в�����-1
int sys_nanosleep(const struct timespec *req, struct timespec *rem){
   return do_nanosleep(&slp_tim_ls,req,rem,0,0);
}
//�����������ʹ����һ��������ڻ���ڵ�ǰ�̵߳��߳������С����û�з����������̣߳���ô��������������̷���Ȼ�����ִ�е�ǰ�̵߳ĳ���
int sys_sched_yield(void){
    task_sche();
    return 0;
}
