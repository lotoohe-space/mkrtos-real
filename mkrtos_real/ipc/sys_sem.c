//
// Created by Administrator on 2022/2/25.
//

#include <sys/sem.h>
#include <errno.h>
#include <arch/atomic.h>
#include <knl_service.h>
#include <mkrtos/sched.h>
#include <arch/arch.h>

#define SEM_NUM 4
static struct semid_ds semid_ds_ls[SEM_NUM]={0};
static struct atomic semid_ds_used[SEM_NUM]={0};
static struct atomic sem_list_lk={0};

static void lock_ls(void){
    while(!atomic_test_set(&sem_list_lk,1));
}
static void unlock_ls(void){
    atomic_set(&sem_list_lk,0);
}
static void lock_sem(struct sem* _sem){
    while(!atomic_test_set(&_sem->semlock,1));
}
static void unlock_sem(struct sem* _sem){
    atomic_set(&_sem->semlock,0);
}
static int32_t inner_find_sem(key_t key){
    int i;

    lock_ls();
    for(i=0;i<SEM_NUM;i++){
        if (!atomic_test(&semid_ds_used[i],0)) {
            if (semid_ds_ls[i].sem_perm.key == key) {
                atomic_set(&sem_list_lk, 0);
                return i;
            }
        }
    }
    unlock_ls();

    return -1;
}

static int32_t inner_sem_creat(key_t key,int nsems,int flag){
    int i;

    lock_ls();
    for(i=0;i<SEM_NUM;i++){
        if (atomic_test_set(&semid_ds_used[i],1)) {
            break;
        }
    }
    unlock_ls();
    if(i==SEM_NUM) {
        return -ENOENT;
    }
    semid_ds_ls[i].sem_base=malloc(nsems);
    if(!semid_ds_ls[i].sem_base) {
        atomic_set(&semid_ds_used[i],0);
        return -ENOMEM;
    }
    semid_ds_ls[i].sem_perm.cgid=get_current_task()->egid;
    semid_ds_ls[i].sem_perm.cuid=get_current_task()->euid;
    semid_ds_ls[i].sem_perm.gid=get_current_task()->rgid;
    semid_ds_ls[i].sem_perm.uid=get_current_task()->ruid;
    semid_ds_ls[i].sem_perm.key=key;
    semid_ds_ls[i].sem_perm.mode=flag;
    semid_ds_ls[i].sem_perm.seq=0;

    semid_ds_ls[i].sem_ctime=0;
    semid_ds_ls[i].sem_nsems=nsems;
    semid_ds_ls[i].sem_otime=0;
    semid_ds_ls[i].sem_pending=0;
    semid_ds_ls[i].sem_pending_last=0;
    semid_ds_ls[i].undo=0;
//    atomic_set(&sem_list_lk,0);

    return i;
}

#define ABS(a) ((a)<0?(-(a)):(a))

//���Ѷ��������е�����
void sem_wake_up(struct sem_queue *queue,int semnum){
    uint32_t t;
    t=dis_cpu_intr();
    while(queue){
        if(queue->task){
            if(
                    ( queue->semid==semnum ||semnum<0)
                    &&(queue->task->status==TASK_SUSPEND
                        ||queue->task->status==TASK_UNINTR)
                    ){
                task_run_1(queue->task);
            }
        }
        queue=queue->next;
    }
    restore_cpu_intr(t);
}

//���һ�����ȴ�������
void sem_add_wait_queue(struct sem_queue** queue,struct sem_queue* add_queue){
    uint32_t t;
    t=dis_cpu_intr();
    if(*queue==NULL){
        *queue=add_queue;
    }else{
        add_queue->next = (*queue);
        *queue=add_queue;
    }
    restore_cpu_intr(t);
}
//�Ƴ�һ���ȴ���
void sem_remove_wait_queue(struct sem_queue ** queue,struct sem_queue* add_queue){
    struct sem_queue *temp=*queue;
    struct sem_queue *prev=NULL;
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
    restore_cpu_intr(t);
}


int32_t sys_semget(key_t key,int nsems,int flag){
    struct semid_ds *sem_ds;
    int id;
    if(key==IPC_PRIVATE){
        id=inner_sem_creat(key,nsems,flag);
        if(id<0) {
            return id;
        }
    }else{
        id=inner_find_sem(key);
        if(!flag){
            if(id<0) {
                return -ENOENT;
            }
            return id;
        }else {
            //��ȡһ�����ڵ�
            if (flag & IPC_CREAT) {
                if (flag & IPC_EXCL) {
                    if(id>=0) {
                        return -ENOENT;
                    }
                    id=inner_sem_creat(key,nsems,flag);
                    if(id<0) {
                        return id;
                    }
                    return 0;
                }
                if(id>=0){
                    return id;
                }
                id=inner_sem_creat(key,nsems,flag);
                return id;
            }
            return id;
        }
    }
    return -ENOMEM;
}
static struct atomic sem_ds_lock[SEM_NUM]={0};
static void lock_sem_ds(int ds_no){
    while(atomic_test_set(&sem_ds_lock[ds_no],1));
}
static void unlock_sem_ds(int ds_no){
    atomic_set(&sem_ds_lock[ds_no],0);
}
int32_t sys_semctl(int semid,int semnum,int cmd,union semun arg){
    int i;
    if(semid<0||semid>=SEM_NUM){
        return -1;
    }
    if(atomic_test(&semid_ds_used[semid],0)){
        return -EIDRM;
    }

    switch (cmd){
        case IPC_STAT:
            lock_sem_ds(semid);
            mkrtos_memcpy(arg.buf,&semid_ds_ls[semid],sizeof(struct semid_ds));
            unlock_sem_ds(semid);
            break;
        case IPC_SET:
            lock_sem_ds(semid);
            if(get_current_task()->ruid==semid_ds_ls[semid].sem_perm.cuid
            ||get_current_task()->ruid==semid_ds_ls[semid].sem_perm.uid
            //||get_current_task()->is_s_user
            ){
                struct semid_ds *ds;
                ds=(struct semid_ds*)arg.buf;
                semid_ds_ls[semid].sem_perm.uid=ds->sem_perm.uid;
                semid_ds_ls[semid].sem_perm.gid=ds->sem_perm.gid;
                semid_ds_ls[semid].sem_perm.mode=ds->sem_perm.mode;
            }
            unlock_sem_ds(semid);
            break;
        case IPC_RMID:
            //�������еȴ����ź����Ľ���
            sem_wake_up(semid_ds_ls[semid].sem_pending,-1);
            //ɾ�����ź���
            atomic_set(&semid_ds_used[semid],0);
            //�ͷ��ڴ�
            free(semid_ds_ls[semid].sem_base);
            break;
        case GETVAL:
            if(semnum>=semid_ds_ls[semid].sem_nsems){
                return -1;
            }
            return semid_ds_ls[semid].sem_base[semnum].semval;
//            break;
        case SETVAL:
            if(semnum>=semid_ds_ls[semid].sem_nsems){
                return -1;
            }
            lock_sem(&semid_ds_ls[semid].sem_base[semnum]);
            semid_ds_ls[semid].sem_base[semnum].semval=arg.val;
            unlock_sem(&semid_ds_ls[semid].sem_base[semnum]);
            break;
        case GETPID:
            if(semnum>=semid_ds_ls[semid].sem_nsems){
                return -1;
            }
            return semid_ds_ls[semid].sem_base[semnum].sempid;
//            break;
        case GETNCNT:
            if(semnum>=semid_ds_ls[semid].sem_nsems){
                return -1;
            }
            return semid_ds_ls[semid].sem_base[semnum].semncnt;
//            break;
        case GETZCNT:
            if(semnum>=semid_ds_ls[semid].sem_nsems){
                return -1;
            }
            return semid_ds_ls[semid].sem_base[semnum].semzcnt;
//            break;
        case GETALL:
            if(semnum>=semid_ds_ls[semid].sem_nsems){
                return -1;
            }
            for(i=0;i<semid_ds_ls[semid].sem_nsems;i++){
                arg.array[i]=semid_ds_ls[semid].sem_base[i].semval;
            }
            break;
        case SETALL:
            if(semnum>=semid_ds_ls[semid].sem_nsems){
                return -1;
            }
            for(i=0;i<semid_ds_ls[semid].sem_nsems;i++){
                lock_sem(&semid_ds_ls[semid].sem_base[i]);
                semid_ds_ls[semid].sem_base[i].semval=arg.array[i];
                unlock_sem(&semid_ds_ls[semid].sem_base[i]);
            }
            break;
    }

    return 0;
}

int sys_semop(int semid,struct sembuf semoparray[],size_t ops){
    struct semid_ds *sem_ds;
    struct sem* _sem;
    if(semid<0||semid>=SEM_NUM){
        return -1;
    }
    if(atomic_test(&semid_ds_used[semid],0)){
        return -EIDRM;
    }
    sem_ds=&semid_ds_ls[semid];

    for(int i=0;i<ops;i++){
        _sem=&semid_ds_ls[semid].sem_base[semoparray[i].sem_num];

        //���һ�β����Ľ���pid
        sem_ds->sem_base[semoparray[i].sem_num].sempid=get_current_task()->pid;
        if(semoparray[i].sem_op>0){
            lock_sem(_sem);
            if(semoparray[i].sem_num>=semid_ds_ls[semid].sem_nsems){
                return -1;
            }
            if(semoparray->sem_flg&SEM_UNDO){
                //�������ȥ
                _sem->semval-=semoparray[i].sem_op;
            }else{
                //��֮����
                _sem->semval+=semoparray[i].sem_op;
            }

            //����Ƿ��ڵȴ�ĳ����Դ��λ
            if(_sem->semval>=ABS(semoparray[i].sem_op)) {
                //ֵ��1
                if(_sem->semncnt>0){
                    _sem->semncnt--;
                }
                //�����ͷ�����Դ�����ѵȴ��Ľ���
                sem_wake_up(semid_ds_ls[semid].sem_pending,semoparray[i].sem_num);
            }

            //����ź����Ƿ���
            if(_sem->semval==0) {
                //ֵ��1
                if(_sem->semzcnt>0){
                    _sem->semzcnt--;
                }
                //�����ͷ�����Դ�����ѵȴ��Ľ���
                sem_wake_up(semid_ds_ls[semid].sem_pending,semoparray[i].sem_num);
            }
            unlock_sem(_sem);
        }else if(semoparray[i].sem_op==0){
            again_0:
            lock_sem(_sem);
            if(!_sem->semval){
                unlock_sem(_sem);
                return 0;
            }
            //ϣ���ȴ������ź���ֵ���0
            if(semid_ds_ls[semid].sem_perm.mode&IPC_NOWAIT){
                unlock_sem(_sem);
                return -EAGAIN;
            }
            _sem->semzcnt++;
            unlock_sem(_sem);
            //�����������
            struct sem_queue sem_q={semoparray[i].sem_num,get_current_task(),NULL};
            sem_add_wait_queue(&semid_ds_ls[semid].sem_pending,&sem_q);
            task_suspend();
            task_sche();
            task_run();
            sem_remove_wait_queue(&semid_ds_ls[semid].sem_pending,&sem_q);

            //�źű�ɾ���ˣ��򷵻�ERMID
            if(atomic_test(&semid_ds_used[semid],0)){
                return -ERMID;
            }
            //���źţ�����EINTR
            if(get_current_task()->sig_bmp[0]
               ||get_current_task()->sig_bmp[1]){
                _sem->semzcnt--;
                return -EINTR;
            }
            goto again_0;
        } if(semoparray[i].sem_op<0){
            again_get:
            lock_sem(_sem);
            //���ھ���ֵ
            //����Ӧ��ԭ�Ӳ�����
            //��ס��ǰ���źż�
            if(_sem->semval>=ABS(semoparray[i].sem_op)) {
                //��ֱ�ӽ��в���SOFTFPU
                if (semoparray->sem_flg & SEM_UNDO) {
                    _sem->semval += ABS(semoparray[i].sem_op);
                } else {
                    _sem->semval -= ABS(semoparray[i].sem_op);
                }
            }else{
                //��֮˵����Դ����
                if(semid_ds_ls[semid].sem_perm.mode&IPC_NOWAIT){
                    unlock_sem(_sem);
                    return -EAGAIN;
                }
                //Ҫ���������
                _sem->semncnt++;
                unlock_sem(_sem);
                //�����������
                struct sem_queue sem_q={semoparray[i].sem_num,get_current_task(),NULL};
                sem_add_wait_queue(&semid_ds_ls[semid].sem_pending,&sem_q);
                task_suspend();
                task_sche();
                task_run();
                sem_remove_wait_queue(&semid_ds_ls[semid].sem_pending,&sem_q);

                //�ź�����ɾ���ˣ��򷵻�ERMID
                if(atomic_test(&semid_ds_used[semid],0)){
                    return -ERMID;
                }
                //���źţ�����EINTR
                if(get_current_task()->sig_bmp[0]
                   ||get_current_task()->sig_bmp[1]){
                    _sem->semncnt--;
                    return -EINTR;
                }
                goto again_get;
            }
        }
    }
    return 0;
}

