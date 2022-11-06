
#include <mkrtos/bk.h>
#include <type.h>
#include <mkrtos/sched.h>
#include <mkrtos/fs.h>
#include <string.h>
#include <stdlib.h>
#include <mm.h>
#include <ipc/ipc.h>
#include "arch/arch.h"
#include <mkrtos/debug.h>
#include <mkrtos.h>


static struct bk_cache* find_bk_cache(dev_t dev_no,uint32_t bk_no);
//static struct bk_cache* sync_one_bk(dev_t dev_no);
/**
 * �黺���ʼ��
 * @param p_bk_ch_ls
 * @param cache_len
 * @return
 */
int32_t bk_cache_init(struct bk_cache** p_bk_ch_ls,uint32_t cache_len,uint32_t bk_size){
    uint32_t i;

    *p_bk_ch_ls = malloc(sizeof(struct bk_cache)*cache_len);
    if(*p_bk_ch_ls == NULL){
        return -1;
    }
    for(i=0;i<cache_len;i++){
        struct bk_cache* pBkCache=&((*p_bk_ch_ls)[i]);
        mkrtos_memset(pBkCache,0,sizeof(struct bk_cache));
        pBkCache->cache=malloc(bk_size);
        if(pBkCache->cache==NULL){
            break;
        }
        pBkCache->bk_size=bk_size;
    }
    if(i==0){
        free(*p_bk_ch_ls);
    }
    return i;
}
/**
* @brief ����BkCache
*/
int32_t bk_cache_destory(struct bk_cache* p_bk_ch_ls,uint32_t cache_len){
    if(p_bk_ch_ls==NULL){
        return -1;
    }

    for(uint32_t i=0;i<cache_len;i++){
        free((p_bk_ch_ls[i].cache));
    }
    free(p_bk_ch_ls);
    return 0;
}
/**
 * hash����
 * @param bk_no
 * @param max
 * @return
 */
int bk_hash(int bk_no,int max){
    return bk_no%max;
}
/**
 * hash����һ��bk����ȷ��ls����ס
 * @param dev_no
 * @param bk_no
 * @return
 */
static struct bk_cache* search_bk(struct bk_cache* bk_cache_ls,uint32_t cache_len,dev_t dev_no,uint32_t bk_no){
    int i;
    int inx;
    i=0;
    //�Ȳ���
    inx = bk_hash(bk_no, cache_len);
    while(!IS_NULLKEY(bk_cache_ls[inx])) {
        //û�б�ɾ�����ұ�ʹ�ã���ֱ�ӷ���
        if (!IS_DELKEY(bk_cache_ls[inx])
            && bk_cache_ls[inx].bk_no == bk_no) {
            return &bk_cache_ls[inx];
        }
        inx = bk_hash(inx + 1, cache_len);
        i++;
        if(i>=cache_len){
            break;
        }
    }
    return NULL;
}
/**
 * ռ��ָ��bk�������Զ��Ľ��ռ�ã����ǽ����Լ��ͷŻ��߽��̹ر�
 * @param dev_no
 * @return
 */
struct bk_cache* occ_bk(dev_t dev_no,bk_no_t bk_no,void* addr){
    struct bk_cache* bk_ch;
    struct bk_operations *bk_ops;
    if(!addr){
        return NULL;
    }
    bk_ops=get_bk_ops(dev_no);
    again:
    bk_ch=find_bk_cache(dev_no,bk_no);
    if(bk_ch==NULL){
        //û�����ͷ�һ��
        bk_ch=sync_rand_bk(dev_no);
        if(bk_ch==NULL) {
            return NULL;
        }
        goto again;
    }
    lock_bk(bk_ch);
    //�ͷ�֮ǰ�Ļ��棬��ʹ��addr�ṩ�Ļ����ַ�������ܹ���֤���û��Ļ���������
    bk_ch->oldcache = bk_ch->cache;
    bk_ch->cache = addr;
    if (!GET_BIT(bk_ch->flag, 2)) {
        if (bk_ops->read_bk(bk_no, bk_ch->cache) < 0) {
        }
        SET_BIT(bk_ch->flag, 2);
    }
    SET_BIT(bk_ch->flag,1);
    SET_MMAP(*bk_ch);
    unlock_bk(bk_ch);
    return bk_ch;
}
int32_t rel_bk1(dev_t dev_no,void* addr){
    struct bk_cache* bk_cache_ls;
    struct bk_operations *bk_ops;
    int cache_len;
    int i;
    bk_ops=get_bk_ops(dev_no);
    if(bk_ops==NULL){
        kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
    }
    bk_cache_ls = get_bk_dev_cache(dev_no,&cache_len);

    lock_bk_ls(dev_no);
    for(i=0;i<cache_len;i++){
        lock_bk(bk_cache_ls+i);
        if(
                bk_cache_ls[i].cache==addr
                && IS_MMAP(bk_cache_ls[i])
                ) {
            if (GET_BIT(bk_cache_ls[i].flag, 1)) {
                //�Ȳ���
                if(bk_ops->erase_bk(bk_cache_ls[i].bk_no)<0){
                    kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
                }
                if(bk_ops->write_bk(bk_cache_ls[i].bk_no,bk_cache_ls[i].cache)<0){
                    kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
                }
                CLR_BIT(bk_cache_ls[i].flag,0);
                CLR_BIT(bk_cache_ls[i].flag,1);
            }
            CLR_KEY(bk_cache_ls[i]);
            bk_cache_ls[i].cache = bk_cache_ls[i].oldcache;
            bk_cache_ls[i].oldcache = NULL;

            unlock_bk(bk_cache_ls + i);
            unlock_bk_ls(dev_no);
            return 0;
        }
    }
    unlock_bk_ls(dev_no);
    return 0;
}
int32_t rel_bk(dev_t dev_no,bk_no_t bk_no){
    struct bk_cache* bk_cache_ls;
    int cache_len;
    int i;

    bk_cache_ls = get_bk_dev_cache(dev_no,&cache_len);

    lock_bk_ls(dev_no);
    for(i=0;i<cache_len;i++) {
        lock_bk(bk_cache_ls + i);
        if (
                bk_cache_ls[i].bk_no == bk_no
                && IS_MMAP(bk_cache_ls[i])
                ) {
            CLR_KEY(bk_cache_ls[i]);
            bk_cache_ls[i].cache = bk_cache_ls[i].oldcache;
            bk_cache_ls[i].oldcache = NULL;
            unlock_bk(bk_cache_ls + i);
            unlock_bk_ls(dev_no);
            return 0;
        }
        unlock_bk(bk_cache_ls + i);
    }
    unlock_bk_ls(dev_no);
    return 0;
}
void sync_bk(dev_t dev_no,uint32_t bk_no){
    uint32_t cache_len;
    struct bk_cache* bk_cache_ls;
    struct bk_operations *bk_ops;
    struct bk_cache* sync_cache;
    bk_ops=get_bk_ops(dev_no);
    if(bk_ops==NULL){
        return ;
    }
    bk_cache_ls = get_bk_dev_cache(dev_no,&cache_len);

    lock_bk_ls(dev_no);

    sync_cache= search_bk(bk_cache_ls,cache_len,dev_no,bk_no);
    if(sync_cache==NULL){
        unlock_bk_ls(dev_no);
        return ;
    }
    lock_bk(sync_cache);
    if (GET_BIT(sync_cache->flag, 1)) {
        //�Ȳ���
        if(bk_ops->erase_bk(sync_cache->bk_no)<0){
            // sync_cache->flag=0;
            kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
        }
        if(bk_ops->write_bk(sync_cache->bk_no,sync_cache->cache)<0){
            //  sync_cache->flag=0;
            kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
        }
        CLR_BIT(sync_cache->flag,0);
        CLR_BIT(sync_cache->flag,1);
    }
    if(!IS_FREEZE(*sync_cache)) {
        CLR_KEY(*sync_cache);
        sync_cache->used_cn=0;
    }
    unlock_bk(sync_cache);
    unlock_bk_ls(dev_no);
}
/**
 * ���ͬ��һ����
 * @param dev_no
 * @param bk_ch
 * @return
 */
struct bk_cache* sync_rand_bk(dev_t dev_no) {
    struct bk_operations *bk_ops;
    struct bk_cache *sync_cache;
    uint32_t cache_len;
    struct bk_cache* bk_cache_ls;

    bk_ops=get_bk_ops(dev_no);
    if(bk_ops==NULL){
        kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
    }
    bk_cache_ls = get_bk_dev_cache(dev_no,&cache_len);

    lock_bk_ls(dev_no);
    again:

    //�������ͬ�������У������������Ҫ����ͬһ��bk��������������Ĳ�һ������ôҪ�ȴ���Ҳ��һ�����������ͷ��˲�ͬ��bk��
    sync_cache=&bk_cache_ls[get_sys_tasks_info()->sys_run_count % cache_len];
    if(atomic_read(&sync_cache->b_lock)){
        //�����Ѿ��������ģ����������һ��
        task_sche();
        goto again;
    }

    lock_bk(sync_cache);
    if (GET_BIT(sync_cache->flag, 1)) {
        //�Ȳ���
        if(bk_ops->erase_bk(sync_cache->bk_no)<0){
            kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
        }
        if(bk_ops->write_bk(sync_cache->bk_no,sync_cache->cache)<0){
            kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
        }
        CLR_BIT(sync_cache->flag,0);
        CLR_BIT(sync_cache->flag,1);
    }
    if(!IS_FREEZE(*sync_cache)) {
        CLR_KEY(*sync_cache);
        (*sync_cache).used_cn=0;
    }else{
        unlock_bk(sync_cache);
        goto again;
    }
    unlock_bk(sync_cache);
    unlock_bk_ls(dev_no);
    return sync_cache;
}
int32_t sync_all_bk(dev_t dev_no){
    struct bk_cache* bk_cache_ls;
    struct bk_operations *bk_ops;
    uint32_t cache_len;
    uint32_t i;

    bk_ops=get_bk_ops(dev_no);
    if(bk_ops==NULL){
        return -1;
    }
    bk_cache_ls= get_bk_dev_cache(dev_no,&cache_len);
    if(bk_cache_ls==NULL){
        return -1;
    }
    lock_bk_ls(dev_no);
    for(i=0;i<cache_len;i++){
        if(!IS_USEDKEY(bk_cache_ls[i])){
            continue;
        }
        lock_bk(bk_cache_ls+i);
        if (GET_BIT(bk_cache_ls[i].flag, 1)) {
            //�Ȳ���
            if(bk_ops->erase_bk(bk_cache_ls[i].bk_no)<0){
               // bk_cache_ls[i].flag=0;
                kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
            }
            if(bk_ops->write_bk(bk_cache_ls[i].bk_no,bk_cache_ls[i].cache)<0){
              //  bk_cache_ls[i].flag=0;
                kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
            }
            CLR_BIT(bk_cache_ls[i].flag,0);
            CLR_BIT(bk_cache_ls[i].flag,1);
        }
        if(!IS_FREEZE(bk_cache_ls[i])) {
            CLR_KEY(bk_cache_ls[i]);
            bk_cache_ls[i].used_cn=0;
        }
        unlock_bk(bk_cache_ls+i);
    }
    unlock_bk_ls(dev_no);
    return 0;
}

/**
 * �ڱ����ҵ�ָ����cache�����û�пյ��򷵻�NULL
 * @param bk_cache_ls
 * @param bk_no
 * @param cache_len
 * @return
 */
static struct bk_cache* find_bk_cache(dev_t dev_no,uint32_t bk_no){
    uint32_t i=0;
    uint32_t cache_len;
    int inx;

    struct bk_cache* bk_cache_ls;
    bk_cache_ls = get_bk_dev_cache(dev_no,&cache_len);

    lock_bk_ls(dev_no);
    //�Ȳ���
    inx = bk_hash(bk_no, cache_len);
    while(!IS_NULLKEY(bk_cache_ls[inx])) {
        //û�б�ɾ�����ұ�ʹ�ã���ֱ�ӷ���
        if (!IS_DELKEY(bk_cache_ls[inx])
        && bk_cache_ls[inx].bk_no == bk_no) {
            bk_cache_ls[inx].used_cn++;
            unlock_bk_ls(dev_no);
            return &bk_cache_ls[inx];
        }
        inx = bk_hash(inx + 1, cache_len);
        i++;
        if(i>=cache_len){
            break;
        }
    }
    i=0;
    inx = bk_hash(bk_no, cache_len);
    if(!IS_NULLKEY(bk_cache_ls[inx]) && !IS_DELKEY(bk_cache_ls[inx])){
        do{
            inx = bk_hash(inx + 1, cache_len);
            i++;
        }while(!IS_NULLKEY(bk_cache_ls[inx]) && !IS_DELKEY(bk_cache_ls[inx]) && i<cache_len);
        if(i==cache_len){
            //���û�յ���
            unlock_bk_ls(dev_no);
            return NULL;
        }
    }
    SET_USEDKEY(bk_cache_ls[inx]);
    bk_cache_ls[inx].bk_no=bk_no;
    atomic_set(&bk_cache_ls[inx].b_lock.counter,0);
    bk_cache_ls[inx].used_cn++;
    unlock_bk_ls(dev_no);
    return &(bk_cache_ls[inx]);
}

/**
 * д��
 * @param dev
 * @param data
 * @param bk_size
 * @return
 */
int32_t wbk(dev_t dev_no,uint32_t bk_no,uint8_t *data,uint32_t ofs,uint32_t size) {
    struct bk_cache* bk_cache_ls;
    struct bk_operations *bk_ops;
    uint32_t cache_len;
    struct bk_cache* bk_tmp;
    uint32_t bk_cn;
    if(get_bk_cn(dev_no,&bk_cn)<0){
        return NULL;
    }
    if(bk_no>=bk_cn){
        //�����˿ɶ�д�Ŀ鷶Χ
        return NULL;
    }
    bk_ops=get_bk_ops(dev_no);
    if(bk_ops==NULL){
        kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
    }
    bk_cache_ls = get_bk_dev_cache(dev_no,&cache_len);
    if(bk_cache_ls==NULL){
        kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
    }
    again:
    bk_tmp=find_bk_cache(dev_no,bk_no);
    if(bk_tmp==NULL){
        //û�����ͷ�һ��
        bk_tmp=sync_rand_bk(dev_no);
        goto again;
    }
    lock_bk(bk_tmp);
    if(bk_tmp->bk_no!=bk_no){
        unlock_bk(bk_tmp);
        //�����ͬ���˿飬��˵��
        goto again;
    }
    if(ofs==0 && size==bk_tmp->bk_size) {
        //����պ�дһ�飬��û��Ҫ��
        SET_BIT(bk_tmp->flag,2);
    }else {
        //�������һ��
        if (!GET_BIT(bk_tmp->flag, 2)) {
            if (bk_ops->read_bk(bk_no, bk_tmp->cache) < 0) {
//                kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
            }
            SET_BIT(bk_tmp->flag, 2);
        }
    }

    if(ofs+size>bk_tmp->bk_size){
        //д�����˱߽磬������
        kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
    }
    mkrtos_memcpy(bk_tmp->cache+ofs,data,size);
    SET_BIT(bk_tmp->flag,1);
    unlock_bk(bk_tmp);

    return 0;
}


/**
 * ����
 * @param dev
 * @param bk_inx
 * @param bk_size
 * @return
 */
int32_t rbk(dev_t dev_no,uint32_t bk_no,uint8_t *data,uint32_t ofs,uint32_t size) {
    struct bk_cache* bk_cache_ls;
    struct bk_operations *bk_ops;
    uint32_t cache_len;
    struct bk_cache* bk_tmp;
    uint32_t bk_cn;
    if(get_bk_cn(dev_no,&bk_cn)<0){
        return NULL;
    }
    if(bk_no>=bk_cn){
        //�����˿ɶ�д�Ŀ鷶Χ
        return NULL;
    }
    bk_ops=get_bk_ops(dev_no);
    if(bk_ops==NULL){
        kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
        return -1;
    }
    bk_cache_ls = get_bk_dev_cache(dev_no,&cache_len);
    if(bk_cache_ls==NULL){
        kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);

        return -1;
    }
    again:
    bk_tmp=find_bk_cache(dev_no,bk_no);
    if(bk_tmp==NULL){
        //û�����ͷ�һ��
        bk_tmp=sync_rand_bk(dev_no);
        goto again;

    }
    lock_bk(bk_tmp);
    if(bk_tmp->bk_no!=bk_no){
        //��ʱ�����Ǹ�bk cache�Ѿ����ı��ˣ����ʱ����Ҫ���»�ȡһ���µĻ���
        unlock_bk(bk_tmp);
        goto again;
    }
    if (!GET_BIT(bk_tmp->flag, 2)) {
        if (bk_ops->read_bk(bk_no, bk_tmp->cache) < 0) {
//            kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
        }
        SET_BIT(bk_tmp->flag, 2);
    }
    if(ofs+size>bk_tmp->bk_size){
        //д�����˱߽磬������
        kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
    }
    mkrtos_memcpy(data,bk_tmp->cache+ofs,size);
    unlock_bk(bk_tmp);

    return 0;
}

/**
 * ��ȡ����
 * @param dev_no
 * @param bk_no
 * @param not_r �������Ϊ1���������ж��������������ֱ��дһ�����ݣ�����Ҫ�������Ǻ�����Ȼ���bk_cache�ṹ�������Ѷ���־
 * @return
 */
struct bk_cache* bk_read(dev_t dev_no,uint32_t bk_no
		,uint32_t may_write,int32_t not_r){
    struct bk_cache* bk_cache_ls;
    struct bk_operations *bk_ops;
    struct bk_cache* bk_tmp;
    uint32_t cache_len=0;
    uint32_t bk_cn;
    if(get_bk_cn(dev_no,&bk_cn)<0){
        return NULL;
    }
    if(bk_no>=bk_cn){
        DEBUG("bk",ERR,"bk_no %d,bk_cn %d.",bk_no,bk_cn);
        //�����˿ɶ�д�Ŀ鷶Χ
        return NULL;
    }
    bk_ops=get_bk_ops(dev_no);
    if(bk_ops==NULL){
        kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
    }
    bk_cache_ls = get_bk_dev_cache(dev_no,&cache_len);
    if(bk_cache_ls==NULL){
        kfatal("%s %s ��������",__FUNCTION__ ,__LINE__);
    }

    again:
    bk_tmp=find_bk_cache(dev_no,bk_no);
    if(bk_tmp==NULL){
        //û�����ͷ�һ��
        bk_tmp=sync_rand_bk(dev_no);
        goto again;

    }
    lock_bk(bk_tmp);
    if(bk_tmp->bk_no!=bk_no){
        //��ʱ�����Ǹ�bk cache�Ѿ����ı��ˣ����ʱ����Ҫ���»�ȡһ���µĻ���
        unlock_bk(bk_tmp);
        goto again;
    }
    if (!not_r && !GET_BIT(bk_tmp->flag, 2)) {
        if (bk_ops->read_bk(bk_no, bk_tmp->cache) < 0) {
        }

    }
    SET_BIT(bk_tmp->flag, 2);
    if(may_write){
        SET_BIT(bk_tmp->flag,1);
    }
    return bk_tmp;
}
/**
 * �ͷŻ���
 * @param bk_tmp
 */
void bk_release(struct bk_cache* bk_tmp){
    if(bk_tmp==NULL){
        return ;
    }
    unlock_bk(bk_tmp);
}

static void __wait_on_bk(struct bk_cache* bk){
    struct wait_queue wait = {get_current_task() , NULL };

    add_wait_queue(&bk->b_wait, &wait);
    again:
    task_suspend();
//    CUR_TASK->status = TASK_SUSPEND;
    if (atomic_read(&( bk->b_lock))) {
        task_sche();
        goto again;
    }
    remove_wait_queue(&bk->b_wait, &wait);
    task_run();
//    CUR_TASK->status = TASK_RUNNING;
}

void wait_on_bk(struct bk_cache* bk){
    if(atomic_read(&bk->b_lock)){
        __wait_on_bk(bk);
    }
}
void lock_bk(struct bk_cache* bk){
    wait_on_bk(bk);
    atomic_set(&bk->b_lock,1);
}

void unlock_bk(struct bk_cache* bk){
    atomic_set(&bk->b_lock,0);
    wake_up(bk->b_wait);
}
