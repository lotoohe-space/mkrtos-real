/**
 * @file futex.c
 * @author ATShining (1358745329@qq.com)
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
#include "globals.h"
#include "string.h"
#include "ipc.h"
#include "err.h"
#include "slist.h"
#include "access.h"
#include "limits.h"
#include "futex.h"
#include "thread_arch.h"
#include <pre_cpu.h>
/**
 * @brief 以下是futex的操作码
 *
 */
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

/**
 * @brief futex对象的操作码
 *
 */
enum futex_op
{
    FUTEX_CTRL, //!< futex的控制
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
    uint32_t *uaddr;               //!< 锁的地址
    slist_head_t thread_list_head; //!< 存储线程的头节点
    size_t cnt;                    //!< 有多少个线程在等待锁
} futex_lock_t;

/**
 * @brief futex的对象定义
 *
 */
typedef struct futex
{
    kobject_t kobj;
    futex_lock_t fl_list[CONFIG_FT_ADDR_NR]; //!< 存储加锁的地址
} futex_t;

static futex_t futex_obj; //!< 全局的futex对象
static void futex_init(futex_t *ft);
/**
 * @brief futex的等待的item
 *
 */
typedef struct futex_wait_item
{
    slist_head_t node;
    thread_t *th;
    mword_t sleep_times;
} futex_wait_item_t;

static PER_CPU(slist_head_t, wait_list); //!< futex的等待队列

/**
 * @brief 初始化一个超时等待队列
 *
 */
static void futex_wait_list_init(void)
{
    for (int i = 0; i < CONFIG_CPU; i++)
    {
        slist_init(pre_cpu_get_var_cpu(i, &wait_list));
    }
}
INIT_KOBJ(futex_wait_list_init);
/**
 * @brief 检查超时队列
 *
 */
void futex_timeout_times_tick(void)
{
    futex_wait_item_t *item;

    slist_foreach_not_next(item,
                           (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_list),
                           node) //!< 第一次循环等待的ipc
    {
        futex_wait_item_t *next = slist_next_entry(item,
                                                   (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_list),
                                                   node);
        if (item->sleep_times > 0)
        {
            if ((--item->sleep_times) == 0)
            {
                slist_del(&item->node);
                //!< 超时时间满后直接唤醒等待者
                thread_ready_remote(item->th, TRUE);
            }
        }
        else
        {
            slist_del(&item->node);
            thread_ready_remote(item->th, TRUE);
        }
        item = next;
    }
}
/**
 * @brief 初始化全局的futex并注册
 *
 */
static void futex_reg(void)
{
    futex_init(&futex_obj);
    global_reg_kobj(&futex_obj.kobj, FUTEX_PROT);
}
INIT_KOBJ(futex_reg);
/**
 * @brief 查找futex中指定线程是否存在
 *
 * @param flt futex_lock_t对象
 * @param thread_hd 线程的指针
 * @return bool_t TRUE：存在 FALSE：不存在
 */
static bool_t futex_find_thread(futex_lock_t *flt, thread_t *thread_hd)
{
    thread_t *pos;

    slist_foreach(pos, &flt->thread_list_head, futex_node)
    {
        if (pos == thread_hd)
        {
            return TRUE;
        }
    }
    return FALSE;
}
/**
 * @brief futex进队
 *
 * @param flt
 * @param th
 */
static void futex_enqueue(futex_lock_t *flt, thread_t *th)
{
    slist_add_append(&flt->thread_list_head, &th->futex_node);
    flt->cnt++;
}
/**
 * @brief futex出队
 *
 * @param flt
 * @param th
 * @return int
 */
static int futex_dequeue(futex_lock_t *flt, thread_t **th)
{
    assert(th);
    if (slist_is_empty(&flt->thread_list_head))
    {
        return -1;
    }
    slist_head_t *first = slist_first(&flt->thread_list_head);
    slist_del(first);
    flt->cnt--;
    *th = container_of(first, thread_t, futex_node);

    return 0;
}
/**
 * @brief 获取futex的锁有多少个线程在等待
 *
 * @param flt
 * @return size_t
 */
static size_t futex_cnt(futex_lock_t *flt)
{
    return flt->cnt;
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

    for (int i = 0; i < CONFIG_FT_ADDR_NR; i++)
    {
        if (ft->fl_list[i].uaddr == uaddr)
        {
            if (!futex_find_thread(&ft->fl_list[i], hd))
            {
                futex_enqueue(&ft->fl_list[i], hd);
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
        futex_enqueue(&ft->fl_list[empty_inx], hd);
        return &ft->fl_list[empty_inx];
    }
    return NULL;
}
/**
 * @brief 找到指定地址的锁结构体
 *
 * @param fst
 * @param uaddr
 * @return futex_lock_t*
 */
static futex_lock_t *futex_find(futex_t *fst, void *uaddr)
{
    for (int i = 0; i < CONFIG_FT_ADDR_NR; i++)
    {
        if (fst->fl_list[i].uaddr == uaddr)
        {
            return &fst->fl_list[i];
        }
    }
    return NULL;
}
static int futex_wake(futex_t *fst, uint32_t *uaddr, int val)
{
    mword_t status;
    int rel_cnt = 0;
    uint32_t *paddr;

    status = spinlock_lock(&fst->kobj.lock);
    paddr = (uint32_t *)task_get_currnt_paddr((vaddr_t)uaddr);
    if (paddr == 0)
    {
        spinlock_set(&fst->kobj.lock, status);
        return -EACCES;
    }
    // printk("%s:%d uaddr: 0x%lx paddr: 0x%lx\n", __func__, __LINE__, uaddr, paddr);
    futex_lock_t *flt = futex_find(fst, paddr);

    if (flt)
    {
        rel_cnt = val = INT_MAX ? futex_cnt(flt) : val;

        for (int i = 0; i < rel_cnt; i++)
        {
            thread_t *th;
            int ret = futex_dequeue(flt, (&th));

            if (ret == 0)
            {
                if (futex_cnt(flt) == 0)
                {
                    flt->uaddr = 0;
                }
                thread_ready_remote(th, TRUE);
            }
        }
    }
    spinlock_set(&fst->kobj.lock, status);

    return rel_cnt;
}
static int futex_wait(thread_t *cur_th, futex_t *fst, uint32_t *uaddr, int val, umword_t timeout)
{
    umword_t status;
    uint32_t *p_addr;

    status = spinlock_lock(&fst->kobj.lock);
    if (!is_rw_access(thread_get_bind_task(cur_th), uaddr, sizeof(*uaddr), FALSE))
    {
        spinlock_set(&fst->kobj.lock, status);
        return -EACCES;
    }
    p_addr = (uint32_t *)task_get_currnt_paddr((vaddr_t)uaddr);
    if (p_addr == 0)
    {
        spinlock_set(&fst->kobj.lock, status);
        return -EACCES;
    }
    // printk("%s:%d uaddr: 0x%lx paddr: 0x%lx\n", __func__, __LINE__, uaddr, p_addr);
    if (val == *p_addr) //!< 检查值是否时期望值
    {
        ref_counter_inc(&cur_th->ref); //!< 指定线程引用计数+1
        if (futex_set_addr(fst, p_addr, cur_th) == NULL)
        {
            ref_counter_dec_and_release(&cur_th->ref, &cur_th->kobj);
            spinlock_set(&fst->kobj.lock, status);
            return -ENOMEM;
        }
        spinlock_set(&fst->kobj.lock, status);
        if (timeout != 0)
        {
            //! 加入到等待队列中
            futex_wait_item_t item = {
                .th = cur_th,
                .sleep_times = timeout,
            };

            slist_init(&item.node);
            slist_add_append((slist_head_t *)pre_cpu_get_current_cpu_var(&wait_list), &item.node);
            thread_suspend(cur_th);
            preemption();
            if (item.sleep_times <= 0)
            {
                ref_counter_dec_and_release(&cur_th->ref, &cur_th->kobj);
                // spinlock_set(&fst->kobj.lock, status);
                return -ETIMEDOUT;
            }
            if (cur_th->ipc_status == THREAD_IPC_ABORT)
            {
                cur_th->ipc_status = THREAD_NONE;
                // spinlock_set(&fst->kobj.lock, status);
                return 0;
            }
        }
        else
        {
            // 一直等待
            thread_suspend(cur_th);
            preemption();
            if (cur_th->ipc_status == THREAD_IPC_ABORT)
            {
                cur_th->ipc_status = THREAD_NONE;
                // spinlock_set(&fst->kobj.lock, status);
                return 0;
            }
        }

        ref_counter_dec_and_release(&cur_th->ref, &cur_th->kobj);
    }
    else
    {
        spinlock_set(&fst->kobj.lock, status);
        return -EAGAIN;
    }
    return 0;
}
static int futex_wake_clear(thread_t *cur_th, futex_t *fst, uint32_t *uaddr, int val)
{
    mword_t status;
    uint32_t *paddr;
    status = spinlock_lock(&fst->kobj.lock);

    paddr = (uint32_t *)task_get_currnt_paddr((vaddr_t)uaddr);
    if (paddr == 0)
    {
        spinlock_set(&fst->kobj.lock, status);
        return -EACCES;
    }
    if (!is_rw_access(thread_get_bind_task(cur_th), paddr, sizeof(*paddr), FALSE))
    {
        spinlock_set(&fst->kobj.lock, status);
        return -EACCES;
    }
    futex_lock_t *flt = futex_find(fst, paddr);
    int rel_cnt = 0;

    if (flt)
    {
        rel_cnt = val = INT_MAX ? futex_cnt(flt) : val;

        for (int i = 0; i < rel_cnt; i++)
        {
            thread_t *th;
            int ret = futex_dequeue(flt, (&th));

            if (ret == 0)
            {
                if (futex_cnt(flt) == 0)
                {
                    flt->uaddr = 0;
                }
                thread_ready_remote(th, TRUE);
            }
        }
    }
    *paddr = 0;
    spinlock_set(&fst->kobj.lock, status);
    return 0;
}
static int futex_requeue(thread_t *cur_th, futex_t *fst,
                         uint32_t *uaddr, uint32_t *uaddr2, int val, int val3)
{
    mword_t status;
    uint32_t *paddr;
    uint32_t *paddr2;

    status = spinlock_lock(&fst->kobj.lock);
    if (!is_rw_access(thread_get_bind_task(cur_th), uaddr, sizeof(*uaddr), FALSE))
    {
        spinlock_set(&fst->kobj.lock, status);
        return -EACCES;
    }
    paddr = (uint32_t *)task_get_currnt_paddr((vaddr_t)uaddr);
    if (paddr == 0)
    {
        spinlock_set(&fst->kobj.lock, status);
        return -EACCES;
    }
    paddr2 = (uint32_t *)task_get_currnt_paddr((vaddr_t)uaddr2);
    if (paddr2 == 0)
    {
        spinlock_set(&fst->kobj.lock, status);
        return -EACCES;
    }
    if (val3 == *paddr)
    {
        futex_lock_t *flt = futex_find(fst, paddr);
        int rel_cnt = 0;
        int wake_cnt = 0;

        if (flt)
        {
            wake_cnt = val = INT_MAX ? futex_cnt(flt) : val;

            for (int i = 0; i < wake_cnt; i++)
            {
                thread_t *th;
                int ret = futex_dequeue(flt, &th);

                if (ret != 0)
                {
                    continue;
                }
                if (futex_cnt(flt) == 0)
                {
                    flt->uaddr = 0;
                }
                thread_ready_remote(th, TRUE);
            }
            if (futex_cnt(flt) <= 0)
            {
                spinlock_set(&fst->kobj.lock, status);
                return wake_cnt;
            }
            int requeue_cn = futex_cnt(flt);
            rel_cnt = requeue_cn;
            for (int i = 0; i < rel_cnt; i++)
            {
                thread_t *th;
                int ret = futex_dequeue(flt, &th);

                if (ret != 0)
                {
                    continue;
                }
                if (futex_cnt(flt) == 0)
                {
                    flt->uaddr = 0;
                }
                futex_set_addr(fst, (void *)paddr2, th);
            }
        }
        spinlock_set(&fst->kobj.lock, status);
        return wake_cnt;
    }
    spinlock_set(&fst->kobj.lock, status);
    return 0;
}
static int futex_lock_pi(thread_t *cur_th, futex_t *fst, uint32_t *uaddr, uint32_t tid)
{
    mword_t status;
    uint32_t *paddr;

    status = spinlock_lock(&fst->kobj.lock);
    if (!is_rw_access(thread_get_bind_task(cur_th), uaddr, sizeof(*uaddr), FALSE))
    {
        spinlock_set(&fst->kobj.lock, status);
        return -EACCES;
    }
    paddr = (uint32_t *)task_get_currnt_paddr((vaddr_t)uaddr);
    if (paddr == 0)
    {
        spinlock_set(&fst->kobj.lock, status);
        return -EACCES;
    }
    if (*paddr = 0)
    {
        *paddr = tid;
    }
    else
    {
        *paddr |= FUTEX_WAITERS;
    }
    spinlock_set(&fst->kobj.lock, status);
    return 0;
}
/**
 * @brief futex的处理函数
 *
 * @param fst
 * @param uaddr
 * @param futex_op
 * @param val
 * @param timeout
 * @param uaddr2
 * @param val3
 * @param tid
 * @return int
 */
static int futex_dispose(futex_t *fst, uint32_t *uaddr, int futex_op, uint32_t val,
                         umword_t timeout /*val2*/, umword_t uaddr2, uint32_t val3, int tid)

{
    thread_t *cur_th = thread_get_current();
    umword_t status;
    int ret = 0;

    futex_op = futex_op & 0x7f;

    switch (futex_op)
    {
    case FUTEX_REQUEUE:
    {
        ret = futex_requeue(cur_th, fst, uaddr, (void *)uaddr2, val, val3);
        if (ret < 0)
        {
            return ret;
        }
    }
    break;
    case FUTEX_WAIT:
    {
        // printk("futex wait:0x%lx\n", uaddr);
        ret = futex_wait(cur_th, fst, uaddr, val, timeout);
        if (ret < 0)
        {
            return ret;
        }
        break;
    }
    break;
    case FUTEX_WAKE:
    {
        int rel_cnt;
        // printk("futex wake:0x%lx\n", uaddr);
        rel_cnt = futex_wake(fst, uaddr, val);
        return rel_cnt;
    }
    case FUTEX_UNLOCK_PI:
    case FUTEX_WAKE_CLEAR:
    {
        int rel_cnt;

        rel_cnt = futex_wake_clear(cur_th, fst, uaddr, val);
        return rel_cnt;
    }
    case FUTEX_LOCK_PI:
    {
        ret = futex_lock_pi(cur_th, fst, uaddr, tid);
        if (ret < 0)
        {
            return ret;
        }
    }
    default:
        return -ENOSYS;
    }
    return ret;
}
/**
 * @brief futex的系统调用
 *
 * @param kobj
 * @param sys_p
 * @param in_tag
 * @param f
 */
static void futex_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    thread_t *th = thread_get_current();
    futex_t *futex = container_of(kobj, futex_t, kobj);
    ipc_msg_t *msg;

    if (sys_p.prot != FUTEX_PROT)
    {
        f->regs[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    msg = thread_get_msg_buf(th);
    if (msg == NULL)
    {
        f->regs[0] = msg_tag_init4(0, 0, 0, -ENOBUFS).raw;
        return;
    }
    switch (sys_p.op)
    {
    case FUTEX_CTRL:
    {
        int ret;
        umword_t *msg_buf = thread_get_kmsg_buf(th);
        uint32_t *uaddr = (uint32_t *)(msg_buf[0]);
        int futex_op = msg_buf[1];
        uint32_t val = msg_buf[2];
        umword_t timeout = msg_buf[3];
        uint32_t uaddr2 = msg_buf[4];
        uint32_t val3 = msg_buf[5];
        int tid = msg_buf[6];
        mword_t lock_status = cpulock_lock();

        ret = futex_dispose(futex, uaddr, futex_op, val, timeout, uaddr2, val3, tid);
        cpulock_set(lock_status);
        tag = msg_tag_init4(0, 0, 0, ret);
    }
    break;
    }
    f->regs[0] = tag.raw;
}
/**
 * @brief 在task结束时，需要删除某个task所关联的futex
 *
 * @param obj_space 需要操作的task的obj_space
 * @param kobj 哪一个futex对象
 */
static void futex_unmap(obj_space_t *obj_space, kobject_t *kobj)
{
    task_t *task = container_of(obj_space, task_t, obj_space);
    futex_t *futex = container_of(kobj, futex_t, kobj);
    printk("%s:%d\n", __func__, __LINE__);

    umword_t status = spinlock_lock(&futex->kobj.lock);
    for (int i = 0; i < CONFIG_FT_ADDR_NR; i++)
    {
        thread_t *pos;

        if (futex->fl_list[i].uaddr)
        {
            slist_foreach_not_next(pos, &futex->fl_list[i].thread_list_head, futex_node)
            {
                thread_t *next = slist_next_entry(pos, &futex->fl_list[i].thread_list_head, futex_node);
                task_t *bind_task = thread_get_bind_task(pos);

                if (bind_task == task)
                {
                    assert(pos->status == THREAD_SUSPEND);
                    slist_del(&pos->futex_node);
                    pos->status == THREAD_IPC_ABORT;
                    thread_ready_remote(pos, TRUE);
                }
                pos = next;
            }
        }
    }
    spinlock_set(&futex->kobj.lock, status);
}
static void futex_release_stage1(kobject_t *kobj)
{
}
static bool_t futex_release_put(kobject_t *kobj)
{
    return FALSE;
}
static void futex_release_stage2(kobject_t *kobj)
{
    printk("futex don't release.\n");
}
/**
 * @brief 初始化futex对象
 *
 * @param ft
 */
static void futex_init(futex_t *ft)
{
    kobject_init(&ft->kobj, FUTEX_TYPE);
    for (int i = 0; i < CONFIG_FT_ADDR_NR; i++)
    {
        slist_init(&ft->fl_list[i].thread_list_head);
    }
    ft->kobj.invoke_func = futex_syscall;
    ft->kobj.unmap_func = futex_unmap;
    ft->kobj.stage_1_func = futex_release_stage1;
    ft->kobj.stage_2_func = futex_release_stage2;
    ft->kobj.put_func = futex_release_put;
}
