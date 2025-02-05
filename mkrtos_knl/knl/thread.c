/**
 * @file thread.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "thread.h"

#include <arch.h>
#include <pre_cpu.h>
#include <thread_arch.h>

#include "access.h"
#include "arch.h"
#include "assert.h"
#include "err.h"
#include "factory.h"
#include "init.h"
#include "ipc.h"
#include "kobject.h"
#include "map.h"
#include "mm_wrap.h"
#include "ram_limit.h"
#include "slist.h"
#include "string.h"
#include "task.h"
#include "thread.h"
#include "thread_task_arch.h"
#include "types.h"
#include "sema.h"
#if IS_ENABLED(CONFIG_SMP)
#include <ipi.h>
#endif
#if IS_ENABLED(CONFIG_MPU)
#include <mpu.h>
#endif

#define TAG "[thread]"
enum thread_op
{
    SET_EXEC_REGS,
    RUN_THREAD,
    BIND_TASK,
    MSG_BUG_GET,
    MSG_BUG_SET,
    YIELD,
    DO_IPC = 6, //!< 与ipc对象中的额IPC_DO一致
    SET_EXEC,   //!< 设置异常处理
};
enum IPC_TYPE
{
    IPC_CALL,
    IPC_REPLY,
    IPC_WAIT,
    IPC_RECV,
    IPC_SEND,
    IPC_FAST_CALL,   //!< 快速CALL通信，不切换上下文
    IPC_FAST_REPLAY, //!<
};
static void thread_syscall(kobject_t *kobj, syscall_prot_t sys_p,
                           msg_tag_t in_tag, entry_frame_t *f);
static bool_t thread_put(kobject_t *kobj);
static void thread_release_stage1(kobject_t *kobj);
static void thread_release_stage2(kobject_t *kobj);

typedef struct thread_wait_entry
{
    slist_head_t node;
    slist_head_t node_timeout;
    thread_t *th;
    mword_t times;
} thread_wait_entry_t;

static inline void thread_wait_entry_init(thread_wait_entry_t *entry,
                                          thread_t *th, mword_t times)
{
    slist_init(&entry->node);
    slist_init(&entry->node_timeout);
    entry->th = th;
    entry->times = times;
}

static PER_CPU(slist_head_t, wait_send_queue);
static PER_CPU(slist_head_t, wait_recv_queue);

static void thread_timeout_init(void)
{
    for (int i = 0; i < CONFIG_CPU; i++)
    {
        slist_init(pre_cpu_get_var_cpu(i, (&wait_send_queue)));
        slist_init(pre_cpu_get_var_cpu(i, (&wait_recv_queue)));
    }
}

INIT_KOBJ(thread_timeout_init);

#if IS_ENABLED(CONFIG_BUDDY_SLAB)
#include <buddy.h>
#endif
/**
 * @brief 在系统初始化时调用，初始化thread的内存
 *
 */
static void thread_mem_init(void)
{
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
#endif
}
INIT_KOBJ_MEM(thread_mem_init);

/**
 * @brief 线程的初始化函数
 *
 * @param th
 */
void thread_init(thread_t *th, ram_limit_t *lim, umword_t flags)
{
    kobject_init(&th->kobj, THREAD_TYPE);
    sched_init(&th->sche);
    slist_init(&th->futex_node);
    slist_init(&th->wait_send_head);
    slist_init(&th->fast_ipc_node);
    spinlock_init(&th->recv_lock);
    spinlock_init(&th->send_lock);
    ref_counter_init(&th->ref);
    ref_counter_inc(&th->ref);
    thread_arch_init(th, flags);
    stack_init(&th->fast_ipc_stack, &th->fast_ipc_stack_data,
               ARRARY_LEN(th->fast_ipc_stack_data),
               sizeof(th->fast_ipc_stack_data[0]));
    th->cpu = arch_get_current_cpu_id();
    th->lim = lim;
    th->kobj.invoke_func = thread_syscall;
    th->kobj.put_func = thread_put;
    th->kobj.stage_1_func = thread_release_stage1;
    th->kobj.stage_2_func = thread_release_stage2;
    th->magic = THREAD_MAGIC;
}
static bool_t thread_put(kobject_t *kobj)
{
    thread_t *th = container_of(kobj, thread_t, kobj);

    return ref_counter_dec(&th->ref) == 1;
}
static void thread_release_stage1_impl(thread_t *th)
{
    if (th->status == THREAD_READY)
    {
        thread_suspend(th);
    }
    th->ipc_status = THREAD_IPC_ABORT;
    thread_wait_entry_t *pos;

    slist_foreach_not_next(
        pos, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_send_queue),
        node_timeout)
    {
        assert(pos->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(
            pos, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_send_queue),
            node_timeout);

        if (pos->th != th)
        {
            pos->th->ipc_status = THREAD_IPC_ABORT;
            thread_ready(pos->th, FALSE);
        }

        slist_del(&pos->node_timeout);
        if (slist_in_list(&pos->node))
        {
            slist_del(&pos->node);
        }
        pos = next;
    }
    thread_wait_entry_t *pos2;

    slist_foreach_not_next(
        pos2, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_recv_queue),
        node)
    {
        assert(pos2->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(
            pos2, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_recv_queue),
            node);

        slist_del(&pos2->node);
        if (pos2->th != th)
        {
            pos2->th->ipc_status = THREAD_IPC_ABORT;
            thread_ready(pos2->th, FALSE);
        }
        pos2 = next;
    }
    thread_unbind(th);
}
#if IS_ENABLED(CONFIG_SMP)
static int thread_remote_release_stage1_handler(ipi_msg_t *msg, bool_t *is_sched)
{
    thread_t *th = (thread_t *)msg->msg;
    assert(th);
    thread_release_stage1_impl(th);
    return 0;
}
#endif
int thread_release_stage1_remote(thread_t *th)
{
#if IS_ENABLED(CONFIG_SMP)
    if (th->cpu != arch_get_current_cpu_id())
    {
        th->ipi_msg_node.msg = (umword_t)th;
        th->ipi_msg_node.cb = thread_remote_release_stage1_handler;
        cpu_ipi_to_msg(1 << th->cpu, &th->ipi_msg_node, IPI_CALL);
    }
    else
    {
        thread_release_stage1_impl(th);
    }
#else
    thread_release_stage1_impl(th);
#endif
    return 0;
}
static void thread_release_stage1(kobject_t *kobj)
{
    thread_t *th = container_of(kobj, thread_t, kobj);
    thread_t *cur = thread_get_current();
    kobject_invalidate(kobj);

    if (cur != th)
    {
        thread_release_stage1_remote(th);
    }
    else
    {
        thread_release_stage1_remote(cur);
    }
}
static void thread_release_stage2(kobject_t *kobj)
{
    thread_t *th = container_of(kobj, thread_t, kobj);
    thread_t *cur_th = thread_get_current();

    if (cur_th == th)
    {
        scheduler_reset();
        thread_sched(FALSE);
        arch_to_sche();
    }
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    mm_limit_free_buddy(th->lim, kobj, CONFIG_THREAD_BLOCK_SIZE);
#else
    mm_limit_free_align(th->lim, kobj, CONFIG_THREAD_BLOCK_SIZE);
    // mm_trace();
    printk("release thread 0x%x, name:%s\n", kobj, kobject_get_name(kobj));
#endif
}

/**
 * @brief 设置运行寄存器
 *
 * @param th
 * @param pc
 * @param ip
 */
void thread_set_exc_regs(thread_t *th, umword_t pc, umword_t user_sp,
                         umword_t ram)
{
    thread_user_pf_set(th, (void *)pc, (void *)user_sp, (void *)ram);
}
/**
 * @brief 线程绑定到task
 *
 * @param th
 * @param tk
 */
void thread_bind(thread_t *th, kobject_t *tk)
{
    th->task = tk;
    task_t *tsk = container_of(tk, task_t, kobj);

    ref_counter_inc(&tsk->ref_cn);
}
/**
 * @brief 解除task绑定
 *
 * @param th
 */
void thread_unbind(thread_t *th)
{
    if (th->task)
    {
        task_t *tsk = container_of(th->task, task_t, kobj);

        ref_counter_dec_and_release(&tsk->ref_cn, &th->kobj);
        th->task = NULL;
    }
}
void thread_suspend_sw(thread_t *th, bool_t is_sche)
{
    assert(cpulock_get_status());
    assert(slist_in_list(&th->sche.node));
    assert(th->cpu == arch_get_current_cpu_id());
    umword_t status = cpulock_lock();

    th->status = THREAD_SUSPEND;
    scheduler_del(&th->sche);

    /* 当前线程才能够切出，不是当前线程执行这个也没有效果，而且还会导致当前线程切出*/
    if (th == thread_get_current() && is_sche)
    {
        thread_sched(TRUE);
    }
    else
    {
        thread_sched(FALSE);
        arch_to_sche(); //!< 触发调度中断
    }
    // printk("suspend: th:0x%lx\n", th);
    cpulock_set(status);
}
/**
 * @brief 挂起一个线程
 *
 * @param th
 */
void thread_suspend(thread_t *th)
{
    thread_suspend_sw(th, TRUE);
}

/**
 * @brief 线程死亡
 *
 * @param th
 */
void thread_dead(thread_t *th)
{
    if (!slist_in_list(&th->sche.node))
    {
        assert(slist_in_list(&th->sche.node));
    }
    scheduler_del(&th->sche);
    th->status = THREAD_DEAD;
    thread_sched(TRUE);
}

/**
 * @brief 进行一次调度
 *
 * @param th
 */
bool_t thread_sched(bool_t is_sche)
{
    umword_t status = cpulock_lock();
    sched_t *next_sche = scheduler_next();
    thread_t *th = thread_get_current();

    assert(next_sche);
    assert(th->magic == THREAD_MAGIC);
    if (next_sche == &th->sche)
    {
        atomic_inc(&th->time_count);
        //!< 线程没有发生变化，则不用切换
        cpulock_set(status);
        return FALSE;
    }
    if (is_sche)
    {
        // 立刻进行切换
#if IS_ENABLED(CONFIG_MMU)
        sche_arch_sw_context();
#else
        arch_to_sche();
#endif
    }
    // printk("sched: cpu:%d sp:0x%lx\n", arch_get_current_cpu_id(), th->sp.sp);
    cpulock_set(status);
    return TRUE;
}

#if IS_ENABLED(CONFIG_SMP)
static int thread_ready_remote_handler(ipi_msg_t *msg, bool_t *is_sched)
{
    thread_t *to_ready_th = (thread_t *)(msg->msg);
    thread_ready(to_ready_th, msg->msg2);

    thread_t *src_th = container_of(msg, thread_t, ipi_msg_node);

    *is_sched = src_th != to_ready_th;
    return 0;
}
#endif
void thread_ready_remote(thread_t *th, bool_t is_sche)
{
#if IS_ENABLED(CONFIG_SMP)
    thread_t *cur_th = thread_get_current();
    if (th->cpu == arch_get_current_cpu_id())
    {
        thread_ready(th, is_sche);
    }
    else
    {
        cur_th->ipi_msg_node.msg = (umword_t)th;
        cur_th->ipi_msg_node.msg2 = is_sche;
        cur_th->ipi_msg_node.cb = thread_ready_remote_handler;
        cpu_ipi_to_msg(1 << th->cpu, &cur_th->ipi_msg_node, IPI_CALL);
    }
#else
    thread_ready(th, is_sche);
#endif
}
/**
 * @brief 线程进入就绪态
 *
 * @param th
 */
void thread_ready(thread_t *th, bool_t is_sche)
{
    bool_t ret;
    umword_t status = cpulock_lock();

    if (slist_in_list(&th->sche.node))
    {
#if THREAD_IS_DEBUG
        dbg_printk(TAG "%s:%d panic : %s[0x%x] status:%d ipc_status:%d \n", __func__, __LINE__,
                   kobject_get_name(&th->kobj), th, th->status, th->ipc_status);
#endif
        assert(!slist_in_list(&th->sche.node));
    }
    assert(th->cpu == arch_get_current_cpu_id());
    th->status = THREAD_READY;
    ret = scheduler_add(&th->sche);
    if (is_sche && ret && th == thread_get_current())
    {
        // ready线程的优先级大于最大优先级
        thread_sched(TRUE);
    }
    else
    {
        thread_sched(FALSE);
        arch_to_sche();
    }
    // printk("ready: th:0x%lx\n", th);
    cpulock_set(status);
}
void thread_todead(thread_t *th, bool_t is_sche)
{
    // if (!!slist_in_list(&th->sche.node))
    // {
    assert(!slist_in_list(&th->sche.node));
    // }
    scheduler_add(&th->sche);
    th->status = THREAD_TODEAD;
    if (is_sche)
    {
        thread_sched(TRUE);
    }
}
/**
 * @brief 创建线程
 *
 * @param ram
 * @return thread_t*
 */
thread_t *thread_create(ram_limit_t *ram, umword_t flags)
{
    thread_t *th = NULL;

#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    th = mm_limit_alloc_buddy(ram, CONFIG_THREAD_BLOCK_SIZE);
#else
    th = mm_limit_alloc_align(ram, CONFIG_THREAD_BLOCK_SIZE,
                              CONFIG_THREAD_BLOCK_SIZE);
#endif
    if (!th)
    {
        return NULL;
    }
    // assert(((mword_t)th & (~(CONFIG_THREAD_BLOCK_SIZE - 1))) == 0);
    memset(th, 0, CONFIG_THREAD_BLOCK_SIZE);
    thread_init(th, ram, flags);
    printk("create thread 0x%x\n", th);
    return th;
}

/**
 * @brief 线程超时检查
 *
 * @param tick
 */
void thread_timeout_check(ssize_t tick)
{
    thread_wait_entry_t *pos;
    slist_head_t *head =
        (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_send_queue);

    slist_foreach_not_next(pos, head, node_timeout)
    {
        if (pos->th->status != THREAD_SUSPEND)
        {
            printk("%s:%d th_name:%s.\n", __func__, __LINE__, pos->th->kobj.dbg.name);
        }
        assert(pos->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(
            pos, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_send_queue),
            node_timeout);
        if (pos->times > 0)
        {
            pos->times -= tick;
            if (pos->times <= 0)
            {
                pos->th->ipc_status = THREAD_TIMEOUT;

                slist_del(&pos->node_timeout);
                if (slist_in_list(&pos->node))
                {
                    slist_del(&pos->node);
                }
                // printk("send timeout:0x%lx\n", pos->th);
                thread_ready(pos->th, TRUE);
            }
        }
        pos = next;
    }

    thread_wait_entry_t *pos2;
    slist_foreach_not_next(
        pos2, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_recv_queue),
        node)
    {
        assert(pos2->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(
            pos2, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_recv_queue),
            node);

        if (pos2->times > 0)
        {
            pos2->times -= tick;
            if (pos2->times <= 0)
            {
                pos2->th->ipc_status = THREAD_TIMEOUT;
                assert(slist_in_list(&pos2->node));
                slist_del(&pos2->node);
                // printk("recv timeout:0x%lx\n", pos2->th);
                thread_ready(pos2->th, TRUE);
            }
        }
        pos2 = next;
    }
}
static void thread_timeout_del_form_send_queue(thread_t *th)
{
    thread_wait_entry_t *pos2;
    assert(th->cpu == arch_get_current_cpu_id());

    slist_foreach_not_next(
        pos2, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_send_queue),
        node_timeout)
    {
        thread_wait_entry_t *next = slist_next_entry(
            pos2, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_send_queue),
            node_timeout);

        if (pos2->th == th)
        {
            slist_del(&pos2->node_timeout);
            if (slist_in_list(&pos2->node))
            {
                slist_del(&pos2->node);
            }
            break;
        }
        pos2 = next;
    }
}
#if IS_ENABLED(CONFIG_SMP)
static int thread_timeout_del_form_send_queue_handler(ipi_msg_t *msg, bool_t *is_sched)
{
    thread_timeout_del_form_send_queue((void *)(msg->msg));
    thread_ready((void *)(msg->msg), TRUE); //!< 直接唤醒接受者
    return 0;
}
#endif
void thread_timeout_del_from_send_queue_remote(thread_t *th)
{
#if IS_ENABLED(CONFIG_SMP)
    if (th->cpu == arch_get_current_cpu_id())
    {
        thread_timeout_del_form_send_queue(th);
        thread_ready(th, TRUE); //!< 直接唤醒接受者
    }
    else
    {
        thread_t *cur_th = thread_get_current();
        cur_th->ipi_msg_node.msg = (umword_t)th;
        cur_th->ipi_msg_node.cb = thread_timeout_del_form_send_queue_handler;
        cpu_ipi_to_msg(1 << th->cpu, &cur_th->ipi_msg_node, IPI_CALL);
    }
#else
    thread_timeout_del_form_send_queue(th);
    thread_ready(th, TRUE); //!< 直接唤醒接受者
#endif
}
static void thread_timeout_del_recv(thread_t *th)
{
    thread_wait_entry_t *pos2;
    assert(th->cpu == arch_get_current_cpu_id());

    slist_foreach_not_next(
        pos2, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_recv_queue),
        node)
    {
        thread_wait_entry_t *next = slist_next_entry(
            pos2, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_recv_queue),
            node);

        if (pos2->th == th)
        {
            slist_del(&pos2->node);
            break;
        }
        pos2 = next;
    }
}
#if IS_ENABLED(CONFIG_SMP)
static int thread_timeout_del_recv_remote_handler(ipi_msg_t *msg, bool_t *is_sched)
{
    thread_t *th = (void *)(msg->msg);
    thread_timeout_del_recv(th);
    thread_ready(th, TRUE); //!< 直接唤醒接受者
    return 0;
}
#endif
void thread_timeout_del_recv_remote(thread_t *th, bool_t is_sche)
{
#if IS_ENABLED(CONFIG_SMP)
    if (th->cpu == arch_get_current_cpu_id())
    {
        thread_timeout_del_recv(th);
        thread_ready(th, is_sche); //!< 直接唤醒接受者
    }
    else
    {
        thread_t *cur_th = thread_get_current();
        cur_th->ipi_msg_node.msg = (umword_t)th;
        cur_th->ipi_msg_node.cb = thread_timeout_del_recv_remote_handler;
        cpu_ipi_to_msg(1 << th->cpu, &cur_th->ipi_msg_node, IPI_CALL);
    }
#else
    thread_timeout_del_recv(th);
    thread_ready(th, is_sche); //!< 直接唤醒接受者
#endif
}
static int ipc_dat_copy_raw(obj_space_t *dst_obj, obj_space_t *src_obj, ram_limit_t *lim,
                            ipc_msg_t *dst_ipc, ipc_msg_t *src_ipc, msg_tag_t tag, int is_reply)
{
    int i = 0;
    if (tag.map_buf_len > 0)
    {
        kobj_del_list_t del;
        int map_len = tag.map_buf_len;

        kobj_del_list_init(&del);

        for (i = 0; i < map_len; i++)
        {
            int ret = 0;

            vpage_t dst_page = vpage_create_raw(dst_ipc->map_buf[i]);
            vpage_t src_page = vpage_create_raw(src_ipc->map_buf[i]);

            // printk("map-> src:%d dst:%d\n", src_page.addr, dst_page.addr);
            if ((src_page.flags & VPAGE_FLAGS_MAP) || is_reply)
            {
                ret = obj_map_src_dst(dst_obj, src_obj,
                                      vpage_get_obj_handler(dst_page),
                                      vpage_get_obj_handler(src_page), lim,
                                      vpage_get_attrs(src_page), &del);
            }

            if (ret < 0)
            {
                return ret;
            }
        }
        kobj_del_list_to_do(&del);
    }
    memcpy(dst_ipc->msg_buf, src_ipc->msg_buf,
           MIN(tag.msg_buf_len * WORD_BYTES, IPC_MSG_SIZE));
    return i;
}
/**
 * @brief ipc传输时的数据拷贝
 *
 * @param dst_th
 * @param src_th
 * @param tag
 * @return int
 */
static int ipc_data_copy(thread_t *dst_th, thread_t *src_th, msg_tag_t tag, int is_reply)
{
    void *src = thread_get_kmsg_buf(src_th);
    void *dst = thread_get_kmsg_buf(dst_th);
    ipc_msg_t *src_ipc;
    ipc_msg_t *dst_ipc;
    task_t *src_tk = thread_get_bind_task(src_th);
    task_t *dst_tk = thread_get_bind_task(dst_th);

    src_ipc = src;
    dst_ipc = dst;

    ipc_dat_copy_raw(&dst_tk->obj_space, &src_tk->obj_space, dst_tk->lim, dst_ipc, src_ipc, tag, is_reply);
    dst_ipc->user[2] = task_pid_get(src_tk);

    dst_th->msg.tag = tag;
    return 0;
}
#if IS_ENABLED(CONFIG_SMP)
static int thread_del_wait_send_handler(ipi_msg_t *msg, bool_t *is_sched)
{
    thread_wait_entry_t *wait = (void *)msg->msg;
    if (slist_in_list(&wait->node_timeout))
    {
        slist_del(&wait->node_timeout);
    }
    thread_ready(wait->th, TRUE);
    return 0;
}
#endif
int thread_del_wait_send_remote(thread_wait_entry_t *wait)
{
    assert(cpulock_get_status() == TRUE);
    thread_t *th = wait->th;
#if IS_ENABLED(CONFIG_SMP)
    if (th->cpu != arch_get_current_cpu_id())
    {
        th->ipi_msg_node.msg = (umword_t)wait;
        th->ipi_msg_node.cb = thread_del_wait_send_handler;
        cpu_ipi_to_msg(1 << th->cpu, &th->ipi_msg_node, IPI_CALL);
    }
    else
#endif
    {
        if (slist_in_list(&wait->node_timeout))
        {
            slist_del(&wait->node_timeout);
        }
        thread_ready(th, FALSE);
    }
    return 0;
}
/**
 * @brief 当前线程接收数据
 *
 * @return int
 */
static int thread_ipc_recv(msg_tag_t *ret_msg, ipc_timeout_t timeout,
                           umword_t *ret_user_id, ipc_t *ipc_kobj,
                           thread_t *recv_obj)
{
    assert(ret_msg);
    assert(ret_user_id);
    int ret = 0;
    umword_t lock_status = 0;
    umword_t lock_status2 = 0;
    thread_wait_entry_t wait = {0};
    slist_head_t *ready_head = NULL; //!< 有发送者
    thread_t *cur_th = thread_get_current();

    assert(cur_th != recv_obj);

    lock_status = cpulock_lock();
    lock_status2 = spinlock_lock(&cur_th->recv_lock);
    if (ipc_kobj)
    {
        /*IPC对象的引用计数+1*/
        ref_counter_inc(&ipc_kobj->ref);
        cur_th->ipc_kobj = &ipc_kobj->kobj;
    }
    else
    {
        cur_th->ipc_kobj = NULL;
    }
#if THREAD_IS_DEBUG
    dbg_printk(TAG "%s:%d ipc recv start: %s[0x%x] status:%d ipc_status:%d recv_obj:0x%x\n", __func__, __LINE__,
               kobject_get_name(&cur_th->kobj), cur_th, cur_th->status, cur_th->ipc_status, recv_obj);
#endif

    if (!slist_is_empty(&cur_th->wait_send_head) && !recv_obj)
    {
        ready_head = slist_first(&cur_th->wait_send_head);
        slist_del(ready_head); //!< 从当前线程中删除唤醒的线程
        thread_wait_entry_t *wait =
            container_of(ready_head, thread_wait_entry_t, node);
        assert(wait->th->status == THREAD_SUSPEND);

        thread_del_wait_send_remote(wait);
#if THREAD_IS_DEBUG
        dbg_printk(TAG "%s:%d ipc recv start3: %s[0x%x] status:%d ipc_status:%d wait_th:0x%x\n", __func__, __LINE__,
                   kobject_get_name(&cur_th->kobj), cur_th, cur_th->status, cur_th->ipc_status, wait->th);
#endif
    }
    else
    {
        if (timeout.recv_timeout)
        {
            thread_wait_entry_init(&wait, cur_th, timeout.recv_timeout);
            // printk("add timeout:0x%lx\n", cur_th);
            slist_add_append(pre_cpu_get_current_cpu_var(&wait_recv_queue),
                             &wait.node); //!< 放到等待队列中
#if THREAD_IS_DEBUG
            dbg_printk(TAG "%s:%d ipc recv app sleep: %s[0x%x] status:%d ipc_status:%d\n", __func__, __LINE__,
                       kobject_get_name(&cur_th->kobj), cur_th, cur_th->status, cur_th->ipc_status);
#endif
        }
    }
    if (recv_obj)
    {
        thread_timeout_del_recv_remote(recv_obj, TRUE);

#if THREAD_IS_DEBUG
        dbg_printk(TAG "%s:%d ipc call wake: %s[0x%x] status:%d ipc_status:%d\n", __func__, __LINE__,
                   kobject_get_name(&recv_obj->kobj), recv_obj, recv_obj->status, recv_obj->ipc_status);
#endif
    }
    // cur_th->has_wait_send_th = FALSE;
    cur_th->has_wait_send_th = TRUE;
    thread_set_ipc_state(cur_th, THREAD_RECV);
    thread_suspend_sw(cur_th, FALSE);
    spinlock_set(&cur_th->recv_lock, lock_status2);
    preemption(); //!< 进行调度
#if THREAD_IS_DEBUG
    dbg_printk(TAG "%s:%d ipc recv wait: %s[0x%x] status:%d ipc_status:%d recv_obj:0x%x\n", __func__, __LINE__,
               kobject_get_name(&cur_th->kobj), cur_th, cur_th->status, cur_th->ipc_status, recv_obj);
#endif
    assert(cur_th->status == THREAD_READY);
    // cur_th->has_wait_send_th = FALSE;
#if THREAD_IS_DEBUG
    dbg_printk(TAG "%s:%d ipc recv wake: %s[0x%x] recv_obj:0x%x\n", __func__, __LINE__,
               kobject_get_name(&cur_th->kobj), cur_th, recv_obj);
#endif
    if (cur_th->ipc_status == THREAD_IPC_ABORT)
    {
        ret = -ESHUTDOWN;
    }
    else if (cur_th->ipc_status == THREAD_TIMEOUT)
    {
        ret = -ERTIMEDOUT;
    }
    else
    {
        *ret_msg = cur_th->msg.tag;
        *ret_user_id = cur_th->user_id;
    }
    cur_th->ipc_status = THREAD_NONE;
    cpulock_set(lock_status);

    return ret;
}
static int thread_ipc_reply_inner(thread_t *form, thread_t *to,
                                  msg_tag_t in_tag)
{
again:;
    mword_t status_lock2 = spinlock_lock(&to->recv_lock);
    if (to->status != THREAD_SUSPEND || to->ipc_status != THREAD_RECV)
    {
        /*TODO:这里应该挂起等待*/
        if (to->ipc_status == THREAD_IPC_ABORT)
        {
            ref_counter_dec_and_release(&to->ref, &to->kobj);
            spinlock_set(&to->recv_lock, status_lock2);
            return -ECANCELED;
        }
        thread_sched(TRUE);
        spinlock_set(&to->recv_lock, status_lock2);
        preemption();
        goto again;
    }
#if THREAD_IS_DEBUG
    dbg_printk(TAG "%s:%d ipc reply: %s[0x%x]-->%s[0x%x]\n", __func__, __LINE__,
               kobject_get_name(&form->kobj), form, kobject_get_name(&to->kobj), to);
#endif
    //!< 发送数据给上一次的发送者
    int ret = ipc_data_copy(to, form, in_tag, TRUE); //!< 拷贝数据

    if (ret < 0)
    {
        in_tag.prot = ret;
    }
    to->msg.tag = in_tag;
    if (to->ipc_status != THREAD_IPC_ABORT)
    {
#if THREAD_IS_DEBUG
        dbg_printk(TAG "%s:%d ipc reply to:%s[0x%x] status:%d\n", __func__, __LINE__,
                   kobject_get_name(&to->kobj), to, to->status);
#endif
        assert(to->status == THREAD_SUSPEND && to->ipc_status == THREAD_RECV);
        thread_timeout_del_from_send_queue_remote(to);
    }
    ref_counter_dec_and_release(&to->ref, &to->kobj);
    spinlock_set(&to->recv_lock, status_lock2);
    return 0;
}
#if IS_ENABLED(CONFIG_SMP)
static int thread_ipc_reply_handler(ipi_msg_t *msg, bool_t *is_sched)
{
    thread_t *form;
    thread_t *to;
    msg_tag_t in_tag;

    form = (thread_t *)msg->msg;
    to = (thread_t *)msg->msg2;
    in_tag = msg_tag_init(msg->msg3);

    return thread_ipc_reply_inner(form, to, in_tag);
}
#endif
int thread_ipc_reply_remote(thread_t *form, thread_t *to, msg_tag_t in_tag)
{
#if IS_ENABLED(CONFIG_SMP)
    if (to->cpu == arch_get_current_cpu_id())
    {
        return thread_ipc_reply_inner(form, to, in_tag);
    }
    else
    {
        to->ipi_msg_node.msg = (umword_t)form;
        to->ipi_msg_node.msg2 = (umword_t)to;
        to->ipi_msg_node.msg3 = (umword_t)in_tag.raw;
        to->ipi_msg_node.cb = thread_ipc_reply_handler;
        cpu_ipi_to_msg(1 << to->cpu, &to->ipi_msg_node, IPI_CALL);

        return to->ipi_msg_node.ret;
    }
#else
    return thread_ipc_reply_inner(form, to, in_tag);
#endif
}
/**
 * @brief 当前线程回复数据给上一次的发送者，回复完成后则释放上一次的接收者指针
 *
 * @param in_tag
 * @return int
 */
static int thread_ipc_reply(msg_tag_t in_tag)
{
    thread_t *cur_th = thread_get_current();
    umword_t status;
    thread_t *last_th;
    int ret;

    if (cur_th->last_send_th == NULL)
    {
        return -1;
    }
    status = spinlock_lock(&cur_th->send_lock);
    if (cur_th->last_send_th == NULL)
    {
        spinlock_set(&cur_th->send_lock, status);
        return -1;
    }
    last_th = cur_th->last_send_th;
    ret = thread_ipc_reply_inner(cur_th, last_th, in_tag);
    spinlock_set(&cur_th->send_lock, status);
    return ret;
}
#if IS_ENABLED(CONFIG_SMP)
static int thread_remote_suspend_handler(ipi_msg_t *msg, bool_t *is_sched)
{
    thread_t *th = (thread_t *)msg->msg;
    assert(th);
    thread_suspend(th);
    return 0;
}
#endif
int thread_suspend_remote(thread_t *th, bool_t is_sche)
{
    assert(cpulock_get_status());
#if IS_ENABLED(CONFIG_SMP)
    if (th->cpu != arch_get_current_cpu_id())
    {
        th->ipi_msg_node.msg = (umword_t)th;
        th->ipi_msg_node.cb = thread_remote_suspend_handler;
        cpu_ipi_to_msg(1 << th->cpu, &th->ipi_msg_node, IPI_CALL);
    }
    else
    {
        thread_suspend_sw(th, is_sche);
    }
#else
    thread_suspend_sw(th, is_sche);
#endif
    return 0;
}

/**
 * @brief 等待某个线程进入挂起状态
 *
 * @param recv_th
 * @param timeout
 * @return int
 */
static int thread_wait_recv_thread(thread_t *recv_th, ipc_timeout_t timeout)
{
    int ret = 0;
    assert(cpulock_get_status() == TRUE);
    thread_t *cur_th = thread_get_current();
    mword_t lock_status;

again_check:
    lock_status = spinlock_lock(&recv_th->recv_lock);
    if ((thread_get_status(recv_th) != THREAD_SUSPEND ||
         thread_get_ipc_state(recv_th) != THREAD_RECV) ||
        recv_th->has_wait_send_th == FALSE /*如果已经在有人准备从这个线程获取数据，则当前call者需要挂起*/)
    {
        thread_wait_entry_t wait;

        // 初始化等待队列
        thread_wait_entry_init(&wait, cur_th, timeout.send_timeout);
#if THREAD_IS_DEBUG
        dbg_printk(TAG "%s:%d ipc wait for: %s[0x%x]\n", __func__, __LINE__,
                   kobject_get_name(&recv_th->kobj), recv_th);
#endif
        slist_add_append(&recv_th->wait_send_head,
                         &wait.node); //!< 放到线程的等待队列中
        slist_add_append(pre_cpu_get_current_cpu_var(&wait_send_queue),
                         &wait.node_timeout);

        recv_th->has_wait_send_th = TRUE;
        thread_set_ipc_state(cur_th, THREAD_SEND);
        thread_suspend_sw(cur_th, FALSE);
        spinlock_set(&recv_th->recv_lock, lock_status);
        preemption();
        if (thread_get_ipc_state(cur_th) == THREAD_IPC_ABORT)
        {
            thread_set_state(cur_th, THREAD_NONE);
            ret = -ESHUTDOWN;
            goto end;
        }
        else if (thread_get_ipc_state(cur_th) == THREAD_TIMEOUT)
        {
            thread_set_state(cur_th, THREAD_NONE);
            ret = -EWTIMEDOUT;
            goto end;
        }
        thread_set_state(cur_th, THREAD_NONE);
        goto again_check;
    }
    else
    {
        recv_th->has_wait_send_th = FALSE;
        spinlock_set(&recv_th->recv_lock, lock_status);
#if THREAD_IS_DEBUG
        dbg_printk(TAG "%s:%d ipc wait end: %s[0x%x] status:%d ipc_status:%d\n", __func__,
                   __LINE__, kobject_get_name(&recv_th->kobj), recv_th, recv_th->status,
                   recv_th->ipc_status);
#endif
    }
end:
    assert(recv_th->status == THREAD_SUSPEND && recv_th->ipc_status == THREAD_RECV);
    return ret;
}
//__attribute__((optimize(0)))
int thread_ipc_call(
    thread_t *to_th, msg_tag_t in_tag, msg_tag_t *ret_tag, ipc_timeout_t timout,
    umword_t *ret_user_id, bool_t is_call)
{
    int ret = -EINVAL;
    if (is_call)
    {
        assert(is_call && ret_tag);
    }
    thread_t *cur_th = thread_get_current();
    thread_t *recv_kobj = to_th;
    mword_t lock_stats2;

#if THREAD_IS_DEBUG
    dbg_printk(TAG "%s:%d ipc call: %s[0x%x]-->%s[0x%x]\n", __func__, __LINE__,
               kobject_get_name(&cur_th->kobj), cur_th,
               kobject_get_name(&recv_kobj->kobj), recv_kobj);
#endif
    lock_stats2 = spinlock_lock(&cur_th->send_lock);
again:
    ret = thread_wait_recv_thread(recv_kobj, timout); //!< 进入接收状态
    if (ret < 0)
    {
        goto end;
    }
    // 下面开始数据接收的流程
    if (thread_get_status(recv_kobj) == THREAD_SUSPEND && thread_get_ipc_state(recv_kobj) == THREAD_RECV)
    {
        //!< 开始发送数据
        ret = ipc_data_copy(recv_kobj, cur_th, in_tag, FALSE); //!< 拷贝数据
        if (ret < 0)
        {
            //!< 拷贝失败
            goto end;
        }
#if THREAD_IS_DEBUG
        dbg_printk(TAG "%s:%d ipc send data success: %s[0x%x]-->%s[0x%x]\n", __func__,
                   __LINE__, kobject_get_name(&cur_th->kobj), cur_th,
                   kobject_get_name(&recv_kobj->kobj), recv_kobj);
#endif
        if (is_call)
        {
            if (recv_kobj->ipc_kobj)
            {
                // 绑定回复的ipc到当前的线程
                assert(ipc_bind(((ipc_t *)(recv_kobj->ipc_kobj)), -1, 0, cur_th) >= 0);
                ref_counter_dec_and_release(&((ipc_t *)(recv_kobj->ipc_kobj))->ref,
                                            recv_kobj->ipc_kobj);
                recv_kobj->ipc_kobj = NULL;
                recv_kobj->last_send_th = NULL;
            }
            else
            {
                ref_counter_inc(&cur_th->ref);    //!< 作为发送者增加一次引用
                recv_kobj->last_send_th = cur_th; //!< 设置接收者的上一次发送者是谁
            }

            ret = thread_ipc_recv(ret_tag, timout, ret_user_id, NULL,
                                  recv_kobj); //!< 当前线程进行接收
            if (ret < 0)
            {
                //!< 接收超时
                goto end;
            }
        }
        else
        {
            thread_timeout_del_recv_remote(recv_kobj, TRUE);
#if THREAD_IS_DEBUG
            dbg_printk(TAG "%s:%d ipc call wake: %s[0x%x]\n", __func__, __LINE__,
                       kobject_get_name(&recv_kobj->kobj), recv_kobj);
#endif
        }
        preemption();
    }
    else
    {
        printk("recv_kobj->status:%d, recv_kobj->ipc_status:%d", recv_kobj->status,
               recv_kobj->ipc_status);
        assert(0);
        // goto again;
    }
    ret = 0;
end:
    spinlock_set(&cur_th->send_lock, lock_stats2);
#if THREAD_IS_DEBUG
    dbg_printk(TAG "%s:%d ipc call done: %s[0x%x]-->%s[0x%x]\n", __func__, __LINE__,
               kobject_get_name(&cur_th->kobj), cur_th,
               kobject_get_name(&recv_kobj->kobj), recv_kobj);
#endif
    return ret;
}
msg_tag_t thread_fast_ipc_call(thread_t *to_th, entry_frame_t *f, umword_t user_id)
{
    task_t *cur_task = thread_get_current_task();
    thread_t *cur_th = thread_get_current();
    msg_tag_t in_tag = msg_tag_init(f->regs[0]);
    int ret;
    // 1.需要切换的资源有r9，task，和用户栈
    // 2.在对方进程中被杀死，需要还原当前线程的状态，并返回一个错误
    // 3.多线程访问时，服务端提供一个小的用户线程栈，然后内核到用户部分为临界区域，在服务端重新分配用户栈用，使用新的用户栈。
    // 4.fastipc嵌套访问会有问题，内核必须要提供一个软件上的调用栈。
    // 在嵌套调用时，如果在其它进程中挂掉，如果是当前线程则需要还原
    task_t *to_task = thread_get_bind_task(to_th);

    if (to_task->nofity_point == NULL)
    {
        printk("task:0x%x, notify point is not set.\n", to_task);
        return msg_tag_init4(0, 0, 0, -EIO);
    }
_to_unlock:
    if (GET_LSB(*to_task->nofity_bitmap, to_task->nofity_bitmap_len) == GET_LSB((~0ULL), to_task->nofity_bitmap_len))
    {
        thread_sched(TRUE); /*TODO:应该挂起，并在释放时唤醒*/
        preemption();
        goto _to_unlock;
    }
    mutex_lock(&to_task->nofity_lock);
    if (GET_LSB(*to_task->nofity_bitmap, to_task->nofity_bitmap_len) == GET_LSB((~0ULL), to_task->nofity_bitmap_len))
    {
        mutex_unlock(&to_task->nofity_lock);
        thread_sched(TRUE); /*TODO:应该挂起，并在释放时唤醒*/
        preemption();
        goto _to_unlock;
    }
    umword_t cpu_status = cpulock_lock();
    ref_counter_inc((&to_task->ref_cn));
    //!< 执行目标线程时用的是当前线程的资源，这里还需要备份当前线程的上下文。
    ret = thread_fast_ipc_save(cur_th, to_task, (void *)(to_task->nofity_stack - 4 * 8 /*FIXME:改成宏*/)); //!< 备份栈和usp
    if (ret >= 0)
    {
        ipc_msg_t *dst_ipc = (void *)to_task->nofity_msg_buf;
        ipc_msg_t *src_ipc = (void *)cur_th->msg.msg;
        ret = ipc_dat_copy_raw(&to_task->obj_space, &cur_task->obj_space, to_task->lim,
                               dst_ipc, src_ipc, in_tag, FALSE);
        if (ret >= 0)
        {
            dst_ipc->user[2] = task_pid_get(cur_task);                       // 设置pid
            slist_add(&to_task->nofity_theads_head, &cur_th->fast_ipc_node); // 添加到链表中，用于进程关闭时进行释放
            pf_s_t *usr_stask_point = (void *)arch_get_user_sp();

            if (thread_is_knl(cur_th))
            {
                // 如果是内核线程则全部重新设置
                thread_set_user_pf_noset_knl_sp(cur_th, to_task->nofity_point,
                                   (void *)to_task->nofity_stack, (void *)to_task->mm_space.mm_block);
                usr_stask_point->rg0[0] = in_tag.raw;
                usr_stask_point->rg0[1] = user_id;
                usr_stask_point->rg0[2] = f->regs[2];
                usr_stask_point->rg0[3] = f->regs[3];

                scheduler_get_current()->sched_reset = 2;
            }
            else
            {
                usr_stask_point->r12 = 0x12121212;
                usr_stask_point->xpsr = 0x01000000L;
                usr_stask_point->lr = (umword_t)NULL; //!< 线程退出时调用的函数
                usr_stask_point->pc = (umword_t)(to_task->nofity_point) | 0x1;

                //! 获得内核栈栈顶
                pf_t *cur_pf = ((pf_t *)((char *)cur_th + CONFIG_THREAD_BLOCK_SIZE + 8)) - 1;
                // 重新设置r9寄存器
                cur_pf->regs[5] = (umword_t)(to_task->mm_space.mm_block);

                //! 寄存器传参数
                f->regs[0] = in_tag.raw;
                f->regs[1] = user_id;
                f->regs[2] = f->regs[2];
                f->regs[3] = f->regs[3];
            }
            // 切换mpu
            mpu_switch_to_task(to_task);
            cpulock_set(cpu_status);
            if (thread_is_knl(cur_th))
            {
                // 内核线程则立刻进行调度
                arch_to_sche();
                preemption();
            }
            return in_tag;
        }
        else
        {
            ref_counter_dec_and_release(&to_task->ref_cn, &to_task->kobj);
            mutex_unlock(&to_task->nofity_lock);
        }
    }
    else
    {
        ref_counter_dec_and_release(&to_task->ref_cn, &to_task->kobj);
        mutex_unlock(&to_task->nofity_lock);
    }
    cpulock_set(cpu_status);

    return msg_tag_init4(0, 0, 0, ret);
}
msg_tag_t thread_fast_ipc_replay(entry_frame_t *f)
{
    task_t *cur_task = thread_get_current_task();
    thread_t *cur_th = thread_get_current();
    msg_tag_t in_tag = msg_tag_init(f->regs[0]);
    task_t *old_task = thread_get_bind_task(cur_th);
    int ret;

    *(cur_task->nofity_bitmap) &= ~(1 << MIN(f->regs[2], cur_task->nofity_bitmap_len)); //!< 解锁bitmap
    slist_del(&cur_th->fast_ipc_node);                                                  // 从链表中删除

    ret = thread_fast_ipc_restore(cur_th); // 还原栈和usp
    if (ret < 0)
    {
        mutex_unlock(&old_task->nofity_lock);
        return msg_tag_init4(0, 0, 0, ret);
    }
    umword_t cpu_status = cpulock_lock();

    cur_task = thread_get_current_task();
    ipc_msg_t *dst_ipc = (void *)cur_th->msg.msg;
    ipc_msg_t *src_ipc = (void *)old_task->nofity_msg_buf;
    ret = ipc_dat_copy_raw(&cur_task->obj_space, &old_task->obj_space, cur_task->lim,
                           dst_ipc, src_ipc, in_tag, TRUE); // copy数据
    // if (ret >=0 ) {
    for (int i = 0; i < CONFIG_THREAD_MAP_BUF_LEN; i++)
    {
        if (i < ret)
        {
            src_ipc->map_buf[i] = old_task->nofity_map_buf[i];
            old_task->nofity_map_buf[i] = 0;
        }
        else
        {
            src_ipc->map_buf[i] = old_task->nofity_map_buf[i];
        }
    }
    // }
    mutex_unlock(&old_task->nofity_lock);
    if (thread_is_knl(cur_th))
    {
        //! 吧r4-r11留出来
        // memcpy((char *)cur_th->sp.knl_sp - 4 * 8 /*FIXME:*/, (char *)cur_th->sp.knl_sp, 4 * 8);
        // memset(cur_th->sp.knl_sp, 0, 4 * 8);
        // cur_th->sp.knl_sp = (char *)cur_th->sp.knl_sp - 4 * 8;
        cur_th->sp.user_sp = 0x0;
        cur_th->sp.sp_type = 0xfffffff9;
        scheduler_get_current()->sched_reset = 3;
    }
    else
    {
        pf_t *cur_pf = ((pf_t *)((char *)cur_th + CONFIG_THREAD_BLOCK_SIZE + 8)) - 1;

        cur_pf->regs[5] = (umword_t)(cur_task->mm_space.mm_block); // 更新r9寄存器
    }
    mpu_switch_to_task(cur_task); // 切换mpu
    ref_counter_dec_and_release(&old_task->ref_cn, &old_task->kobj);
    if (ret < 0)
    {
        in_tag = msg_tag_init4(0, 0, 0, ret);
    }
    cpulock_set(cpu_status);
    if (thread_is_knl(cur_th))
    {
        arch_to_sche();
        preemption();
    }
    return in_tag;
}
/**
 * @brief 执行ipc
 *
 * @param kobj
 * @param in_tag
 * @param f
 * @return int
 */
msg_tag_t thread_do_ipc(kobject_t *kobj, entry_frame_t *f, umword_t user_id)
{
    assert(kobj);
    task_t *cur_task = thread_get_current_task();
    thread_t *cur_th = thread_get_current();
    thread_t *to_th = (thread_t *)kobj;
    umword_t ipc_type = f->regs[1];
    obj_handler_t th_hd = 0;
    int ret = -EINVAL;

    switch (ipc_type)
    {
    case IPC_FAST_REPLAY:
    {
        return thread_fast_ipc_replay(f);
    }
    break;
    case IPC_FAST_CALL:
    {
        return thread_fast_ipc_call(to_th, f, user_id);
    }
    break;
    case IPC_CALL:
    {
        msg_tag_t in_tag = msg_tag_init(f->regs[0]);
        msg_tag_t recv_tag;
        th_hd = f->regs[2];
        ipc_timeout_t ipc_tm_out = ipc_timeout_create(f->regs[3]);

        to_th->user_id = user_id;
        ret = thread_ipc_call(to_th, in_tag, &recv_tag, ipc_tm_out, &f->regs[1],
                              TRUE);
        if (ret < 0)
        {
            return msg_tag_init4(0, 0, 0, ret);
        }
        return recv_tag;
    }
    case IPC_REPLY:
    {
        msg_tag_t in_tag = msg_tag_init(f->regs[0]);

        ret = thread_ipc_reply(in_tag);
        return msg_tag_init4(0, 0, 0, ret);
    }
    case IPC_RECV:
    case IPC_WAIT:
    {
        msg_tag_t ret_msg;
        ipc_timeout_t ipc_tm_out = ipc_timeout_create(f->regs[3]);
        kobject_t *ipc_kobj = obj_space_lookup_kobj_cmp_type(
            &cur_task->obj_space, f->regs[4], IPC_TYPE);

        int ret = thread_ipc_recv(&ret_msg, ipc_tm_out, &f->regs[1],
                                  (ipc_t *)ipc_kobj, NULL);
        if (ret < 0)
        {
            return msg_tag_init4(0, 0, 0, ret);
        }
        return ret_msg;
    }
    case IPC_SEND:
    {
        msg_tag_t in_tag = msg_tag_init(f->regs[0]);
        msg_tag_t recv_tag;
        // th_hd = f->regs[2];
        ipc_timeout_t ipc_tm_out = ipc_timeout_create(f->regs[3]);

        to_th->user_id = user_id;
        ret = thread_ipc_call(to_th, in_tag, NULL, ipc_tm_out, NULL, FALSE);
        return msg_tag_init4(0, 0, 0, ret);
    }
    default:
        ret = -ENOSYS;
        break;
    }

    return msg_tag_init4(0, 0, 0, ret);
}
#if IS_ENABLED(CONFIG_SMP)
static int thread_remote_migration(ipi_msg_t *msg, bool_t *is_sched)
{
    thread_t *th = (thread_t *)msg->msg;
    umword_t tagcpu = msg->msg2;
    int cur_cpu = arch_get_current_cpu_id();
    assert(th);
    assert(tagcpu == cur_cpu);
    assert(th->cpu != cur_cpu);

    // 在迁移之前必须在之前的核中已经挂起该线程
    assert(!slist_in_list(&th->sche.node));
    assert(th->cpu != cur_cpu);

    assert(th->status == THREAD_SUSPEND || th->status == THREAD_IDLE);
    assert(th->magic == THREAD_MAGIC);

    th->cpu = cur_cpu;
    *is_sched = FALSE;
    // 在新的核上调度
    thread_ready(th, TRUE);
    return 0;
}

#endif
static void thread_syscall(kobject_t *kobj, syscall_prot_t sys_p,
                           msg_tag_t in_tag, entry_frame_t *f)
{
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    task_t *task = thread_get_current_task();
    thread_t *tag_th = container_of(kobj, thread_t, kobj);
    thread_t *cur_th = thread_get_current();

    if (sys_p.prot != THREAD_PROT)
    {
        f->regs[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }

    switch (sys_p.op)
    {
    case SET_EXEC_REGS:
    {
        thread_set_exc_regs(tag_th, f->regs[1], f->regs[2], f->regs[3]);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case MSG_BUG_SET:
    {
        task_t *tag_tk = thread_get_bind_task(tag_th);

        if (is_rw_access(tag_tk, (void *)(f->regs[1]), THREAD_MSG_BUG_LEN,
                         FALSE))
        {
            void *kmsg = (void *)task_get_paddr(tag_tk, f->regs[1]);

            if (kmsg)
            {
#if IS_ENABLED(CONFIG_MMU)
                thread_set_msg_buf(tag_th, kmsg, (void *)(f->regs[1]));
#else
                thread_set_msg_buf(tag_th, (void *)(f->regs[1]),
                                   (void *)(f->regs[1]));
#endif
                tag = msg_tag_init4(0, 0, 0, 0);
            }
            else
            {
                tag = msg_tag_init4(0, 0, 0, -EINVAL);
            }
        }
        else
        {
            //!< 内存不可访问
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        }
    }
    case MSG_BUG_GET:
    {
        f->regs[1] = (umword_t)(thread_get_msg_buf(tag_th));
        f->regs[2] = THREAD_MSG_BUG_LEN;
        if (thread_get_msg_buf(tag_th) == NULL)
        {
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        }
        else
        {
            tag = msg_tag_init4(0, 0, 0, 0);
        }
    }
    break;
    case RUN_THREAD:
    {
        task_t *tag_tsk = thread_get_bind_task(tag_th);
        if (tag_tsk == NULL)
        {
            tag = msg_tag_init4(0, 0, 0, -EACCES);
            break;
        }
        if (task_pid_get(tag_tsk) == -1)
        {
            //!< 只有设置了pid才能启动，pid只有init进程能够设置，这就使得只有pid能够启动应用程序
            tag = msg_tag_init4(0, 0, 0, -EACCES);
            break;
        }
        int cur_cpu = arch_get_current_cpu_id();
        mword_t tge_cpu = f->regs[2];
        int tge_prio = f->regs[1];
        umword_t status = cpulock_lock();

#if IS_ENABLED(CONFIG_SMP)
        if (tge_cpu >= CONFIG_CPU)
        {
            tge_cpu = cur_cpu;
        }
        if (tge_cpu < 0)
        {
            tge_cpu = cur_cpu;
        }
        // 刚刚创建的新线程直接运行
        if (tag_th->status == THREAD_NONE)
        {
            tag_th->sche.prio = (tge_prio >= PRIO_MAX ? PRIO_MAX - 1 : tge_prio);
            tag_th->cpu = tge_cpu;
            thread_ready_remote(tag_th, TRUE);
            goto run_thread_end;
        }

        if (tge_cpu == cur_cpu)
        {
            // 就在当前核，则直接修改优先级
            if (tag_th == cur_th)
            {
                thread_suspend_sw(tag_th, FALSE);
            }
            else
            {
                thread_suspend(tag_th);
            }
            tag_th->sche.prio =
                (tge_prio >= PRIO_MAX ? PRIO_MAX - 1 : tge_prio);
            thread_ready(tag_th, TRUE);
            goto run_thread_end;
        }
        if (tag_th != cur_th)
        {
            thread_suspend_remote(tag_th, TRUE);
        }
        tag_th->sche.prio = (tge_prio >= PRIO_MAX ? PRIO_MAX - 1 : tge_prio);
        tag_th->ipi_msg_node.msg = (umword_t)tag_th;
        tag_th->ipi_msg_node.msg2 = tge_cpu;
        tag_th->ipi_msg_node.msg3 = tge_prio;
        tag_th->ipi_msg_node.cb = thread_remote_migration;
        cpu_ipi_to_msg(1 << tge_cpu, &cur_th->ipi_msg_node, IPI_CALL);
        assert(tag_th->cpu == tge_cpu);
#else
        if (!slist_in_list(&tag_th->sche.node))
        {
            tag_th->sche.prio = tge_prio;
            thread_ready(tag_th, TRUE);
        }
        else
        {
            thread_suspend(tag_th);
            if (tag_th != thread_get_current())
            {
                preemption();
            }
            tag_th->sche.prio =
                (tge_prio >= PRIO_MAX ? PRIO_MAX - 1 : tge_prio);
            thread_ready(tag_th, TRUE);
        }
#endif
    run_thread_end:
        cpulock_set(status);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case BIND_TASK:
    {
        kobject_t *task_kobj = obj_space_lookup_kobj_cmp_type(
            &task->obj_space, f->regs[1], TASK_TYPE);
        if (task_kobj == NULL)
        {
            f->regs[0] = msg_tag_init4(0, 0, 0, -ENOENT).raw;
            return;
        }
        thread_bind(tag_th, task_kobj);
        tag = msg_tag_init4(0, 0, 0, 0);
        // printk("thread bind to %d\n", f->regs[1]);
    }
    break;
    case YIELD:
    {
        thread_sched(TRUE);
        // arch_to_sche();
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case DO_IPC:
    {
        tag = thread_do_ipc(kobj, f, 0);
    }
    break;
    case SET_EXEC:
    {
        kobject_t *th_kobj = obj_space_lookup_kobj_cmp_type(
            &task->obj_space, f->regs[0], THREAD_TYPE);
        if (th_kobj == NULL)
        {
            f->regs[0] = msg_tag_init4(0, 0, 0, -EINVAL).raw;
            return;
        }
        ref_counter_inc(&((thread_t *)th_kobj)->ref);
        task->exec_th = th_kobj;
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    }
    f->regs[0] = tag.raw;
}

/**
 * @brief 该函数创建一个工厂对象
 *
 * @param lim
 * @param arg0
 * @param arg1
 * @param arg2
 * @param arg3
 * @return kobject_t*
 */
static kobject_t *thread_create_func(ram_limit_t *lim, umword_t arg0,
                                     umword_t arg1, umword_t arg2,
                                     umword_t arg3)
{
    kobject_t *kobj;

    kobj = (kobject_t *)thread_create(lim, arg0);
    if (!kobj)
    {
        return NULL;
    }
    return kobj;
}

/**
 * @brief 工厂注册函数
 *
 */
static void thread_factory_register(void)
{
    factory_register(thread_create_func, THREAD_PROT);
}
INIT_KOBJ(thread_factory_register);
/**
 * @brief 获取当前的task
 *
 * @return task_t*
 */
task_t *thread_get_current_task(void)
{
    thread_t *cur = thread_get_current();
    kobject_t *kobj = cur->task;

    if (!kobj)
    {
        return NULL;
    }
    return container_of(kobj, task_t, kobj);
}
task_t *thread_get_task(thread_t *th)
{
    return container_of(th->task, task_t, kobj);
}
