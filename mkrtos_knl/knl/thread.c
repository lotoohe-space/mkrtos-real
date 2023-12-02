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

#include "types.h"
#include "kobject.h"
#include "ram_limit.h"
#include "factory.h"
#include "thread.h"
#include "mm_wrap.h"
#include "string.h"
#include "init.h"
#include "task.h"
#include "thread.h"
#include "slist.h"
#include "thread_armv7m.h"
#include "assert.h"
#include "err.h"
#include "map.h"
#include "access.h"

enum thread_op
{
    SET_EXEC_REGS,
    RUN_THREAD,
    BIND_TASK,
    MSG_BUG_GET,
    MSG_BUG_SET,
    YIELD,
    DO_IPC = 6, //!< 与ipc对象中的额IPC_DO一致
};
enum IPC_TYPE
{
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

typedef struct thread_wait_entry
{
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

static slist_head_t wait_send_queue;
static slist_head_t wait_recv_queue;

static void thread_timeout_init(void)
{
    slist_init(&wait_send_queue);
    slist_init(&wait_recv_queue);
}

INIT_KOBJ(thread_timeout_init);

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

    if (cur != th)
    {
        //! 线程在运行中，则挂起线程
        if (th->status == THREAD_READY)
        {
            thread_suspend(th);
        }
    }
    else
    {
        if (cur->status == THREAD_READY)
        {
            thread_suspend(th);
        }
    }
    thread_wait_entry_t *pos;

    slist_foreach_not_next(pos, &wait_send_queue, node_timeout)
    {
        assert(pos->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(pos, &wait_send_queue, node_timeout);

        pos->th->ipc_status = THREAD_IPC_ABORT;
        thread_ready(pos->th, TRUE);

        slist_del(&pos->node_timeout);
        if (slist_in_list(&pos->node))
        {
            slist_del(&pos->node);
        }
        pos = next;
    }
    thread_wait_entry_t *pos2;

    slist_foreach_not_next(pos2, &wait_recv_queue, node)
    {
        assert(pos2->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(pos2, &wait_recv_queue, node);

        pos2->th->ipc_status = THREAD_IPC_ABORT;
        thread_ready(pos2->th, TRUE);

        slist_del(&pos2->node);
        pos2 = next;
    }

    thread_unbind(th);
}
static void thread_release_stage2(kobject_t *kobj)
{
    thread_t *th = container_of(kobj, thread_t, kobj);
    thread_t *cur_th = thread_get_current();
    printk("release thread 0x%x\n", kobj);
    mm_limit_free_align(th->lim, kobj, THREAD_BLOCK_SIZE);

    if (cur_th == th)
    {
        scheduler_reset();
        thread_sched();
    }
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
    if (th->task)
    {
        task_t *tsk = container_of(th->task, task_t, kobj);

        ref_counter_dec_and_release(&tsk->ref_cn, &th->kobj);
        th->task = NULL;
    }
}
/**
 * @brief 挂起一个线程
 *
 * @param th
 */
void thread_suspend(thread_t *th)
{
    // if (!slist_in_list(&th->sche.node))
    // {
    assert(slist_in_list(&th->sche.node));
    // }
    scheduler_del(&th->sche);
    th->status = THREAD_SUSPEND;
    thread_sched();
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
    thread_sched();
}

/**
 * @brief 进行一次调度
 *
 * @param th
 */
void thread_sched(void)
{
    umword_t status = cpulock_lock();
    sched_t *next_sche = scheduler_next();
    thread_t *th = thread_get_current();

    assert(th->magic == THREAD_MAGIC);
    if (next_sche == &th->sche)
    {
        cpulock_set(status);

        return;
    }
    to_sche();
    cpulock_set(status);
}
/**
 * @brief 线程进入就绪态
 *
 * @param th
 */
void thread_ready(thread_t *th, bool_t is_sche)
{
    umword_t status = cpulock_lock();

    // if (!!slist_in_list(&th->sche.node))
    // {
    assert(!slist_in_list(&th->sche.node));
    // }
    scheduler_add(&th->sche);
    th->status = THREAD_READY;
    if (is_sche)
    {
        thread_sched();
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
    if (is_sche)
    {
        thread_sched();
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
    thread_t *th = mm_limit_alloc_align(ram, THREAD_BLOCK_SIZE, THREAD_BLOCK_SIZE);

    if (!th)
    {
        return NULL;
    }
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

    slist_foreach_not_next(pos, &wait_send_queue, node_timeout)
    {
        assert(pos->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(pos, &wait_send_queue, node_timeout);
        if (pos->times > 0)
        {
            pos->times -= tick;
            if (pos->times <= 0)
            {
                pos->th->ipc_status = THREAD_TIMEOUT;
                thread_ready(pos->th, TRUE);

                slist_del(&pos->node_timeout);
                if (slist_in_list(&pos->node))
                {
                    slist_del(&pos->node);
                }
            }
        }
        pos = next;
    }
    thread_wait_entry_t *pos2;
    slist_foreach_not_next(pos2, &wait_recv_queue, node)
    {
        assert(pos2->th->status == THREAD_SUSPEND);
        thread_wait_entry_t *next = slist_next_entry(pos2, &wait_recv_queue, node);

        if (pos2->times > 0)
        {
            pos2->times -= tick;
            if (pos2->times <= 0)
            {
                pos2->th->ipc_status = THREAD_TIMEOUT;
                thread_ready(pos2->th, TRUE);

                slist_del(&pos2->node);
            }
        }
        pos2 = next;
    }
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
    void *src = src_th->msg.msg;
    void *dst = dst_th->msg.msg;
    ipc_msg_t *src_ipc;
    ipc_msg_t *dst_ipc;
    task_t *src_tk = thread_get_bind_task(src_th);
    task_t *dst_tk = thread_get_bind_task(dst_th);

    src_ipc = src;
    dst_ipc = dst;

    if (tag.map_buf_len > 0)
    {
        kobj_del_list_t del;
        int map_len = tag.map_buf_len;

        kobj_del_list_init(&del);

        for (int i = 0; i < map_len; i++)
        {
            int ret = 0;

            vpage_t dst_page = vpage_create_raw(dst_ipc->map_buf[i]);
            vpage_t src_page = vpage_create_raw(src_ipc->map_buf[i]);

            if (src_page.flags & VPAGE_FLAGS_MAP)
            {
                ret = obj_map_src_dst(&dst_tk->obj_space, &src_tk->obj_space,
                                      vpage_get_obj_handler(dst_page),
                                      vpage_get_obj_handler(src_page),
                                      dst_tk->lim,
                                      vpage_get_attrs(src_page), &del);
            }

            if (ret < 0)
            {
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

/**
 * @brief 当前线程接收数据
 *
 * @return int
 */
static int thread_ipc_recv(msg_tag_t *ret_msg, ipc_timeout_t timeout, umword_t *ret_user_id)
{
    int ret = 0;
    assert(ret_msg);
    assert(ret_user_id);
    thread_t *cur_th = thread_get_current();
    umword_t lock_status;
    thread_wait_entry_t wait;

    lock_status = cpulock_lock();
    cur_th->ipc_status = THREAD_RECV; //!< 因为接收挂起
    if (!slist_is_empty(&cur_th->wait_send_head))
    {
        //!< 有发送者
        slist_head_t *mslist = slist_first(&cur_th->wait_send_head);
        thread_wait_entry_t *wait = container_of(mslist, thread_wait_entry_t, node);

        slist_del(mslist); //!< 删除唤醒的线程
        if (slist_in_list(&wait->node_timeout))
        {
            slist_del(&wait->node_timeout);
        }
        thread_ready(wait->th, TRUE);
    }
    else
    {
        //!< 加入等待队列
        if (timeout.recv_timeout)
        {

            thread_wait_entry_init(&wait, cur_th, timeout.recv_timeout);
            slist_add_append(&wait_recv_queue, &wait.node); //!< 放到等待队列中
        }
    }
    thread_suspend(cur_th); //!< 挂起
    preemption();           //!< 进行调度
    if (cur_th->ipc_status == THREAD_IPC_ABORT)
    {
        cur_th->ipc_status = THREAD_NONE;
        ret = -ESHUTDOWN;
    }
    else if (cur_th->ipc_status == THREAD_TIMEOUT)
    {
        cur_th->ipc_status = THREAD_NONE;
        ret = -ERTIMEDOUT;
    }
    else
    {
        cur_th->ipc_status = THREAD_NONE;
        *ret_msg = cur_th->msg.tag;
        *ret_user_id = cur_th->user_id;
    }
    cpulock_set(lock_status);

    return ret;
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

    if (cur_th->last_send_th == NULL)
    {
        return -1;
    }
    status = cpulock_lock();
    if (cur_th->last_send_th == NULL)
    {
        cpulock_set(status);
        return -1;
    }
    assert(cur_th->last_send_th->status == THREAD_SUSPEND);
    assert(cur_th->last_send_th->ipc_status == THREAD_RECV);
    //!< 发送数据给上一次的发送者
    int ret = ipc_data_copy(cur_th->last_send_th, cur_th, in_tag); //!< 拷贝数据

    if (ret < 0)
    {
        cpulock_set(status);
        return ret;
    }
    cur_th->last_send_th->msg.tag = in_tag;
    thread_ready(cur_th->last_send_th, TRUE); //!< 直接唤醒接受者
    ref_counter_dec_and_release(&cur_th->last_send_th->ref, &cur_th->last_send_th->kobj);
    cur_th->last_send_th = NULL;
    cpulock_set(status);
    return ret;
}
static int thread_ipc_send(thread_t *to_th, msg_tag_t in_tag, ipc_timeout_t timout)
{
    int ret = -EINVAL;
    thread_t *cur_th = thread_get_current();
    thread_t *recv_kobj = to_th;
    mword_t lock_stats = spinlock_lock(&cur_th->kobj.lock);

    if (lock_stats < 0)
    {
        //!< 锁已经无效了
        return -EACCES;
    }
again_check:
    if (recv_kobj->status == THREAD_READY)
    {
        thread_wait_entry_t wait;

        cur_th->ipc_status = THREAD_SEND; //!< 因为发送挂起

        thread_wait_entry_init(&wait, cur_th, timout.send_timeout);
        slist_add_append(&recv_kobj->wait_send_head, &wait.node); //!< 放到线程的等待队列中
        slist_add_append(&wait_send_queue, &wait.node_timeout);
        thread_suspend(cur_th); //!< 挂起
        preemption();           //!< 进行调度
        if (cur_th->ipc_status == THREAD_IPC_ABORT)
        {
            cur_th->ipc_status = THREAD_NONE;
            ret = -ESHUTDOWN;
            goto end;
        }
        else if (cur_th->ipc_status == THREAD_TIMEOUT)
        {
            ret = -EWTIMEDOUT;
            goto end;
        }
        cur_th->ipc_status = THREAD_NONE;
        goto again_check;
    }
    else if (recv_kobj->status == THREAD_SUSPEND && recv_kobj->ipc_status == THREAD_RECV)
    {
        // if (slist_in_list(&recv_kobj->wait_node))
        // {
        //     //!< 如果已经在队列中，则删除
        //     slist_del(&recv_kobj->wait_node);
        // }
        //!< 开始发送数据
        ret = ipc_data_copy(recv_kobj, cur_th, in_tag); //!< 拷贝数据
        if (ret < 0)
        {
            //!< 拷贝失败
            goto end;
        }
        recv_kobj->last_send_th = cur_th; //!< 设置接收者的上一次发送者是谁
        ref_counter_inc(&cur_th->ref);    //!< 作为发送者增加一次引用
        thread_ready(recv_kobj, TRUE);    //!< 直接唤醒接受者
        preemption();                     //!< 进行调度
    }
    ret = 0;
end:
    spinlock_set(&cur_th->kobj.lock, lock_stats);
}
static int thread_ipc_call(thread_t *to_th, msg_tag_t in_tag, msg_tag_t *ret_tag, ipc_timeout_t timout, umword_t *ret_user_id)
{
    assert(ret_tag);
    int ret = -EINVAL;
    thread_t *cur_th = thread_get_current();
    thread_t *recv_kobj = to_th;
    mword_t lock_stats = spinlock_lock(&cur_th->kobj.lock);

    if (lock_stats < 0)
    {
        //!< 锁已经无效了
        return -EACCES;
    }
again_check:
    if (recv_kobj->status == THREAD_READY)
    {
        thread_wait_entry_t wait;

        cur_th->ipc_status = THREAD_SEND; //!< 因为发送挂起

        thread_wait_entry_init(&wait, cur_th, timout.send_timeout);
        slist_add_append(&recv_kobj->wait_send_head, &wait.node); //!< 放到线程的等待队列中
        slist_add_append(&wait_send_queue, &wait.node_timeout);
        thread_suspend(cur_th); //!< 挂起
        preemption();
        if (cur_th->ipc_status == THREAD_IPC_ABORT)
        {
            cur_th->ipc_status = THREAD_NONE;
            ret = -ESHUTDOWN;
            goto end;
        }
        else if (cur_th->ipc_status == THREAD_TIMEOUT)
        {
            ret = -EWTIMEDOUT;
            goto end;
        }
        cur_th->ipc_status = THREAD_NONE;
        goto again_check;
    }
    else if (recv_kobj->status == THREAD_SUSPEND && recv_kobj->ipc_status == THREAD_RECV)
    {
        // if (slist_in_list(&recv_kobj->wait_node))
        // {
        //     //!< 如果已经在队列中，则删除
        //     slist_del(&recv_kobj->wait_node);
        // }
        //!< 开始发送数据
        ret = ipc_data_copy(recv_kobj, cur_th, in_tag); //!< 拷贝数据
        if (ret < 0)
        {
            //!< 拷贝失败
            goto end;
        }
        recv_kobj->last_send_th = cur_th;                    //!< 设置接收者的上一次发送者是谁
        ref_counter_inc(&cur_th->ref);                       //!< 作为发送者增加一次引用
        thread_ready(recv_kobj, TRUE);                       //!< 直接唤醒接受者
        ret = thread_ipc_recv(ret_tag, timout, ret_user_id); //!< 当前线程进行接收
        if (ret < 0)
        {
            //!< 接收超时
            goto end;
        }
        preemption(); //!< 进行调度
    }
    ret = 0;
end:
    spinlock_set(&cur_th->kobj.lock, lock_stats);
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
    umword_t ipc_type = f->r[1];
    obj_handler_t th_hd = 0;
    int ret = -EINVAL;

    switch (ipc_type)
    {
    case IPC_CALL:
    {
        msg_tag_t in_tag = msg_tag_init(f->r[0]);
        msg_tag_t recv_tag;
        th_hd = f->r[2];
        ipc_timeout_t ipc_tm_out = ipc_timeout_create(f->r[3]);

        to_th->user_id = user_id;
        ret = thread_ipc_call(to_th, in_tag, &recv_tag, ipc_tm_out, &f->r[1]);
        if (ret < 0)
        {
            return msg_tag_init4(0, 0, 0, ret);
        }
        return recv_tag;
    }
    case IPC_REPLY:
    {
        msg_tag_t in_tag = msg_tag_init(f->r[0]);

        ret = thread_ipc_reply(in_tag);
        return msg_tag_init4(0, 0, 0, ret);
    }
    case IPC_RECV:
    case IPC_WAIT:
    {
        msg_tag_t ret_msg;
        ipc_timeout_t ipc_tm_out = ipc_timeout_create(f->r[3]);

        int ret = thread_ipc_recv(&ret_msg, ipc_tm_out, &f->r[1]);
        if (ret < 0)
        {
            return msg_tag_init4(0, 0, 0, ret);
        }
        return ret_msg;
    }
    case IPC_SEND:
    {
        msg_tag_t in_tag = msg_tag_init(f->r[0]);
        msg_tag_t recv_tag;
        th_hd = f->r[2];
        ipc_timeout_t ipc_tm_out = ipc_timeout_create(f->r[3]);

        to_th->user_id = user_id;
        ret = thread_ipc_call(to_th, in_tag, &recv_tag, ipc_tm_out, &f->r[1]);
        return msg_tag_init4(0, 0, 0, ret);
    }
    default:
        ret = -ENOSYS;
        break;
    }

    return msg_tag_init4(0, 0, 0, ret);
}
static void thread_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    task_t *task = thread_get_current_task();
    thread_t *tag_th = container_of(kobj, thread_t, kobj);
    thread_t *cur_th = thread_get_current();

    if (sys_p.prot != THREAD_PROT)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }

    switch (sys_p.op)
    {
    case SET_EXEC_REGS:
    {
        umword_t stack_bottom = 0;

        if (f->r[4]) //!< cp_stack
        {
            stack_bottom = (umword_t)(cur_th->msg.msg);
        }
        thread_set_exc_regs(tag_th, f->r[1], f->r[2], f->r[3], stack_bottom);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case MSG_BUG_SET:
    {
        if (is_rw_access(thread_get_bind_task(tag_th), (void *)(f->r[1]), THREAD_MSG_BUG_LEN, FALSE))
        {
            thread_set_msg_bug(tag_th, (void *)(f->r[1]));
            tag = msg_tag_init4(0, 0, 0, 0);
        }
        else
        {
            //!< 内存不可访问
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        }
    }
    case MSG_BUG_GET:
    {
        f->r[1] = (umword_t)(tag_th->msg.msg);
        f->r[2] = THREAD_MSG_BUG_LEN;
        if (tag_th->msg.msg == NULL)
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
        umword_t status = cpulock_lock();
        if (!slist_in_list(&tag_th->sche.node))
        {
            tag_th->sche.prio = (f->r[1] >= PRIO_MAX ? PRIO_MAX - 1 : f->r[1]);
            thread_ready(tag_th, TRUE);
        }
        cpulock_set(status);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case BIND_TASK:
    {
        kobject_t *task_kobj = obj_space_lookup_kobj_cmp_type(&task->obj_space, f->r[1], TASK_TYPE);
        if (task_kobj == NULL)
        {
            f->r[0] = msg_tag_init4(0, 0, 0, -ENOENT).raw;
            return;
        }
        thread_bind(tag_th, task_kobj);
        tag = msg_tag_init4(0, 0, 0, 0);
        printk("thread bind to %d\n", f->r[1]);
    }
    break;
    case YIELD:
    {
        thread_sched();
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case DO_IPC:
    {
        tag = thread_do_ipc(kobj, f, 0);
    }
    break;
    }
    f->r[0] = tag.raw;
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
    return container_of(
        kobj, task_t, kobj);
}
task_t *thread_get_task(thread_t *th)
{
    return container_of(
        th->task, task_t, kobj);
}
