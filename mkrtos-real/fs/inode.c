//
// Created by Administrator on 2022/1/12.
//

#include <mkrtos/fs.h>
#include <mm.h>
#include <ipc/ipc.h>
#include <mkrtos/sched.h>
#include <string.h>
#include <arch/atomic.h>

//ϵͳ��֧�ֵ����inode����
#define INODE_NUM 12
static struct inode inode_ls[INODE_NUM];

//���е�inode����
static struct atomic inode_free_num={
        .counter=INODE_NUM
};
static struct wait_queue* ino_ls=NULL;
static struct atomic ino_ls_lock={0};

//��סinode����
static void __wait_on_inode_list(void){
    struct wait_queue wait = {get_current_task() , NULL };

    add_wait_queue(&ino_ls, &wait);
    again:
    task_suspend();
    if (atomic_read(&( ino_ls_lock))) {
        task_sche();
        goto again;
    }
    remove_wait_queue(&ino_ls, &wait);
    task_run();
}
void wake_up_wait_inode_list(void){
    wake_up(ino_ls);
}
/**
 * ��ס���inode���õ�ʱ�����
 * @param p_inode
 */
void locki(struct inode* p_inode){
    atomic_set(&(p_inode->i_lock),1);
}
/**
 * ����
 * @param p_inode
 */
void unlocki(struct inode* p_inode){
    atomic_set(&(p_inode->i_lock),0);
}
/**
 * ��ȡһ���յ�inode
 * @return
 */
struct inode* get_empty_inode(void){
    uint32_t i;
    if(atomic_read(&inode_free_num)==0){
        //��������Ҳ�����Ӧ����ϵͳ���ߵȴ��ͷ��źŵ���
        __wait_on_inode_list();
    }
    again:
    for(i=0;i<INODE_NUM;i++){
        if(atomic_test_inc(&(inode_ls[i].i_used_count))){
            static struct inode tmp={.i_used_count={1}};
            atomic_dec(&inode_free_num);
            mkrtos_memcpy(&(inode_ls[i]),&tmp,sizeof(struct inode));
            return &(inode_ls[i]);
        }
    }
    //��������Ҳ�����Ӧ����ϵͳ���ߵȴ��ͷ��źŵ���
    __wait_on_inode_list();
    //���²��ҿ��õ�
    goto again;
}
/**
 * ָ���豸��inode,����i_used_count++
 * @param ino
 * @return
 */
struct inode* get_inode(dev_t dev_no,ino_t ino){
    uint32_t i;
    for(i=0;i<INODE_NUM;i++){
        if(atomic_read(&(inode_ls[i].i_used_count))
        && inode_ls[i].i_sb->s_dev_no==dev_no
        && inode_ls[i].i_no==ino
        ){
            atomic_inc(&(inode_ls[i].i_used_count));
            return &(inode_ls[i]);
        }
    }
    return NULL;
}
/**
 * ����һ��inode
 * @param p_inode
 */
void lose_inode(struct inode* p_inode){
    //�����������
    atomic_test_dec_nq(&(p_inode->i_used_count));
    if(atomic_read(&(p_inode->i_used_count))==0){
        if(p_inode->i_sb->s_ops
        &&p_inode->i_sb->s_ops->free_inode
        ) {
            p_inode->i_sb->s_ops->free_inode(p_inode);
        }
        atomic_inc(&inode_free_num);
        //�ͷŵȴ��Ľ���
        wake_up_wait_inode_list();
    }
}
/**
 * ͬ�����е�inode
 */
int sync_all_inode(void){
    int i;
    for(i=0;i<INODE_NUM;i++){
        if(!atomic_test(&(inode_ls[i].i_used_count),0)){
           // lock_inode(&inode_ls[i]);
//        	sche_lock();
            if(inode_ls[i].i_sb
            &&inode_ls[i].i_sb->s_ops
            &&inode_ls[i].i_sb->s_ops->write_inode
            ) {
                inode_ls[i].i_sb->s_ops->write_inode(&inode_ls[i]);
            }
//            sche_unlock();
           // unlock_inode(&inode_ls[i]);
        }
    }
    return 0;
}
/**
 * ��ȡָ��sb��inode����
 * @param p_sb
 * @param ino
 * @return
 */
struct inode* geti(struct super_block* p_sb,ino_t ino){

    struct inode* r_inode= NULL;

    r_inode=get_inode(p_sb->s_dev_no,ino);
    if(r_inode!=NULL){
        if (r_inode->i_mount) {
            //����������ˣ����ȡ���صĵ�
            struct inode * tmp = r_inode->i_mount;
            atomic_inc(&(tmp->i_used_count));
            puti(r_inode);
            r_inode = tmp;
        }
        return r_inode;
    }

    //����û�У������¶�ȡһ��
    r_inode=get_empty_inode();
    //ʹ�ü���
    atomic_set(&(r_inode->i_used_count),1);
    atomic_set(&(r_inode->i_lock),0);

    r_inode->i_file_size=0;
    r_inode->i_wait_q=NULL;
    //������
    r_inode->i_sb=p_sb;
    r_inode->i_no=ino;
    r_inode->i_hlink=0;
    //����һ��inode
    if(p_sb->s_ops->alloc_inode(r_inode) == NULL){
        lose_inode(r_inode);
        //����ʧ��
        return NULL;
    }
   // lock_inode(r_inode);
    //��ȡinode
    if(p_sb->s_ops->read_inode(r_inode)<0){
        p_sb->s_ops->free_inode(r_inode);
        lose_inode(r_inode);
        unlock_inode(r_inode);
        return NULL;
    }
   // unlock_inode(r_inode);

    return r_inode;
}
/**
 * �ͷ�һ��inode���õ�һ�����е�inode
 * @param put_inode
 * @return
 */
int32_t puti(struct inode* put_inode){

    if(!put_inode){
        return -1;
    }
    wait_on_inode(put_inode);

    if(!atomic_read(&( put_inode->i_used_count))){
        //�ͷŵĿյ�inode������һ�������
        return -1;
    }

    //����1��ֱ�Ӽ�1������
    if(atomic_read(&( put_inode->i_used_count))>1){
        lose_inode(put_inode);
        return 0;
    }

    //����1����Ҫд��inode
    if(put_inode
        &&put_inode->i_sb
        &&put_inode->i_sb->s_ops
        &&put_inode->i_sb->s_ops->put_inode
    ){
        //Ӳ����������������ͷ����inode
        if(put_inode->i_hlink==0) {
            put_inode->i_sb->s_ops->put_inode(put_inode);
        }
    }

    if(put_inode
       &&put_inode->i_sb
       &&put_inode->i_sb->s_ops
       &&put_inode->i_sb->s_ops->write_inode
    ){
    	//������д�뵽����֮��
        put_inode->i_sb->s_ops->write_inode(put_inode);
    }

    lose_inode(put_inode);
    return 0;
}

static void __wait_on_inode(struct inode * inode)
{
    struct wait_queue wait = {get_current_task() , NULL };

    add_wait_queue(&inode->i_wait_q, &wait);
    again:
    task_suspend();
    if (atomic_read(&( inode->i_lock))) {
        task_sche();
        goto again;
    }
    remove_wait_queue(&inode->i_wait_q, &wait);
    task_run();
}
void wait_on_inode(struct inode* inode){
    if (atomic_read(&( inode->i_lock))) {
        __wait_on_inode(inode);
    }
}

void lock_inode(struct inode* inode){
    wait_on_inode(inode);
    atomic_set(&inode->i_lock,1);
}

void unlock_inode(struct inode* inode){
    atomic_set(&inode->i_lock,0);
    wake_up(inode->i_wait_q);
}

