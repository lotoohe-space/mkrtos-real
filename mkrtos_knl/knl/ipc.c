/**
 * @file ipc.c
 * @author zhangzheng (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "ipc.h"
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
#include "string.h"
#include "mm_wrap.h"
#include "map.h"
struct ipc;
typedef struct ipc ipc_t;
typedef struct ipc_wait_item
{
    slist_head_t node;
    thread_t *th;
    umword_t sleep_times;
} ipc_wait_item_t;
/**
 * @brief ipc 对象，用于接收与发送另一个thread发送的消息
 *
 */
typedef struct ipc
{
    kobject_t kobj;         //!< 内核对象
    spinlock_t lock;        //!< 操作的锁
    slist_head_t wait_send; //!< 发送等待队列
    slist_head_t recv_send; //!< 接收等待队列
    slist_head_t node;      //!< 超时检查链表
    thread_t *svr_th;       //!< 服务端
    thread_t *last_cli_th;  //!< 上一次发送数据的客户端
    ram_limit_t *lim;       //!< 内存限额
    umword_t user_id;       //!< 服务端绑定的数据
} ipc_t;

enum ipc_op
{
    IPC_CALL,   //!< 客户端CALL操作
    IPC_WAIT,   //!< 服务端等待接收信息
    IPC_REPLY,  //!< 服务端回复信息
    IPC_BIND,   //!< 绑定服务端线程
    IPC_UNBIND, //!< 解除绑定
    IPC_SEND,   //!<
};
static int wake_up_th(ipc_t *ipc);
static slist_head_t wait_list;

/**
 * @brief 初始化一个超时等待队列
 *
 */
static void timeout_wait_list_init(void)
{
    slist_init(&wait_list);
}
INIT_KOBJ(timeout_wait_list_init);

/**
 * @brief 检查超时队列
 *
 */
void timeout_times_tick(void)
{
    ipc_t *ipc;

    slist_foreach(ipc, &wait_list, node) //!< 第一次循环等待的ipc
    {
        umword_t status = spinlock_lock(&ipc->lock);
        ipc_wait_item_t *item;

        slist_foreach(item, &ipc->wait_send, node) //!< 第二次循环等待irq里面的等待者
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
                //!< 超时时间满后直接唤醒等待者
                if (item->th->status == THREAD_SUSPEND)
                {
                    thread_ready(item->th, TRUE);
                }
            }
        }
        spinlock_set(&ipc->lock, status);
    }
}
/**
 * @brief 唤醒某个ipc的所有等待者
 *
 * @param ipc
 */
static void timeout_times_wake_ipc(ipc_t *ipc)
{
    assert(ipc);
    ipc_wait_item_t *item;

    slist_foreach(item, &ipc->wait_send, node) //!< 第二次循环等待irq里面的等待者
    {
        //!< 超时时间满后直接唤醒等待者
        if (thread_get_status(item->th) == THREAD_SUSPEND)
        {
            thread_todead(item->th, TRUE);
        }
    }
    slist_foreach(item, &ipc->recv_send, node) //!< 第二次循环等待irq里面的等待者
    {
        //!< 超时时间满后直接唤醒等待者
        if (thread_get_status(item->th) == THREAD_SUSPEND)
        {
            thread_todead(item->th, TRUE);
        }
    }
    thread_sched();
    preemption();
}
/**
 * @brief ipc_wait_item_t结构体初始化
 *
 * @param item
 * @param ipc
 * @param th
 * @param times
 */
static void ipc_wait_item_init(ipc_wait_item_t *item, ipc_t *ipc, thread_t *th, umword_t times)
{
    slist_init(&item->node);
    item->th = th;
    item->sleep_times = times;
}
/**
 * @brief 添加到一个等待队列，并进行解锁
 *
 * @param ipc
 * @param head
 * @param th
 * @param times 超时时间，为0代表一直超时
 * @return int
 */
static int add_wait_unlock(ipc_t *ipc, slist_head_t *head, thread_t *th, umword_t times, bool_t set_last_th)
{
    int ret = 0;
    ipc_wait_item_t item;

    ipc_wait_item_init(&item, ipc, th, times);
    if (times)
    {
        if (slist_is_empty(head))
        {
            slist_add_append(&wait_list, &ipc->node);
        }
    }
    slist_add_append(head, &item.node);
    thread_suspend(th);
    preemption();
    slist_del(&item.node);
    // spinlock_set(lock, status);
    if (times)
    {
        if (slist_is_empty(head))
        {
            slist_del(&ipc->node);
        }
        if (item.sleep_times == 0)
        {
            ret = -ETIMEDOUT;
        }
    }
    if (thread_get_status(th) == THREAD_TODEAD)
    {
        ret = -ESHUTDOWN;
    }
    return ret;
}
/**
 * @brief 拿出等待队列中的第一个并唤醒
 *
 * @param ipc
 */
static int wake_up_th(ipc_t *ipc)
{
    slist_head_t *mslist = slist_first(&ipc->wait_send);
    ipc_wait_item_t *item = container_of(mslist, ipc_wait_item_t, node);

    if (thread_get_status(item->th) == THREAD_TODEAD)
    {
        return -ESHUTDOWN;
    }
    else
    {
        if (item->th->status == THREAD_SUSPEND)
        {
            thread_ready(item->th, TRUE);
        }
    }
    return 0;
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

    src_ipc = src;
    dst_ipc = dst;

    if (tag.map_buf_len > 0)
    {
        kobj_del_list_t del;
        int map_len = tag.map_buf_len;

        kobj_del_list_init(&del);
        task_t *src_tk = thread_get_bind_task(src_th);
        task_t *dst_tk = thread_get_bind_task(dst_th);
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
    memcpy(dst_ipc->msg_buf, src_ipc->msg_buf, MIN(tag.msg_buf_len * WORD_BYTES, IPC_MSG_SIZE));
    return 0;
}
/**
 * @brief 客户端发送并接收数据
 *
 * @param ipc
 * @param th
 * @param f
 * @param tag
 */
static msg_tag_t ipc_call(ipc_t *ipc, thread_t *th, entry_frame_t *f, msg_tag_t tag, ipc_timeout_t timeout)
{
    umword_t status;
    int ret = -1;
    msg_tag_t tmp_tag;

    assert(th != ipc->svr_th);
    status = spinlock_lock(&ipc->lock);
__check:
    if (ipc->svr_th == NULL || ipc->svr_th->status != THREAD_SUSPEND)
    {
        ret = add_wait_unlock(ipc, &ipc->wait_send, th,
                              timeout.send_timeout, FALSE);
        if (ret < 0)
        {
            spinlock_set(&ipc->lock, status);
            return msg_tag_init4(MSG_TAG_KNL_ERR, 0, 0, ret);
        }
        goto __check;
    }
    //!< 发送数据给svr_th
    ret = ipc_data_copy(ipc->svr_th, th, tag); //!< 拷贝数据
    if (ret < 0)
    {
        //!< 拷贝失败
        spinlock_set(&ipc->lock, status);
        return msg_tag_init4(MSG_TAG_KNL_ERR, 0, 0, ret);
    }
    ipc->svr_th->msg.tag = tag;
    thread_ready(ipc->svr_th, TRUE); //!< 直接唤醒接受者
    ipc->last_cli_th = th;           //!< 设置上一次发送的客户端
    ret = add_wait_unlock(ipc, &ipc->recv_send, th, timeout.recv_timeout, TRUE);
    if (ret < 0)
    {
        // ref_counter_dec_and_release(&ipc->last_cli_th->ref, &ipc->last_cli_th->kobj);
        ipc->last_cli_th = NULL;
        spinlock_set(&ipc->lock, status);
        return msg_tag_init4(MSG_TAG_KNL_ERR, 0, 0, ret);
    }
    tmp_tag = th->msg.tag;
    // ipc->last_cli_th = NULL;
    spinlock_set(&ipc->lock, status);
    return tmp_tag;
}
/**
 * @brief ipc发送操作
 *
 * @param ipc
 * @param th
 * @param f
 * @param tag
 * @param timeout
 * @return msg_tag_t
 */
static msg_tag_t ipc_send(ipc_t *ipc, thread_t *th, entry_frame_t *f, msg_tag_t tag, ipc_timeout_t timeout)
{
    umword_t status;
    int ret = -1;
    msg_tag_t tmp_tag;

    assert(th != ipc->svr_th);
    status = spinlock_lock(&ipc->lock);
__check:
    if (ipc->svr_th == NULL || ipc->svr_th->status != THREAD_SUSPEND)
    {
        ret = add_wait_unlock(ipc, &ipc->wait_send, th,
                              timeout.send_timeout, FALSE);
        if (ret < 0)
        {
            spinlock_set(&ipc->lock, status);
            return msg_tag_init4(MSG_TAG_KNL_ERR, 0, 0, ret);
        }
        goto __check;
    }
    //!< 发送数据给svr_th
    ret = ipc_data_copy(ipc->svr_th, th, tag); //!< 拷贝数据
    if (ret < 0)
    {
        //!< 拷贝失败
        spinlock_set(&ipc->lock, status);
        return msg_tag_init4(MSG_TAG_KNL_ERR, 0, 0, ret);
    }
    ipc->svr_th->msg.tag = tag;
    thread_ready(ipc->svr_th, TRUE); //!< 直接唤醒接受者
    ipc->last_cli_th = NULL;
    spinlock_set(&ipc->lock, status);
    return tmp_tag;
}
/**
 * @brief 服务端用于回复
 *
 * @param ipc
 * @param th
 * @param f
 * @param tag
 */
static int ipc_reply(ipc_t *ipc, thread_t *th, entry_frame_t *f, msg_tag_t tag)
{
    umword_t status;
    if (ipc->last_cli_th == NULL)
    {
        return -1;
    }
    assert(th == ipc->svr_th); // 服务端才能回复
    status = spinlock_lock(&ipc->lock);
    if (ipc->last_cli_th == NULL)
    {
        spinlock_set(&ipc->lock, status);
        return -1;
    }
    //!< 发送数据给svr_th
    int ret = ipc_data_copy(ipc->last_cli_th, th, tag); //!< 拷贝数据

    if (ret < 0)
    {
        spinlock_set(&ipc->lock, status);
        return ret;
    }
    ipc->last_cli_th->msg.tag = tag;
    if (thread_get_status(ipc->last_cli_th) != THREAD_TODEAD)
    {
        thread_ready(ipc->last_cli_th, TRUE); //!< 直接唤醒接受者
    }
    else
    {
        ret = -EAGAIN;
    }
    spinlock_set(&ipc->lock, status);
    return ret;
}

/**
 * @brief 服务端用于接收数据
 *
 * @param ipc
 * @param th
 * @param f
 * @param tag
 */
static msg_tag_t ipc_wait(ipc_t *ipc, thread_t *th, entry_frame_t *f, msg_tag_t in_tag)
{
    assert(ipc->svr_th == th);
    umword_t status;
    msg_tag_t tag;

    status = spinlock_lock(&ipc->lock);
    thread_suspend(th);
    if (!slist_is_empty(&ipc->wait_send))
    {
        int ret = wake_up_th(ipc);

        if (-ESHUTDOWN == ret)
        {
            thread_ready(th, TRUE);
            tag = msg_tag_init4(0, 0, 0, -EAGAIN);
        }
        else
        {
            preemption();
            tag = th->msg.tag;
        }
    }
    else
    {
        preemption();
        tag = th->msg.tag;
    }
    spinlock_set(&ipc->lock, status);
    return tag;
}
/**
 * @brief ipc的系统调用
 *
 * @param kobj
 * @param ram
 * @param f
 * @return msg_tag_t
 */
static void ipc_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    assert(kobj);
    assert(f);
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    thread_t *th = thread_get_current();
    ipc_t *ipc = container_of(kobj, ipc_t, kobj);

    if (sys_p.prot != IPC_PROT)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case IPC_CALL:
    {
        if (th == ipc->svr_th)
        {
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        }
        else
        {
            ref_counter_inc(&th->ref);                                       //!< 引用计数+1
            tag = ipc_call(ipc, th, f, in_tag, ipc_timeout_create(f->r[1])); //!< ipc call
            ref_counter_dec_and_release(&th->ref, &th->kobj);                //!< 引用计数-1
            if (msg_tag_get_val(tag) == -ESHUTDOWN)
            {
                thread_dead(th);
            }
        }
    }
    break;
    case IPC_SEND:
    {
        if (th == ipc->svr_th)
        {
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        }
        else
        {
            ref_counter_inc(&th->ref);                                       //!< 引用计数+1
            tag = ipc_send(ipc, th, f, in_tag, ipc_timeout_create(f->r[1])); //!< ipc call
            ref_counter_dec_and_release(&th->ref, &th->kobj);                //!< 引用计数-1
            if (msg_tag_get_val(tag) == -ESHUTDOWN)
            {
                thread_dead(th);
            }
        }
    }
    break;
    case IPC_WAIT:
    {
        if (ipc->svr_th != th) //!< 只有服务端才能wait
        {
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        }
        else
        {
            ref_counter_inc(&th->ref);                        //!< 引用计数+1
            tag = ipc_wait(ipc, th, f, in_tag);               //!< 进入等待
            ref_counter_dec_and_release(&th->ref, &th->kobj); //!< 引用计数-1
            if (msg_tag_get_val(tag) == -ESHUTDOWN)
            {
                thread_dead(th);
            }
            f->r[1] = ipc->user_id;
        }
    }
    break;
    case IPC_REPLY:
    {
        if (ipc->svr_th != th) //!< 只有服务端才能回复
        {
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        }
        else
        {
            ref_counter_inc(&th->ref); //!< 引用计数+1
            int ret = ipc_reply(ipc, th, f, in_tag);
            ref_counter_dec_and_release(&th->ref, &th->kobj); //!< 引用计数-1
            if (msg_tag_get_val(tag) == -ESHUTDOWN)
            {
                thread_dead(th);
            }
            tag = msg_tag_init4(0, 0, 0, ret);
        }
    }
    break;
    case IPC_BIND:
    {
        if (!ipc->svr_th)
        {
            kobject_t *source_kobj = obj_space_lookup_kobj(&thread_get_current_task()->obj_space, f->r[1]);

            if (!source_kobj)
            {
                tag = msg_tag_init4(0, 0, 0, -ENOENT);
                break;
            }
            thread_t *srv_th = container_of(source_kobj, thread_t, kobj);

            ref_counter_inc(&srv_th->ref);
            ipc->svr_th = srv_th;
            ipc->user_id = f->r[2];
            tag = msg_tag_init4(0, 0, 0, 0);
        }
        else
        {
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        }
    }
    break;
    case IPC_UNBIND:
    {
        if (ipc->svr_th == th)
        {
            ref_counter_dec_and_release(&th->ref, &th->kobj); //!< 引用计数-1
            ipc->svr_th = NULL;
            tag = msg_tag_init4(0, 0, 0, 0);
        }
        else
        {
            tag = msg_tag_init4(0, 0, 0, -EACCES);
        }
    }
    break;
    }
    f->r[0] = tag.raw;
}
static void ipc_release_stage1(kobject_t *kobj)
{
    ipc_t *ipc = container_of(kobj, ipc_t, kobj);

    kobject_invalidate(kobj);
    timeout_times_wake_ipc(ipc);
    if (ipc->svr_th)
    {
        ref_counter_dec_and_release(&ipc->svr_th->ref, &ipc->svr_th->kobj);
    }
}
static void ipc_release_stage2(kobject_t *kobj)
{
    ipc_t *ipc = container_of(kobj, ipc_t, kobj);

    mm_limit_free(ipc->lim, kobj);
    printk("ipc release 0x%x\n", kobj);
}
static void ipc_init(ipc_t *ipc, ram_limit_t *lim)
{
    kobject_init(&ipc->kobj, IPC_TYPE);
    slist_init(&ipc->wait_send);
    slist_init(&ipc->recv_send);
    slist_init(&ipc->node);
    spinlock_init(&ipc->lock);
    ipc->lim = lim;
    ipc->svr_th = NULL;
    ipc->kobj.invoke_func = ipc_syscall;
    ipc->kobj.stage_1_func = ipc_release_stage1;
    ipc->kobj.stage_2_func = ipc_release_stage2;
}
static ipc_t *ipc_create(ram_limit_t *lim)
{
    ipc_t *ipc = mm_limit_alloc(lim, sizeof(ipc_t));

    if (!ipc)
    {
        return NULL;
    }
    ipc_init(ipc, lim);
    return ipc;
}
static kobject_t *ipc_create_func(ram_limit_t *lim, umword_t arg0, umword_t arg1,
                                  umword_t arg2, umword_t arg3)
{

    return &ipc_create(lim)->kobj;
}

/**
 * @brief 工厂注册函数
 *
 */
static void ipc_factory_register(void)
{
    factory_register(ipc_create_func, IPC_PROT);
}
INIT_KOBJ(ipc_factory_register);

void ipc_dump(void)
{
}
