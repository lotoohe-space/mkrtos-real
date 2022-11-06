//
// Created by Administrator on 2022/3/26.
//

#include <mkrtos/fs.h>
#include <mkrtos/sched.h>
#include <mm.h>

/**
 * �ļ�ϵͳ�ĵȴ�����
 */
struct sp_wait_queue{
    struct wait_queue wait;
    uint32_t flag;
    uint32_t mask;
};

//POLLIN           �����ݿɶ�
//POLLRDNORM ����ͨ���ݿɶ�����Ч��POLLIN
//POLLPRI         �н������ݿɶ�
//POLLOUT        д���ݲ��ᵼ������
//POLLER          ָ�����ļ���������������
//POLLHUP        ָ�����ļ������������¼�
//POLLNVAL      ��Ч�����󣬴򲻿�ָ�����ļ�������
/**
 * �ļ�ϵͳ��poll
 */
static int sp_poll(struct inode * inode,struct file *fp, uint32_t flag,struct timeval *timeout){
//    int ret=0;

//    if(!timeout) {
        struct sp_wait_queue *wait;
        wait=malloc(sizeof(struct sp_wait_queue));
        if(!wait){
            return -ENOMEM;
        }
        wait->wait.task=get_current_task();
        wait->wait.next=NULL;
        wait->mask=0;
        wait->flag=flag;

        add_wait_queue(&inode->poll_wait, &wait);
//        task_suspend();
//        remove_wait_queue(&inode->poll_wait, &wait);
//    }else{
//        extern int sys_nanosleep(const struct timespec *req, struct timespec *rem);
//        struct timespec times;
//        times.tv_sec=timeout->tv_sec;
//        times.tv_nsec=timeout->tv_usec*1000;
//        //����һ��ʱ��
//        ret=sys_nanosleep(&times,NULL);
//
//    }
    return 0;
}
//int sp_select(struct inode * inode, struct file * fp, int, uint32_t *){
//
//}

