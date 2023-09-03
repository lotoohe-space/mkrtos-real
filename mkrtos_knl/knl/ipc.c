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
/**
 * @brief ipc 对象，用于接收与发送另一个thread发送的消息
 *
 */
typedef struct ipc
{
    kobject_t kobj;         //!< 内核对象
    spinlock_t lock;        //!< 操作的锁
    slist_head_t wait_send; //!< 发送等待队列
    thread_t *rcv;          //!< 只有一个接受者
    ram_limit_t *lim;
} ipc_t;

enum ipc_op
{
    IPC_SEND, //!< 发送IPC消息
    IPC_REVC, //!< 接受IPC消息
    IPC_MAP,  //!< 该消息是一个映射消息
};

static void add_wait(ipc_t *ipc, thread_t *th)
{
    umword_t status;
    status = spinlock_lock(&ipc->lock);
    slist_add_append(&ipc->wait_send, &th->wait);
    thread_suspend(th);
    spinlock_set(&ipc->lock, status);
}
static void add_wait_unlock(ipc_t *ipc, thread_t *th)
{
    slist_add_append(&ipc->wait_send, &th->wait);
    thread_suspend(th);
}

static bool_t ipc_bind_rcv(ipc_t *ipc, thread_t *th)
{
    if (!ipc->rcv)
    {
        ipc->rcv = th;
        ref_counter_inc(&th->ref);
        return TRUE;
    }
    return FALSE;
}
static void ipc_unbind_rcv(ipc_t *ipc)
{
    if (ipc->rcv)
    {
        ref_counter_dec_and_release(&ipc->rcv->ref, &ipc->kobj);
        ipc->rcv = 0;
    }
}
static void ipc_data_copy(thread_t *dst_th, thread_t *src_th, int len, ipc_type_t ipc_type)
{
    void *src = dst_th->msg.msg;
    void *dst = src_th->msg.msg;
    if (ipc_type.type)
    {
        ipc_msg_t *src_ipc;
        ipc_msg_t *dst_ipc;

        src_ipc = src;
        dst_ipc = dst;

        if (ipc_type.map_buf_len > 0)
        {
            int map_len = ipc_type.map_buf_len;
            task_t *src_tk = thread_get_bind_task(src_th);
            task_t *dst_tk = thread_get_bind_task(dst_th);
            for (int i = 0; i < map_len; i++)
            {
                obj_map_src_dst(&dst_tk->obj_space, &src_tk->obj_space,
                                dst_ipc->map_buf[i], src_ipc->map_buf[i], dst_tk->lim);
            }
        }
        memcpy(dst_ipc->msg_buf, src_ipc->msg_buf, MIN(ipc_type.msg_buf_len, IPC_MSG_SIZE));
    }
    else
    {
        memcpy(src, dst, len);
    }
}

static int ipc_send(ipc_t *ipc, entry_frame_t *f, msg_tag_t tag)
{
    umword_t status;
    size_t send_len_c = -1;
    umword_t send_flag = f->r[2];
    ipc_type_t ipc_ide = ipc_type_create(tag.type2);
    thread_t *th = thread_get_current();
    void *send_addr = th->msg.msg;

    th->msg.len = f->r[1];
    th->msg.ipc_ide = ipc_ide.raw;
again_send:
    if (!ipc->rcv)
    {
        status = spinlock_lock(&ipc->lock);
        if (!ipc->rcv)
        {
            add_wait_unlock(ipc, th);
        }
        spinlock_set(&ipc->lock, status);
        if (th->msg.flags & MSG_BUF_HAS_DATA_FLAGS)
        {
            th->msg.flags &= ~MSG_BUF_HAS_DATA_FLAGS;
            return th->msg.len;
        }
    }
    assert(ipc->rcv);
    if (ipc->rcv->status != THREAD_SUSPEND)
    {
        if (ipc->rcv->status == THREAD_DEAD)
        {
            return -EACCES;
        }
        status = spinlock_lock(&ipc->lock);
        add_wait_unlock(ipc, th);
        spinlock_set(&ipc->lock, status);
        if (th->msg.flags & MSG_BUF_HAS_DATA_FLAGS)
        {
            th->msg.flags &= ~MSG_BUF_HAS_DATA_FLAGS;
            return th->msg.len;
        }
        goto again_send;
    }
    else
    {
        status = spinlock_lock(&ipc->lock);
        if (
            ((send_flag & MSG_BUF_REPLY_FLAGS) == 0 || ((ipc->rcv->msg.flags & MSG_BUF_RECV_R_FLAGS) && ipc->rcv == th->msg.recv_th)))
        {
            if (ipc->rcv->status == THREAD_SUSPEND)
            {
                send_len_c = MIN(THREAD_MSG_BUG_LEN, th->msg.len);

                // 接收线程正在等待中，直接复制数据到目标缓存中，然后唤醒目标线程
                // memcpy(ipc->rcv->msg.msg, th->msg.msg, send_len_c);
                ipc_data_copy(ipc->rcv, th, send_len_c, ipc_type_create(tag.type2)); //!< 拷贝数据
                ipc->rcv->msg.flags |= MSG_BUF_HAS_DATA_FLAGS;
                ipc->rcv->msg.len = send_len_c;
                thread_ready(ipc->rcv, TRUE); //!< 直接唤醒接受者
                ipc->rcv->msg.send_th = th;   //!< 设置接收者的消息来源
                ipc_unbind_rcv(ipc);
                th->msg.recv_th = NULL;
            }
            else
            {
                assert(0);
            }
            spinlock_set(&ipc->lock, status);
        }
        else
        {
            thread_sched();
            spinlock_set(&ipc->lock, status);
            goto again_send;
        }
        return send_len_c;
    }
}
static int ipc_recv(ipc_t *ipc, entry_frame_t *f, msg_tag_t tag)
{
    umword_t recv_flags = f->r[1];
    thread_t *th = thread_get_current();

again_recv:
    if (ipc->rcv != th)
    {
        umword_t status = spinlock_lock(&ipc->lock);
        if (!ipc_bind_rcv(ipc, th))
        {
            thread_sched(); // TODO:
            spinlock_set(&ipc->lock, status);
            goto again_recv;
        }
        spinlock_set(&ipc->lock, status);
    }
    th->msg.flags |= (recv_flags & MSG_BUF_RECV_R_FLAGS);
    void *recv_addr = th->msg.msg;
    // 没有数据则睡眠
    while (slist_is_empty(&ipc->wait_send))
    {
        thread_suspend(th);
        if (th->msg.flags & MSG_BUF_HAS_DATA_FLAGS)
        {
            th->msg.flags &= ~MSG_BUF_HAS_DATA_FLAGS;
            // 已经有数据了，直接返
            return th->msg.len;
        }
    }

    size_t recv_len_c = 0;
    if (slist_is_empty(&ipc->wait_send))
    {
        goto again_recv;
    }
    umword_t status = spinlock_lock(&ipc->lock);
    /*************************/
    thread_t *send_th = NULL;
    int find = 0;
    if (th->msg.flags & MSG_BUF_RECV_R_FLAGS)
    {
        slist_foreach(send_th, &ipc->wait_send, wait)
        {
            if (th == send_th->msg.send_th || th->msg.recv_th == send_th)
            {
                find = 1;
                send_th->msg.send_th = NULL;
                break;
            }
        }
        if (find == 0)
        {
            thread_sched(); // TODO:应该挂起，并等待唤醒
            spinlock_set(&ipc->lock, status);
            goto again_recv;
        }
        slist_del(&send_th->wait);
        th->msg.flags &= ~MSG_BUF_RECV_R_FLAGS;
    }
    else
    {
        slist_head_t *mslist = slist_first(&ipc->wait_send);
        slist_del(mslist);
        send_th = container_of(mslist, thread_t, wait);
    }

    recv_len_c = MIN(THREAD_MSG_BUG_LEN, send_th->msg.len);
    // memcpy(recv_addr, send_th->msg.msg, recv_len_c);
    ipc_data_copy(th, send_th, recv_len_c, ipc_type_create(send_th->msg.ipc_ide)); //!< 拷贝数据
    send_th->msg.flags |= MSG_BUF_HAS_DATA_FLAGS;
    send_th->msg.len = recv_len_c;
    send_th->msg.recv_th = th; //!< 设置消息发送给了谁
    ipc_unbind_rcv(ipc);
    thread_ready(send_th, TRUE); //!< 唤醒发送线程
    spinlock_set(&ipc->lock, status);
    return recv_len_c;
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
    msg_tag_t tag = msg_tag_init3(0, 0, -EINVAL);
    thread_t *th = thread_get_current();
    ipc_t *ipc = container_of(kobj, ipc_t, kobj);

    if (sys_p.prot != IPC_PROT)
    {
        f->r[0] = msg_tag_init3(0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case IPC_SEND:
    {
        tag = msg_tag_init3(0, 0, ipc_send(ipc, f, in_tag));
    }
    break;
    case IPC_REVC:
    {
        tag = msg_tag_init3(0, 0, ipc_recv(ipc, f, in_tag));
    }
    break;
    }
    f->r[0] = tag.raw;
}
static void ipc_release_stage1(kobject_t *kobj)
{
    ipc_t *ipc = container_of(kobj, ipc_t, kobj);

    kobject_invalidate(kobj);
    if (ipc->rcv)
    {
        ref_counter_dec_and_release(&ipc->rcv->ref, &ipc->rcv->kobj);
    }
}
static void ipc_release_stage2(kobject_t *kobj)
{
    ipc_t *ipc = container_of(kobj, ipc_t, kobj);

    mm_limit_free(ipc->lim, kobj);
}
static void ipc_init(ipc_t *ipc, ram_limit_t *lim)
{
    kobject_init(&ipc->kobj);
    slist_init(&ipc->wait_send);
    spinlock_init(&ipc->lock);
    ipc->lim = lim;
    ipc->rcv = NULL;
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
