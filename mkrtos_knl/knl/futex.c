/**
 * @file futex.c
 * @author zhangzheng (1358745329@qq.com)
 * @brief futex在内核中实现
 * @note TODO: 可以使用hashmap优化
 * @version 0.1
 * @date 2023-11-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "types.h"
#include "init.h"
#include "prot.h"
#include "kobject.h"
#include "factory.h"
#include "task.h"
#include "thread.h"
#include "assert.h"
#include "slist.h"
#include "spinlock.h"
#include "futex_queue.h"
#include "globals.h"
#include "string.h"
#include "ipc.h"
#include "err.h"
#include <access.h>

#define INT_MAX 0x7fffffff

#define FT_ADDR_NR 16 //!< 最多加锁的对象

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1
#define FUTEX_FD 2
#define FUTEX_REQUEUE 3
#define FUTEX_CMP_REQUEUE 4
#define FUTEX_WAKE_OP 5
#define FUTEX_LOCK_PI 6
#define FUTEX_UNLOCK_PI 7
#define FUTEX_TRYLOCK_PI 8
#define FUTEX_WAIT_BITSET 9
#define FUTEX_WAKE_CLEAR 10
#define FUTEX_WAITERS 0x80000000

enum futex_op
{
    FUTEX_CTRL,
};

struct timespec
{
    time_t tv_sec; /* seconds */
    long tv_nsec;  /* nanoseconds */
};
/**
 * @brief 存储锁信息
 *
 */
typedef struct futex_lock
{
    uint32_t *uaddr;   //!< 锁的地址
    futex_queue_t fqt; //!< 锁的等待队列
} futex_lock_t;

typedef struct futex
{
    kobject_t kobj;
    futex_lock_t fl_list[FT_ADDR_NR]; //!< 存储加锁的地址
    spinlock_t lock;
} futex_t;
static futex_t futex_obj;
static void futex_init(futex_t *ft);
typedef struct futex_wait_item
{
    slist_head_t node;
    thread_t *th;
    umword_t sleep_times;
} futex_wait_item_t;
static slist_head_t wait_list;

/**
 * @brief 初始化一个超时等待队列
 *
 */
static void futex_wait_list_init(void)
{
    slist_init(&wait_list);
}
INIT_KOBJ(futex_wait_list_init);
/**
 * @brief 检查超时队列
 *
 */
void futex_timeout_times_tick(void)
{
    futex_wait_item_t *item;

    slist_foreach(item, &wait_list, node) //!< 第一次循环等待的ipc
    {
        if (item->sleep_times > 0)
        {
            if ((--item->sleep_times) == 0)
            {
                //!< 超时时间满后直接唤醒等待者
                thread_ready(item->th, TRUE);
            }
        }
        else
        {
            if (item->th->status == THREAD_SUSPEND)
            {
                thread_ready(item->th, TRUE);
            }
        }
    }
}
static void futex_reg(void)
{
    futex_init(&futex_obj);
    global_reg_kobj(&futex_obj.kobj, FUTEX_PROT);
}
INIT_KOBJ(futex_reg);

static bool_t futex_find_thread(futex_queue_t *fq, thread_t *thread_hd)
{
    int find = 0;
    int len;

    len = fq_queue_len(fq);
    for (int i = 0; i < len; i++)
    {
        umword_t out;

        fq_dequeue(fq, &out);
        if (out == (umword_t)thread_hd)
        {
            find = 1;
        }
        fq_enqueue(fq, out);
    }
    return find;
}
/**
 * @brief 查找一个地址，如果已经有了，则加入等待，否则增加一个信息，并加入等待
 *
 * @param ft
 * @param uaddr
 * @param hd
 * @return futex_lock_t*
 */
static futex_lock_t *futex_set_addr(futex_t *ft, void *uaddr, thread_t *hd)
{
    int empty_inx = -1;

    for (int i = 0; i < FT_ADDR_NR; i++)
    {
        if (ft->fl_list[i].uaddr == uaddr)
        {
            if (!futex_find_thread(&ft->fl_list[i].fqt, hd))
            {
                fq_enqueue(&ft->fl_list[i].fqt, (umword_t)hd);
            }
            return &ft->fl_list[i];
        }
        else if (ft->fl_list[i].uaddr == NULL)
        {
            empty_inx = i;
        }
    }
    if (empty_inx != -1)
    {
        ft->fl_list[empty_inx].uaddr = uaddr;
        fq_enqueue(&ft->fl_list[empty_inx].fqt, (umword_t)hd);
        return &ft->fl_list[empty_inx];
    }
    return NULL;
}
static futex_lock_t *futex_find(futex_t *fst, void *uaddr)
{
    for (int i = 0; i < FT_ADDR_NR; i++)
    {
        if (fst->fl_list[i].uaddr == uaddr)
        {
            return &fst->fl_list[i];
        }
    }
    return NULL;
}

static int futex_dispose(futex_t *fst, uint32_t *uaddr, int futex_op, uint32_t val,
                         umword_t timeout /*val2*/, uint32_t uaddr2, uint32_t val3, int tid)

{
    thread_t *cur_th = thread_get_current();
    futex_op = futex_op & 0x7f;
    umword_t status;

    status = spinlock_lock(&fst->lock);
    switch (futex_op)
    {
    case FUTEX_REQUEUE:
    {
        if (!is_rw_access(uaddr, sizeof(*uaddr), FALSE))
        {
            spinlock_set(&fst->lock, status);
            return -EACCES;
        }
        if (val3 == *uaddr)
        {
            futex_lock_t *flt = futex_find(fst, uaddr);
            int rel_cnt = 0;
            int wake_cnt = 0;

            if (flt)
            {
                wake_cnt = val = INT_MAX ? fq_queue_len(&flt->fqt) : val;

                for (int i = 0; i < wake_cnt; i++)
                {
                    thread_t *th;
                    int ret = fq_dequeue(&flt->fqt, (umword_t *)&th);

                    if (ret != 0)
                    {
                        continue;
                    }
                    if (fq_queue_len(&flt->fqt) == 0)
                    {
                        flt->uaddr = 0;
                    }
                    thread_ready(th, TRUE);
                }
                if (fq_queue_len(&flt->fqt) <= 0)
                {
                    spinlock_set(&fst->lock, status);
                    return wake_cnt;
                }
                int requeue_cn = fq_queue_len(&flt->fqt);
                rel_cnt = requeue_cn;
                for (int i = 0; i < rel_cnt; i++)
                {
                    thread_t *th;
                    int ret = fq_dequeue(&flt->fqt, (umword_t *)&th);

                    if (ret != 0)
                    {
                        continue;
                    }
                    if (fq_queue_len(&flt->fqt) == 0)
                    {
                        flt->uaddr = 0;
                    }
                    futex_set_addr(fst, (void *)uaddr2, th);
                }
            }
            spinlock_set(&fst->lock, status);
            return wake_cnt;
        }
    }
    break;
    case FUTEX_WAIT:
    {
        if (!is_rw_access(uaddr, sizeof(*uaddr), FALSE))
        {
            spinlock_set(&fst->lock, status);
            return -EACCES;
        }
        if (val == *uaddr)
        {
            ref_counter_inc(&cur_th->ref);
            if (futex_set_addr(fst, uaddr, cur_th) == NULL)
            {
                ref_counter_dec_and_release(&cur_th->ref, &cur_th->kobj);
                spinlock_set(&fst->lock, status);
                return -ENOMEM;
            }
            if (timeout != 0)
            {
                //! 加入到等待队列中
                futex_wait_item_t item = {
                    .th = cur_th,
                    .sleep_times = timeout,
                };

                slist_init(&item.node);
                slist_add_append(&wait_list, &item.node);
                thread_suspend(cur_th);
                preemption();
                slist_del(&item.node);
                if (timeout <= 0)
                {
                    ref_counter_dec_and_release(&cur_th->ref, &cur_th->kobj);
                    spinlock_set(&fst->lock, status);
                    return -ETIMEDOUT;
                }
            }
            else
            {
                // 一直等待
                thread_suspend(cur_th);
                preemption();
            }

            ref_counter_dec_and_release(&cur_th->ref, &cur_th->kobj);
        }
        else
        {
            spinlock_set(&fst->lock, status);
            return -EAGAIN;
        }
        break;
    }
    break;
    case FUTEX_WAKE:
    {
        futex_lock_t *flt = futex_find(fst, uaddr);
        int rel_cnt = 0;

        if (flt)
        {
            rel_cnt = val = INT_MAX ? fq_queue_len(&flt->fqt) : val;

            for (int i = 0; i < rel_cnt; i++)
            {
                thread_t *th;
                int ret = fq_dequeue(&flt->fqt, (umword_t *)(&th));

                if (ret == 0)
                {
                    if (fq_queue_len(&flt->fqt) == 0)
                    {
                        flt->uaddr = 0;
                    }
                    thread_ready(th, TRUE);
                }
            }
        }
        spinlock_set(&fst->lock, status);
        return rel_cnt;
    }
    case FUTEX_UNLOCK_PI:
    case FUTEX_WAKE_CLEAR:
    {
        if (!is_rw_access(uaddr, sizeof(*uaddr), FALSE))
        {
            spinlock_set(&fst->lock, status);
            return -EACCES;
        }
        futex_lock_t *flt = futex_find(fst, uaddr);
        int rel_cnt = 0;

        if (flt)
        {
            rel_cnt = val = INT_MAX ? fq_queue_len(&flt->fqt) : val;

            for (int i = 0; i < rel_cnt; i++)
            {
                thread_t *th;
                int ret = fq_dequeue(&flt->fqt, (umword_t *)(&th));

                if (ret == 0)
                {
                    if (fq_queue_len(&flt->fqt) == 0)
                    {
                        flt->uaddr = 0;
                    }
                    thread_ready(th, TRUE);
                }
            }
        }
        *uaddr = 0;
        spinlock_set(&fst->lock, status);
        return rel_cnt;
    }
    case FUTEX_LOCK_PI:
    {
        if (!is_rw_access(uaddr, sizeof(*uaddr), FALSE))
        {
            spinlock_set(&fst->lock, status);
            return -EACCES;
        }
        if (*uaddr = 0)
        {
            *uaddr = tid;
        }
        else
        {
            *uaddr |= FUTEX_WAITERS;
        }
        break;
    }
    default:
        spinlock_set(&fst->lock, status);
        return -ENOSYS;
    }
    spinlock_set(&fst->lock, status);
    return 0;
}
static void futex_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    thread_t *th = thread_get_current();
    futex_t *futex = container_of(kobj, futex_t, kobj);
    ipc_msg_t *msg;

    if (sys_p.prot != FUTEX_PROT)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    msg = thread_get_msg_buf(th);
    if (msg == NULL)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -ENOBUFS).raw;
        return;
    }
    switch (sys_p.op)
    {
    case FUTEX_CTRL:
    {
        int ret;
        uint32_t *uaddr = (uint32_t *)(msg->msg_buf[0]);
        int futex_op = msg->msg_buf[1];
        uint32_t val = msg->msg_buf[2];
        umword_t timeout = msg->msg_buf[3];
        uint32_t uaddr2 = msg->msg_buf[4];
        uint32_t val3 = msg->msg_buf[5];
        int tid = msg->msg_buf[6];

        ret = futex_dispose(futex, uaddr, futex_op, val, timeout, uaddr2, val3, tid);
        msg_tag_init4(0, 0, 0, ret);
    }
    break;
    }
    f->r[0] = tag.raw;
}
static void futex_release_stage1(kobject_t *kobj)
{
    /*TODO:删除task所占用的futex资源*/
}
static void futex_release_stage2(kobject_t *kobj)
{
    printk("futex don't release.\n");
}
static void futex_init(futex_t *ft)
{
    kobject_init(&ft->kobj, IPC_TYPE);
    spinlock_init(&ft->lock);
    ft->kobj.invoke_func = futex_syscall;
    ft->kobj.stage_1_func = futex_release_stage1;
    ft->kobj.stage_2_func = futex_release_stage2;
}
