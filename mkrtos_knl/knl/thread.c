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
#include "sleep.h"
#include "thread_knl.h"
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
    SLEEP,
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
#if 0
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
#endif

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
    assert(th);
    assert(th->com);
    assert(lim);
    kobject_init(&th->kobj, THREAD_TYPE);
    sched_init(&th->sche);
    slist_init(&th->futex_node);
    slist_init(&th->release_node);
#if 0
    slist_init(&th->wait_send_head);
    spinlock_init(&th->recv_lock);
    spinlock_init(&th->send_lock);
#endif
    ref_counter_init(&th->ref);
    ref_counter_inc(&th->ref);
    thread_arch_init(th, flags);

    kobject_set_name(&th->kobj, kobject_get_name(&th->kobj));
    slist_init(&th->com->fast_ipc_node);
    stack_init(&th->com->fast_ipc_stack, &th->com->fast_ipc_stack_data,
               ARRARY_LEN(th->com->fast_ipc_stack_data),
               sizeof(th->com->fast_ipc_stack_data[0]));
    th->com->th = th;

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
    if (stack_len(&th->com->fast_ipc_stack) == 0)
    {
        // 处于ipc通信中，不能直接删除，否者会立刻中断ipc中的执行
        if (th->status == THREAD_READY)
        {
            thread_suspend(th);
        }

        thread_sleep_del(th); //!< 从休眠中删除
        thread_unbind(th);
        th->ipc_status = THREAD_IPC_ABORT;
    }
    else
    {
        th->ipc_status = THREAD_IPC_ABORT;
    }
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

        ref_counter_dec_and_release(&tsk->ref_cn, &tsk->kobj);
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
        //!< 线程没有发生变化，则不用切换
        cpulock_set(status);
        return FALSE;
    }
    if (is_sche)
    {
        // 立刻进行切换
        arch_to_sche();
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
    assert(th);
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
    }
    cpulock_set(status);
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
    memset(th, 0, CONFIG_THREAD_BLOCK_SIZE);
    th->com = mm_limit_alloc(ram, sizeof(*th->com));
    if (!th->com)
    {
        mm_limit_free(ram, th);
        return NULL;
    }
    // assert(((mword_t)th & (~(CONFIG_THREAD_BLOCK_SIZE - 1))) == 0);
    memset(th->com, 0, sizeof(*th->com));
    thread_init(th, ram, flags);
    printk("create thread 0x%x\n", th);
    return th;
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
 *  快速ipc call
 * FIXME:以下代码是arch相关的，需要整理
 **/
msg_tag_t thread_fast_ipc_call(task_t *to_task, entry_frame_t *f, umword_t user_id)
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
    // 回收ipc线程注意事项：
    // 1.ipc的线程在与其它进程通信时在其它进程中死亡（这个最常见）
    // 2.ipc的线程在其它进程通信时其它进程死亡（需要吧其它进程的ipc线程给还原回去）
    // 3.ipc的线程在与其他的进程通信时，ipc线程的原进程死亡（需要推迟ipc线程的删除到ipc操作完成，否者可能发生未知的错误）

    if (to_task->notify_point == NULL)
    {
        printk("task:0x%x, notify point is not set.\n", to_task);
        return msg_tag_init4(0, 0, 0, -EIO);
    }
    sema_down(&to_task->notify_sema, 0);
    mutex_lock(&to_task->nofity_lock, 0);
    umword_t cpu_status = cpulock_lock();
    assert(cur_th->magic == THREAD_MAGIC);

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
            dst_ipc->user[2] = task_pid_get(cur_task);                            // 设置pid
            slist_add(&to_task->nofity_theads_head, &cur_th->com->fast_ipc_node); // 添加到链表中，用于进程关闭时进行释放
            pf_s_t *usr_stask_point = (void *)arch_get_user_sp();

            if (thread_is_knl(cur_th))
            {
                // 如果是内核线程则全部重新设置
                thread_set_user_pf_noset_knl_sp(cur_th, to_task->notify_point,
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
                usr_stask_point->pc = (umword_t)(to_task->notify_point) | 0x1;

                //! 获得内核栈栈顶
                pf_t *cur_pf = ((pf_t *)((char *)cur_th + CONFIG_THREAD_BLOCK_SIZE + 8)) - 1;
                // 重新设置r9寄存器
                cur_pf->regs[5] = (umword_t)(to_task->mm_space.mm_block);
                cur_th->sp.user_sp = cur_pf;

                //! 寄存器传参数
                f->regs[0] = in_tag.raw;
                f->regs[1] = user_id;
                f->regs[2] = f->regs[2];
                f->regs[3] = f->regs[3];
            }
            // 切换mpu
            ref_counter_inc(&cur_th->ref);
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
            cpulock_set(cpu_status);
            mutex_unlock(&to_task->nofity_lock);
            sema_up(&to_task->notify_sema);
        }
    }
    else
    {
        ref_counter_dec_and_release(&to_task->ref_cn, &to_task->kobj);
        cpulock_set(cpu_status);
        mutex_unlock(&to_task->nofity_lock);
        sema_up(&to_task->notify_sema);
    }
   
    return msg_tag_init4(0, 0, 0, ret);
}
msg_tag_t thread_fast_ipc_replay(entry_frame_t *f)
{
    task_t *cur_task = thread_get_current_task();
    thread_t *cur_th = thread_get_current();
    msg_tag_t in_tag = msg_tag_init(f->regs[0]);
    task_t *old_task = thread_get_bind_task(cur_th);
    int ret;

    assert(cur_th->magic == THREAD_MAGIC);
    *(cur_task->nofity_bitmap) &= ~(1 << MIN(f->regs[2], cur_task->nofity_bitmap_len)); //!< 解锁bitmap
    slist_del(&cur_th->com->fast_ipc_node);                                             // 从链表中删除

    ret = thread_fast_ipc_restore(cur_th); // 还原栈和usp
    if (ret < 0)
    {
        mutex_unlock(&old_task->nofity_lock);
        in_tag = msg_tag_init4(0, 0, 0, ret);
        goto end;
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
        // 映射了多少个，则重新填充多少个新的
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
    sema_up(&old_task->notify_sema);
    if (thread_is_knl(cur_th))
    {
        arch_to_sche();
    }
end:
    if (thread_get_ipc_state(cur_th) == THREAD_IPC_ABORT && stack_len(&cur_th->com->fast_ipc_stack) == 0)
    {
        // 原进程死亡，ipc结束的时候直接回收线程内存
        cpu_status = cpulock_lock();
        thread_unbind(cur_th);
        thread_suspend(cur_th);
        thread_knl_release_helper(cur_th);
        cpulock_set(cpu_status);
    }
    else
    {
        ref_counter_dec_and_release(&cur_th->ref, &cur_th->kobj);
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
    task_t *to_tk = (task_t *)kobj;
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
        return thread_fast_ipc_call(to_tk, f, user_id);
    }
    break;
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
int thread_set_prio(thread_t *th, int prio)
{
    assert(cpulock_get_status());
    int old_prio = thread_get_prio(th);

    if (old_prio != prio)
    {
        if (thread_get_status(th) == THREAD_READY)
        {
            thread_suspend(th);
            th->sche.prio = prio;
            thread_ready(th, TRUE);
        }
        else
        {
            th->sche.prio = prio;
        }
    }
    return old_prio;
}
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
        goto run_thread_end;
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
        tag = thread_do_ipc(&thread_get_task(container_of(kobj, thread_t, kobj))->kobj, f, 0);
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
    case SLEEP:
    {
        int ret;
        umword_t status;

        status = cpulock_lock();
        ret = thread_sleep(f->regs[0]);
        cpulock_set(status);
        tag = msg_tag_init4(0, 0, 0, ret);
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
