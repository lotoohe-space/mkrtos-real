//
// Created by Administrator on 2022/1/12.
//

#ifndef UNTITLED1_IPC_H
#define UNTITLED1_IPC_H
#include <type.h>
#include <arch/atomic.h>
#include <sys/sem.h>


//mutex.c
struct mutex{
    //ʹ�ô���
    Atomic_t s_used_count;
    //��ס
    Atomic_t m_lock;
    //˭����smutx
    struct task* m_who_lock;
    //����֮ǰ�����ȼ�
    uint32_t m_tk_prev_prio;
    //������ܵĵȴ�����
    struct wait_queue *m_wait;
};

int32_t mutex_get(sem_t mid);
int32_t mutex_put(sem_t mid) ;
int32_t lock_mutex(int32_t mt_l);
int32_t unlock_mutex(int32_t mt_l);


int sys_msgget(key_t key,int flag);
int sys_msgctl(int msgid,int cmd,struct msqid_ds *buf);
int sys_msgrcv(int msgid,void *ptr,size_t nbytes,long type,int flag);
int sys_msgsnd(int msgid,const void *ptr,size_t nbytes,int flag);


int32_t sys_semget(key_t key,int nsems,int flag);
int32_t sys_semctl(int semid,int semnum,int cmd,union semun arg);
int sys_semop(int semid,struct sembuf semoparray[],size_t ops);


#endif //UNTITLED1_IPC_H
