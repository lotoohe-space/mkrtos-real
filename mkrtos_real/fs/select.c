//
// Created by Administrator on 2022/3/26.
//
#include <sys/select.h>
#include <errno.h>
#include <mkrtos/sched.h>
#include <sys/poll.h>

static int get_bit(unsigned long bits[],int inx){
    return (bits[inx/NFDBITS]>>(inx%NFDBITS))&0x1L;
}
//�������select�Ĳ�����
//��1��intmaxfdp��һ������ֵ����ָ�����������ļ��������ķ�Χ���������ļ������������ֵ��1�����ܴ�
//˵�����������ԭ��Ľ��Ϳ��Կ��ϱ�fd_set����ϸ���ͣ�fd_set����λͼ����ʽ���洢��Щ�ļ���������maxfdpҲ���Ƕ�����λͼ����Ч��λ�ĸ�����
//��2��fd_set*readfds��ָ��fd_set�ṹ��ָ�룬���������Ӧ�ð����ļ���������������Ҫ������Щ�ļ��������Ķ��仯�ģ������ǹ����Ƿ���Դ���Щ�ļ��ж�ȡ������
// ����������������һ���ļ��ɶ���select�ͻ᷵��һ������0��ֵ����ʾ���ļ��ɶ������û�пɶ����ļ��������timeout�������ж��Ƿ�ʱ��������timeout��ʱ��
// ��select����0�����������󷵻ظ�ֵ�����Դ���NULLֵ����ʾ�������κ��ļ��Ķ��仯��
//��3��fd_set*writefds��ָ��fd_set�ṹ��ָ�룬���������Ӧ�ð����ļ���������������Ҫ������Щ�ļ���������д�仯�ģ������ǹ����Ƿ��������Щ�ļ���д��������
// ����������������һ���ļ���д��select�ͻ᷵��һ������0��ֵ����ʾ���ļ���д�����û�п�д���ļ��������timeout�������ж��Ƿ�ʱ
// ��������timeout��ʱ�䣬select����0�����������󷵻ظ�ֵ�����Դ���NULLֵ����ʾ�������κ��ļ���д�仯��
//��4��fd_set*errorfdsͬ����������������ͼ�����������ļ������쳣�ļ���
//��5��structtimeval* timeout��select�ĳ�ʱʱ�䣬�������������Ҫ��������ʹselect��������״̬
// ����һ������NULL���βδ��룬��������ʱ��ṹ�����ǽ�select��������״̬��һ���ȵ������ļ�������������ĳ���ļ������������仯Ϊֹ��
//  �ڶ�������ʱ��ֵ��Ϊ0��0���룬�ͱ��һ������ķ����������������ļ��������Ƿ��б仯�������̷��ؼ���ִ�У��ļ��ޱ仯����0���б仯����һ����ֵ��������timeout��ֵ����0������ǵȴ��ĳ�ʱʱ�䣬�� select��timeoutʱ������������ʱʱ��֮�����¼������ͷ����ˣ������ڳ�ʱ�󲻹�����һ�����أ�����ֵͬ������
//˵����
//�������أ�
//��1�������ӵ���Ӧ���ļ�������������������ʱ������˵���ļ����������������ݵ���ʱ���ں�(I/O)����״̬�޸��ļ�����������������һ������0������
//��2����û�������������ļ��������������õ�timeval���ʱ�䳬ʱʱ��select�����᷵��һ��Ϊ0��ֵ��
//��3����select���ظ�ֵʱ����������
int file_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,struct timeval *timeout){
    int ret;
    //flag��
    uint32_t  flag_tb[FD_SETSIZE];
    if(!readset && !writeset){
        return -EINVAL;
    }
    struct file *tk;
    tk=get_current_task()->files;
    int i;
    if(readset) {
        for (i = 0; i < maxfdp1; i++) {
            int bit;
            bit=get_bit(readset->fds_bits,i);
            if(!bit){
                continue;
            }
            if(!tk[i].used){
                continue;
            }
            if(!(tk[i].f_op
            &&tk[i].f_inode
            &&tk[i].f_op->poll
            )){
                continue;
            }

            //3����־ͬʱ����
            flag_tb[i]|=POLLIN;
            flag_tb[i]|=POLLRDNORM;
            flag_tb[i]|=POLLPRI;
            flag_tb[i]|=POLLERR;
        }
    }
    if(writeset){
        for(i=0;i<maxfdp1;i++){
            int bit;
            bit=get_bit(writeset->fds_bits,i);
            if(!bit){
                continue;
            }
            if(!tk[i].used){
                continue;
            }
            if(!(tk[i].f_op
                 &&tk[i].f_inode
                 &&tk[i].f_op->poll
            )){
                continue;
            }
            flag_tb[i]|=POLLOUT;
            flag_tb[i]|=POLLERR;
        }
    }
    //�����û���¼�����
    //TODO:

    for(int i=0;i<FD_SETSIZE;i++) {
        if (timeout||(timeout->tv_usec == 0 && timeout->tv_sec == 0)) {
            //����
            ret = tk[i].f_op->poll(tk[i].f_inode, tk + i, flag_tb[i], timeout);
            if (ret < 0) {
                return ret;
            }
        } else{
            //�����ߣ���ֱ�Ӽ��
        }
    }
    return 0;
}

