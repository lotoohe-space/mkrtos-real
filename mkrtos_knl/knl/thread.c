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
#include <arch.h>
#include <pre_cpu.h>
#if IS_ENABLED(CONFIG_SMP)
#include <ipi.h>
#endif
enum thread_op {
    SET_EXEC_REGS,
    RUN_THREAD,
    BIND_TASK,
    MSG_BUG_GET,
    MSG_BUG_SET,
    YIELD,
    DO_IPC = 6, //!< 与ipc对象中的额IPC_DO一致
};
enum IPC_TYPE {
    IPC_CALL,
    IPC_REPLY,
    IPC_WAIT,
    IPC_RECV,
    IPC_SEND,
};
static void thread_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f);
static bool_t thread_put(kobject_t *kobj);
static void thread_release_stage1(kobject_t *kobj);
static void thread_release_stage2(kobject_t *kobj);

typedef struct thread_wait_entry {
    slist_head_t node;
    slist_head_t node_timeout;
    thread_t *th;
    mword_t times;
} thread_wait_entry_t;

static inline void thread_wait_entry_init(thread_wait_entry_t *entry, thread_t *th, mword_t times)
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
    for (int i = 0; i < CONFIG_CPU; i++) {
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
void thread_init(thread_t *th, ram_limit_t *lim)
{
    kobject_init(&th->kobj, THREAD_TYPE);
    sched_init(&th->sche);
    slist_init(&th->futex_node);
    slist_init(&th->wait_send_head);
    ref_counter_init(&th->ref);
    ref_counter_inc(&th->ref);
    th->cpu = 0;
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
static void thread_release_stage1(kobject_t *kobj)
{
    thread_t *th = container_of(kobj, thread_t, kobj);
    thread_t *cur = thread_get_current();
    kobject_invalidate(kobj);

    if (cur != th) {
        //! 线程在运行中，则挂起线程
        if (th->status == THREAD_READY) {
            thread_suspend(th);
        }
        th->ipc_status = THREAD_IPC_ABORT;
    } else {
        if (cur->status == THREAD_READY) {
            thread_suspend(th);
        }
        cur->ipc_status = THREAD_IPC_ABORT;
    }
    thread_wait_entry_t *pos;

    slist_foreach_not_next(pos, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_send_queue), node_timeout)
    {
        assert(pos->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(pos, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_send_queue), node_timeout);

        if (pos->th != th) {
            pos->th->ipc_status = THREAD_IPC_ABORT;
            thread_ready(pos->th, TRUE);
        }

        slist_del(&pos->node_timeout);
        if (slist_in_list(&pos->node)) {
            slist_del(&pos->node);
        }
        pos = next;
    }
    thread_wait_entry_t *pos2;

    slist_foreach_not_next(pos2, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_recv_queue), node)
    {
        assert(pos2->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(pos2, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_recv_queue), node);

        slist_del(&pos2->node);
        if (pos2->th != th) {
            pos2->th->ipc_status = THREAD_IPC_ABORT;
            thread_ready(pos2->th, TRUE);
        }
        pos2 = next;
    }
    thread_unbind(th);
}
static void thread_release_stage2(kobject_t *kobj)
{
    thread_t *th = container_of(kobj, thread_t, kobj);
    thread_t *cur_th = thread_get_current();
    // printk("release thread 0x%x\n", kobj);

    if (cur_th == th) {
        scheduler_reset();
        thread_sched(FALSE);
        arch_to_sche();
    }
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    mm_limit_free_buddy(th->lim, kobj, THREAD_BLOCK_SIZE);
#else
    mm_limit_free_align(th->lim, kobj, THREAD_BLOCK_SIZE);
    // mm_trace();
#endif
}

/**
 * @brief 设置运行寄存器
 *
 * @param th
 * @param pc
 * @param ip
 */
void thread_set_exc_regs(thread_t *th, umword_t pc, umword_t user_sp, umword_t ram, umword_t stack)
{
    thread_user_pf_set(th, (void *)pc, (void *)user_sp, (void *)ram, stack);
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
    if (th->task) {
        task_t *tsk = container_of(th->task, task_t, kobj);

        ref_counter_dec_and_release(&tsk->ref_cn, &th->kobj);
        th->task = NULL;
    }
}
void thread_suspend_sw(thread_t *th, bool_t is_sche)
{
    assert(slist_in_list(&th->sche.node));
    assert(th->cpu == arch_get_current_cpu_id());
    umword_t status = cpulock_lock();

    scheduler_del(&th->sche);
    th->status = THREAD_SUSPEND;

    /* 当前线程才能够切出，不是当前线程执行这个也没有效果，而且还会导致当前线程切出*/
    if (th == thread_get_current() && is_sche) {
        thread_sched(TRUE);
    } else {
        arch_to_sche(); //!< 触发调度中断
    }
    // arch_to_sche();
    // printk("suspend: cpu:%d sp:0x%lx\n", arch_get_current_cpu_id(), th->sp.sp);
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
    if (!slist_in_list(&th->sche.node)) {
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

    assert(th->magic == THREAD_MAGIC);
    if (next_sche == &th->sche) {
        //!< 线程没有发生变化，则不用切换
        cpulock_set(status);

        return FALSE;
    }
    if (is_sche) {
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
/**
 * @brief 线程进入就绪态
 *
 * @param th
 */
void thread_ready_to_cpu(thread_t *th, int cpu, bool_t is_sche)
{
    assert(cpu < CONFIG_CPU);
    umword_t status = cpulock_lock();

    assert(!slist_in_list(&th->sche.node));
    scheduler_add_to_cpu(&th->sche, cpu);
    th->status = THREAD_READY;
    if (is_sche) {
        umword_t status = cpulock_lock();
        sched_t *next_sche = scheduler_next_cpu(cpu);

        assert(th->magic == THREAD_MAGIC);
    }
    cpulock_set(status);
}
#if IS_ENABLED(CONFIG_SMP)
static int thread_ready_remote_handler(ipi_msg_t *msg)
{
    thread_ready((thread_t *)(msg->msg), msg->msg2);
    return 0;
}
#endif
void thread_ready_remote(thread_t *th, bool_t is_sche)
{
#if IS_ENABLED(CONFIG_SMP)
    if (th->cpu == arch_get_current_cpu_id()) {
        thread_ready(th, is_sche);
    } else {
        th->ipi_msg_node.msg = (umword_t)th;
        th->ipi_msg_node.msg2 = is_sche;
        th->ipi_msg_node.cb = thread_ready_remote_handler;
        cpu_ipi_to_msg(1 << th->cpu, &th->ipi_msg_node, IPI_CALL);
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

    assert(!slist_in_list(&th->sche.node));
    assert(th->cpu == arch_get_current_cpu_id());
    th->status = THREAD_READY;
    ret = scheduler_add(&th->sche);
    if (is_sche && ret && th == thread_get_current()) {
        // ready线程的优先级大于最大优先级
        thread_sched(TRUE);
    } else {
        arch_to_sche();
    }
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
    if (is_sche) {
        thread_sched(TRUE);
    }
}
/**
 * @brief 创建线程
 *
 * @param ram
 * @return thread_t*
 */
thread_t *thread_create(ram_limit_t *ram)
{
    thread_t *th = NULL;

#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    th = mm_limit_alloc_buddy(ram, THREAD_BLOCK_SIZE);
#else
    th = mm_limit_alloc_align(ram, THREAD_BLOCK_SIZE, THREAD_BLOCK_SIZE);
#endif
    if (!th) {
        return NULL;
    }
    // assert(((mword_t)th & (~(THREAD_BLOCK_SIZE - 1))) == 0);
    memset(th, 0, THREAD_BLOCK_SIZE);
    thread_init(th, ram);
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

    slist_foreach_not_next(pos, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_send_queue), node_timeout)
    {
        assert(pos->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(pos, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_send_queue), node_timeout);
        if (pos->times > 0) {
            pos->times -= tick;
            if (pos->times <= 0) {
                pos->th->ipc_status = THREAD_TIMEOUT;

                slist_del(&pos->node_timeout);
                if (slist_in_list(&pos->node)) {
                    slist_del(&pos->node);
                }
                // printk("send timeout:0x%lx\n", pos->th);
                thread_ready(pos->th, TRUE);
            }
        }
        pos = next;
    }

    thread_wait_entry_t *pos2;
    slist_foreach_not_next(pos2,
                           (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_recv_queue), node)
    {
        assert(pos2->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(pos2,
                                                     (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_recv_queue), node);

        if (pos2->times > 0) {
            pos2->times -= tick;
            if (pos2->times <= 0) {
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
static void thread_timeout_del_recv(thread_t *th)
{
    thread_wait_entry_t *pos2;
    assert(th->cpu == arch_get_current_cpu_id());

    slist_foreach_not_next(pos2, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_recv_queue), node)
    {
        thread_wait_entry_t *next = slist_next_entry(pos2, (slist_head_t *)pre_cpu_get_current_cpu_var(&wait_recv_queue), node);

        if (pos2->th == th) {

            slist_del(&pos2->node);
            break;
        }
        pos2 = next;
    }
}
#if IS_ENABLED(CONFIG_SMP)
static int thread_timeout_del_recv_remote_handler(ipi_msg_t *msg)
{
    thread_timeout_del_recv((void *)(msg->msg));
    thread_ready((void *)(msg->msg), TRUE); //!< 直接唤醒接受者
    return 0;
}
#endif
void thread_timeout_del_recv_remote(thread_t *th)
{
#if IS_ENABLED(CONFIG_SMP)
    if (th->cpu == arch_get_current_cpu_id()) {
        thread_timeout_del_recv(th);
        thread_ready(th, TRUE); //!< 直接唤醒接受者
    } else {
        th->ipi_msg_node.msg = (umword_t)th;
        th->ipi_msg_node.cb = thread_timeout_del_recv_remote_handler;
        cpu_ipi_to_msg(1 << th->cpu, &th->ipi_msg_node, IPI_CALL);
    }
#else
    thread_timeout_del_recv(th);
    thread_ready(th, TRUE); //!< 直接唤醒接受者
#endif
}
/**
 * @brief ipc传输时的数据拷贝
 *
 * @param dst_th
 * @param src_th
 * @param tag
 * @return int
 */
static int ipc_data_copy(thread_t *dst_th, thread_t *src_th, msg_tag_t tag)
{
    void *src = thread_get_kmsg_buf(src_th);
    void *dst = thread_get_kmsg_buf(dst_th);
    ipc_msg_t *src_ipc;
    ipc_msg_t *dst_ipc;
    task_t *src_tk = thread_get_bind_task(src_th);
    task_t *dst_tk = thread_get_bind_task(dst_th);

    src_ipc = src;
    dst_ipc = dst;

    if (tag.map_buf_len > 0) {
        kobj_del_list_t del;
        int map_len = tag.map_buf_len;

        kobj_del_list_init(&del);

        for (int i = 0; i < map_len; i++) {
            int ret = 0;

            vpage_t dst_page = vpage_create_raw(dst_ipc->map_buf[i]);
            vpage_t src_page = vpage_create_raw(src_ipc->map_buf[i]);

            if (src_page.flags & VPAGE_FLAGS_MAP) {
                ret = obj_map_src_dst(&dst_tk->obj_space, &src_tk->obj_space,
                                      vpage_get_obj_handler(dst_page),
                                      vpage_get_obj_handler(src_page),
                                      dst_tk->lim,
                                      vpage_get_attrs(src_page), &del);
            }

            if (ret < 0) {
                return ret;
            }
        }
        kobj_del_list_to_do(&del);
    }
    dst_ipc->user[2] = task_pid_get(src_tk);
    memcpy(dst_ipc->msg_buf, src_ipc->msg_buf, MIN(tag.msg_buf_len * WORD_BYTES, IPC_MSG_SIZE));
    dst_th->msg.tag = tag;
    return 0;
}
#if IS_ENABLED(CONFIG_SMP)
static int thread_del_wait_send_handler(ipi_msg_t *msg)
{
    thread_wait_entry_t *wait = (void *)msg->msg;
    if (slist_in_list(&wait->node_timeout)) {
        slist_del(&wait->node_timeout);
    }
    thread_ready(wait->th, TRUE);
    return 0;
}
#endif
int thread_del_wait_send_remote(thread_wait_entry_t *wait)
{
#if IS_ENABLED(CONFIG_SMP)
    thread_t *th = wait->th;

    if (th->cpu != arch_get_current_cpu_id()) {
        th->ipi_msg_node.msg = (umword_t)wait;
        th->ipi_msg_node.cb = thread_del_wait_send_handler;
        cpu_ipi_to_msg(1 << th->cpu, &th->ipi_msg_node, IPI_CALL);
    } else {
        if (slist_in_list(&wait->node_timeout)) {
            slist_del(&wait->node_timeout);
        }
        thread_ready(wait->th, TRUE);
    }
#else
    if (slist_in_list(&wait->node_timeout)) {
        slist_del(&wait->node_timeout);
    }
    thread_ready(wait->th, TRUE);
#endif
    return 0;
}
/**
 * @brief 当前线程接收数据
 *
 * @return int
 */
static int thread_ipc_recv(msg_tag_t *ret_msg, ipc_timeout_t timeout,
                           umword_t *ret_user_id, ipc_t *ipc_kobj, thread_t *recv_obj)
{
    assert(ret_msg);
    assert(ret_user_id);
    int ret = 0;
    thread_t *cur_th = thread_get_current();
    umword_t lock_status;
    thread_wait_entry_t wait;
    //!< 有发送者
    slist_head_t *ready_head = NULL;

    lock_status = cpulock_lock();
    if (ipc_kobj) {
        /*IPC对象的引用计数+1*/
        ref_counter_inc(&ipc_kobj->ref);
        cur_th->ipc_kobj = &ipc_kobj->kobj;
    } else {
        cur_th->ipc_kobj = NULL;
    }
    // 先吧当前线程从调度队列中删除
    if (!recv_obj) {
        lock_status = spinlock_lock(&cur_th->wait_send_head_lock);
    }
    assert(slist_in_list(&cur_th->sche.node));
    assert(cur_th->cpu == arch_get_current_cpu_id());
    cur_th->status = THREAD_SUSPEND;
    cur_th->ipc_status = THREAD_RECV; //!< 因为接收挂起
    scheduler_del(&cur_th->sche);

    if (recv_obj) {
        // thread_ready_remote(recv_obj, TRUE);
        thread_timeout_del_recv_remote(recv_obj);
    }
    if (!slist_is_empty(&cur_th->wait_send_head) && !recv_obj) {
        ready_head = slist_first(&cur_th->wait_send_head);
        slist_del(ready_head); //!< 从当前线程中删除唤醒的线程
        if (!recv_obj) {
            spinlock_set(&cur_th->wait_send_head_lock, lock_status);
        }
        thread_wait_entry_t *wait = container_of(ready_head, thread_wait_entry_t, node);
        assert(wait->th->status == THREAD_SUSPEND);

        thread_del_wait_send_remote(wait);
    } else {

        if (timeout.recv_timeout) {
            thread_wait_entry_init(&wait, cur_th, timeout.recv_timeout);
            // printk("add timeout:0x%lx\n", cur_th);
            slist_add_append(pre_cpu_get_current_cpu_var(&wait_recv_queue),
                             &wait.node); //!< 放到等待队列中
        }
        if (!recv_obj) {
            spinlock_set(&cur_th->wait_send_head_lock, lock_status);
        }
    }
   
    thread_sched(TRUE);
    preemption(); //!< 进行调度
    // if (slist_in_list(&wait.node))
    // {
    //     slist_del(&wait.node);
    // }
    if (cur_th->ipc_status == THREAD_IPC_ABORT) {
        cur_th->ipc_status = THREAD_NONE;
        ret = -ESHUTDOWN;
    } else if (cur_th->ipc_status == THREAD_TIMEOUT) {
        cur_th->ipc_status = THREAD_NONE;
        ret = -ERTIMEDOUT;
    } else {
        cur_th->ipc_status = THREAD_NONE;
        *ret_msg = cur_th->msg.tag;
        *ret_user_id = cur_th->user_id;
    }
    cpulock_set(lock_status);

    return ret;
}
static int thread_ipc_reply_inner(thread_t *form, thread_t *to, msg_tag_t in_tag)
{
    if (to->status != THREAD_SUSPEND &&
        to->ipc_status != THREAD_RECV) {
        to->last_send_th = NULL;
        return -1;
    }
    //!< 发送数据给上一次的发送者
    int ret = ipc_data_copy(to, form, in_tag); //!< 拷贝数据

    if (ret < 0) {
        in_tag.prot = ret;
    }
    to->msg.tag = in_tag;
    if (to->ipc_status != THREAD_IPC_ABORT) {
        thread_ready_remote(to, TRUE); //!< 直接唤醒接受者
    }
    to->last_send_th = NULL;
    ref_counter_dec_and_release(&to->ref, &to->kobj);
    return 0;
}
#if IS_ENABLED(CONFIG_SMP)
static int thread_ipc_reply_handler(ipi_msg_t *msg)
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
    if (to->cpu == arch_get_current_cpu_id()) {
        return thread_ipc_reply_inner(form, to, in_tag);
    } else {
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

    if (cur_th->last_send_th == NULL) {
        return -1;
    }
    status = cpulock_lock();
    if (cur_th->last_send_th == NULL) {
        cpulock_set(status);
        return -1;
    }
    // printk("0x%x reply.\n", cur_th);

    last_th = cur_th->last_send_th;
    ret = thread_ipc_reply_inner(cur_th, last_th, in_tag);
    cpulock_set(status);
    return ret;
}
#if IS_ENABLED(CONFIG_SMP)
static int thread_remote_suspend_handler(ipi_msg_t *msg)
{
    thread_t *th = (thread_t *)msg->msg;
    assert(th);
    thread_suspend(th);
    return 0;
}
#endif
int thread_suspend_remote(thread_t *th)
{
#if IS_ENABLED(CONFIG_SMP)
    if (th->cpu != arch_get_current_cpu_id()) {
        th->ipi_msg_node.msg = (umword_t)th;
        th->ipi_msg_node.cb = thread_remote_suspend_handler;
        cpu_ipi_to_msg(1 << th->cpu, &th->ipi_msg_node, IPI_CALL);
    } else {
        thread_suspend(th);
    }
#else
    thread_suspend(th);
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
    thread_t *cur_th = thread_get_current();
    mword_t lock_status;

again_check:
    lock_status = spinlock_lock(&recv_th->wait_send_head_lock);
    if (recv_th->status != THREAD_SUSPEND && recv_th->ipc_status != THREAD_RECV) {

        // TODO:这里有互斥问题，recv_th可能recv的线程不是同一个核，这里就会出现互斥问题。
        thread_wait_entry_t wait;

        assert(slist_in_list(&cur_th->sche.node));
        assert(cur_th->cpu == arch_get_current_cpu_id());
        cur_th->ipc_status = THREAD_RECV; //!< 因为接收挂起
        cur_th->status = THREAD_SUSPEND;
        scheduler_del(&cur_th->sche);

        thread_wait_entry_init(&wait, cur_th, timeout.send_timeout);

        slist_add_append(&recv_th->wait_send_head, &wait.node); //!< 放到线程的等待队列中
        slist_add_append(pre_cpu_get_current_cpu_var(&wait_send_queue), &wait.node_timeout);
        spinlock_set(&recv_th->wait_send_head_lock, lock_status);

        thread_sched(TRUE);
        preemption();
        if (cur_th->ipc_status == THREAD_IPC_ABORT) {
            cur_th->ipc_status = THREAD_NONE;
            ret = -ESHUTDOWN;
            goto end;
        } else if (cur_th->ipc_status == THREAD_TIMEOUT) {
            ret = -EWTIMEDOUT;
            goto end;
        }
        cur_th->ipc_status = THREAD_NONE;
        goto again_check;
    } else {
        spinlock_set(&recv_th->wait_send_head_lock, lock_status);
    }
end:
    return ret;
}
__attribute__((optimize(0))) int thread_ipc_call(thread_t *to_th, msg_tag_t in_tag, msg_tag_t *ret_tag,
                                                 ipc_timeout_t timout, umword_t *ret_user_id, bool_t is_call)
{
    int ret = -EINVAL;
    if (is_call) {
        assert(is_call && ret_tag);
    }
    thread_t *cur_th = thread_get_current();
    thread_t *recv_kobj = to_th;
    mword_t lock_stats = spinlock_lock(&to_th->kobj.lock);

    if (lock_stats < 0) {
        //!< 锁已经无效了
        return -EACCES;
    }
    // printk("0x%x call.\n", cur_th);
    ret = thread_wait_recv_thread(recv_kobj, timout);
    if (ret < 0) {
        goto end;
    }
    // 下面开始数据接收的流程
    if (recv_kobj->status == THREAD_SUSPEND && recv_kobj->ipc_status == THREAD_RECV) {
        //!< 开始发送数据
        ret = ipc_data_copy(recv_kobj, cur_th, in_tag); //!< 拷贝数据
        if (ret < 0) {
            //!< 拷贝失败
            goto end;
        }

        if (is_call) {
            if (recv_kobj->ipc_kobj) {
                // 绑定回复的ipc到当前的线程
                assert(ipc_bind(((ipc_t *)(recv_kobj->ipc_kobj)), -1, 0, cur_th) >= 0);
                ref_counter_dec_and_release(&((ipc_t *)(recv_kobj->ipc_kobj))->ref,
                                            recv_kobj->ipc_kobj);
                recv_kobj->ipc_kobj = NULL;
                recv_kobj->last_send_th = NULL;
            } else {
                ref_counter_inc(&cur_th->ref);    //!< 作为发送者增加一次引用
                recv_kobj->last_send_th = cur_th; //!< 设置接收者的上一次发送者是谁
            }

            ret = thread_ipc_recv(ret_tag, timout, ret_user_id, NULL, recv_kobj); //!< 当前线程进行接收
            if (ret < 0) {
                //!< 接收超时
                goto end;
            }
        } else {
            thread_timeout_del_recv_remote(recv_kobj);
        }
        preemption();
    } else {
        assert(0);
    }
    ret = 0;
end:
    spinlock_set(&to_th->kobj.lock, lock_stats);
    return ret;
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

    switch (ipc_type) {
    case IPC_CALL: {
        msg_tag_t in_tag = msg_tag_init(f->regs[0]);
        msg_tag_t recv_tag;
        th_hd = f->regs[2];
        ipc_timeout_t ipc_tm_out = ipc_timeout_create(f->regs[3]);

        to_th->user_id = user_id;
        ret = thread_ipc_call(to_th, in_tag, &recv_tag, ipc_tm_out, &f->regs[1], TRUE);
        if (ret < 0) {
            return msg_tag_init4(0, 0, 0, ret);
        }
        return recv_tag;
    }
    case IPC_REPLY: {
        msg_tag_t in_tag = msg_tag_init(f->regs[0]);

        ret = thread_ipc_reply(in_tag);
        return msg_tag_init4(0, 0, 0, ret);
    }
    case IPC_RECV:
    case IPC_WAIT: {
        msg_tag_t ret_msg;
        ipc_timeout_t ipc_tm_out = ipc_timeout_create(f->regs[3]);
        kobject_t *ipc_kobj = obj_space_lookup_kobj_cmp_type(&cur_task->obj_space, f->regs[4], IPC_TYPE);

        int ret = thread_ipc_recv(&ret_msg, ipc_tm_out, &f->regs[1], (ipc_t *)ipc_kobj, NULL);
        if (ret < 0) {
            return msg_tag_init4(0, 0, 0, ret);
        }
        return ret_msg;
    }
    case IPC_SEND: {
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
static int thread_remote_migration(ipi_msg_t *msg)
{
    thread_t *th = (thread_t *)msg->msg;
    umword_t tagcpu = msg->msg2;
    int cur_cpu = arch_get_current_cpu_id();
    assert(th);
    assert(tagcpu == cur_cpu);
    assert(th->cpu != cur_cpu);

    // 等待切换完成
    while (atomic_read(&th->time_count) == msg->th_time_cn)
        ;
    assert(!slist_in_list(&th->sche.node));
    assert(th->cpu != cur_cpu);

    assert(th->status == THREAD_SUSPEND || th->status == THREAD_IDLE);
    assert(th->magic == THREAD_MAGIC);
    th->cpu = cur_cpu;
    // 在新的核上调度
    thread_ready(th, TRUE);
    return 0;
}

#endif
static void thread_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    task_t *task = thread_get_current_task();
    thread_t *tag_th = container_of(kobj, thread_t, kobj);
    thread_t *cur_th = thread_get_current();

    if (sys_p.prot != THREAD_PROT) {
        f->regs[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }

    switch (sys_p.op) {
    case SET_EXEC_REGS: {
        umword_t stack_bottom = 0;

        if (f->regs[4]) //!< cp_stack
        {
            stack_bottom = (umword_t)(thread_get_msg_buf(cur_th));
        }
        thread_set_exc_regs(tag_th, f->regs[1], f->regs[2], f->regs[3], stack_bottom);
        tag = msg_tag_init4(0, 0, 0, 0);
    } break;
    case MSG_BUG_SET: {
        task_t *tag_tk = thread_get_bind_task(tag_th);
        if (is_rw_access(tag_tk, (void *)(f->regs[1]), THREAD_MSG_BUG_LEN, FALSE)) {
#if IS_ENABLED(CONFIG_MMU)
            thread_set_msg_buf(tag_th, (void *)mm_get_paddr(mm_space_get_pdir(&tag_tk->mm_space), f->regs[1], PAGE_SHIFT),
                               (void *)(f->regs[1]));
#else
            thread_set_msg_buf(tag_th, (void *)(f->regs[1]),
                               (void *)(f->regs[1]));
#endif
            tag = msg_tag_init4(0, 0, 0, 0);
        } else {
            //!< 内存不可访问
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        }
    }
    case MSG_BUG_GET: {
        f->regs[1] = (umword_t)(thread_get_msg_buf(tag_th));
        f->regs[2] = THREAD_MSG_BUG_LEN;
        if (thread_get_msg_buf(tag_th) == NULL) {
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        } else {
            tag = msg_tag_init4(0, 0, 0, 0);
        }
    } break;
    case RUN_THREAD: {
        task_t *tag_tsk = thread_get_bind_task(tag_th);
        if (tag_tsk == NULL) {
            tag = msg_tag_init4(0, 0, 0, -EACCES);
            break;
        }
        if (task_pid_get(tag_tsk) == -1) {
            //!< 只有设置了pid才能启动，pid只有init进程能够设置，这就使得只有pid能够启动应用程序
            tag = msg_tag_init4(0, 0, 0, -EACCES);
            break;
        }
        int cur_cpu = arch_get_current_cpu_id();
        umword_t status = cpulock_lock();

#if IS_ENABLED(CONFIG_SMP)
        if (f->regs[2] >= CONFIG_CPU) {
            f->regs[2] = cur_cpu;
        }

        if (tag_th->cpu != cur_cpu) {
            atomic_set(&tag_th->ipi_msg_node.flags, 0);

            // 目标cpu和线程当前cpu不在同一个cpu上，则需要先让目标cpu吧线程挂起，然后再进行迁移
            thread_suspend_remote(tag_th);
        } else {
            if (slist_in_list(&tag_th->sche.node)) {
                // 当前核先挂起线程
                // thread_suspend(tag_th);
                tag_th->status = THREAD_SUSPEND;
                scheduler_del(&tag_th->sche);
            }
        }
        tag_th->sche.prio = (f->regs[1] >= PRIO_MAX ? PRIO_MAX - 1 : f->regs[1]);

        if (f->regs[2] != tag_th->cpu) {
            // 目标核运行该线程
            tag_th->ipi_msg_node.msg = (umword_t)tag_th;
            tag_th->ipi_msg_node.msg2 = f->regs[2];
            tag_th->ipi_msg_node.th_time_cn = tag_th->status == THREAD_IDLE ? 1 : atomic_read(&tag_th->time_count);
            tag_th->ipi_msg_node.cb = thread_remote_migration;
            cpu_ipi_to_msg(1 << f->regs[2], &tag_th->ipi_msg_node, IPI_CALL);
            assert(tag_th->cpu == f->regs[2]);
        } else {
            if (tag_th->cpu == cur_cpu) {
                thread_ready(tag_th, TRUE);
            } else {
                assert(0);
            }
        }
#else
        if (!slist_in_list(&tag_th->sche.node)) {
            thread_ready(tag_th, TRUE);
        } else {
            thread_suspend(tag_th);
            tag_th->sche.prio = (f->regs[1] >= PRIO_MAX ? PRIO_MAX - 1 : f->regs[1]);
            thread_ready(tag_th, TRUE);
        }
#endif
        cpulock_set(status);
        tag = msg_tag_init4(0, 0, 0, 0);
    } break;
    case BIND_TASK: {
        kobject_t *task_kobj = obj_space_lookup_kobj_cmp_type(&task->obj_space, f->regs[1], TASK_TYPE);
        if (task_kobj == NULL) {
            f->regs[0] = msg_tag_init4(0, 0, 0, -ENOENT).raw;
            return;
        }
        thread_bind(tag_th, task_kobj);
        tag = msg_tag_init4(0, 0, 0, 0);
        // printk("thread bind to %d\n", f->regs[1]);
    } break;
    case YIELD: {
        thread_sched(TRUE);
        // arch_to_sche();
        tag = msg_tag_init4(0, 0, 0, 0);
    } break;
    case DO_IPC: {
        tag = thread_do_ipc(kobj, f, 0);
    } break;
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
static kobject_t *thread_create_func(ram_limit_t *lim, umword_t arg0, umword_t arg1,
                                     umword_t arg2, umword_t arg3)
{
    kobject_t *kobj = (kobject_t *)thread_create(lim);
    if (!kobj) {
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

    if (!kobj) {
        return NULL;
    }
    return container_of(
        kobj, task_t, kobj);
}
task_t *thread_get_task(thread_t *th)
{
    return container_of(
        th->task, task_t, kobj);
}
