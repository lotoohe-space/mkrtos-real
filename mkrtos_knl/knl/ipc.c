#include "ipc.h"
#include "types.h"
#include "init.h"
#include "prot.h"
#include "kobject.h"
#include "factory.h"
#include "thread.h"
#include "assert.h"
#include "slist.h"
#include "spinlock.h"
#include "string.h"
#include "mm_wrap.h"

/**
 * @brief ipc 对象，用于接收与发送另一个thread发送的消息
 *
 */
typedef struct ipc
{
    kobject_t kobj;         //!< 内核对象
    spinlock_t lock;        //!< 操作的锁
    slist_head_t wait_send; //!< 发送等待队列
    //!< 等待回复的链表
    //!< 发送消息后立刻进入挂起状态，并标记flags在等待中，这时rcv变量将不能改变，直到接受者reply后，清空。
    thread_t *rcv; //!< 只有一个接受者
    thread_t *call_send;
} ipc_t;

enum ipc_op
{
    IPC_SEND, //!< 发送IPC消息
    IPC_REVC, //!< 接受IPC消息
};

static bool_t is_call_send(ipc_t *ipc)
{
    if (ipc->rcv->msg.call_send)
    {
        return TRUE;
    }
    return FALSE;
}
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
// if (ipc->rcv->msg.call_send == th)
// {
// }
static int ipc_recv(ipc_t *ipc, entry_frame_t *f);

static int ipc_send(ipc_t *ipc, entry_frame_t *f)
{
    umword_t status;
    size_t send_len_c = -1;
    umword_t send_flag = f->r[2];
    thread_t *th = thread_get_current();

    void *send_addr = th->msg.msg;
    th->msg.len = f->r[1];
    if (send_flag & MSG_BUF_CALL_FLAGS)
    {
        th->msg.flags |= MSG_BUF_CALL_FLAGS;
        th->msg.call_send = NULL;
    }
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
            if (th->msg.flags & MSG_BUF_CALL_FLAGS)
            {
                /*如果是call则需要吧发送者转变为接收者*/
                /*TODO:call等待接收者回复消息*/
                int ret = ipc_recv(ipc, f);
                th->msg.flags &= ~MSG_BUF_CALL_FLAGS;
                return ret;
            }
            return th->msg.len;
        }
    }
again_send:
    assert(ipc->rcv);
    if (ipc->rcv->status != THREAD_SUSPEND)
    {
        if (ipc->rcv->status == THREAD_DEAD)
        {
            return -EACCES;
        }
        add_wait(ipc, th);
        if (th->msg.flags & MSG_BUF_HAS_DATA_FLAGS)
        {
            th->msg.flags &= ~MSG_BUF_HAS_DATA_FLAGS;
            if (th->msg.flags & MSG_BUF_CALL_FLAGS)
            {
                /*如果是call则需要吧发送者转变为接收者*/
                /*TODO:call等待接收者回复消息*/
                int ret = ipc_recv(ipc, f);
                th->msg.flags &= ~MSG_BUF_CALL_FLAGS;
                return ret;
            }
            return th->msg.len;
        }
        goto again_send;
    }
    else
    {
        status = spinlock_lock(&ipc->lock);
        if (ipc->rcv->msg.call_send == th || ipc->rcv->msg.call_send == NULL)
        {
            if (ipc->rcv->status == THREAD_SUSPEND)
            {
                send_len_c = MIN(THREAD_MSG_BUG_LEN, th->msg.len);

                // 接收线程正在等待中，直接复制数据到目标缓存中，然后唤醒目标线程
                memcpy(ipc->rcv->msg.msg, th->msg.msg, send_len_c); //!< 拷贝数据
                ipc->rcv->msg.flags |= MSG_BUF_HAS_DATA_FLAGS;
                if (th->msg.flags & MSG_BUF_CALL_FLAGS)
                {
                    // ipc->rcv->msg.flags |= MSG_BUF_CALL_FLAGS;
                    ipc->rcv->msg.call_send = th; //! 当前线程是call发起方
                }
                else
                {
                    ipc->rcv->msg.call_send = NULL;
                }
                ipc->rcv->msg.len = send_len_c;
                thread_ready(ipc->rcv, TRUE); //!< 直接唤醒接受者
                spinlock_set(&ipc->lock, status);
                if (th->msg.flags & MSG_BUF_CALL_FLAGS)
                {
                    ipc->rcv = th;
                    /*如果是call则需要吧发送者转变为接收者*/
                    int ret = ipc_recv(ipc, f);
                    th->msg.flags &= ~MSG_BUF_CALL_FLAGS;
                    send_len_c = ret;
                }
                else
                {
                    ipc->rcv->msg.call_send = NULL;
                    ipc->rcv = NULL; //! 如果不是call，则清空接收者
                }
            }
        }
        else if (ipc->rcv->msg.call_send != th && ipc->rcv->msg.call_send != NULL)
        {
            spinlock_set(&ipc->lock, status);
            add_wait(ipc, th);
            goto again_send;
        }
        else
        {
            spinlock_set(&ipc->lock, status);
        }
    }
    return send_len_c;
}
static int ipc_recv(ipc_t *ipc, entry_frame_t *f)
{
    thread_t *th = thread_get_current();

    if (!ipc->rcv)
    {
        umword_t status = spinlock_lock(&ipc->lock);
        if (!ipc->rcv)
        {
            ipc->rcv = th;
        }
        else
        {
            spinlock_set(&ipc->lock, status);
            return -EAGAIN;
        }
        spinlock_set(&ipc->lock, status);
    }
    void *recv_addr = th->msg.msg;
again_recv:
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
    umword_t status = spinlock_lock(&ipc->lock);
    if (slist_is_empty(&ipc->wait_send))
    {
        spinlock_set(&ipc->lock, status);
        goto again_recv;
    }
/*************************/
    thread_t *send_th = NULL;
    slist_foreach(send_th, &ipc->wait_send, wait)
    {
        if (send_th->msg.call_send == th)
        {
            break;
        }
    }
    if (send_th == NULL)
    {
        slist_head_t *mslist = slist_first(&ipc->wait_send); // TODO从链表中找到与th相同的
        slist_del(mslist);
        send_th = container_of(mslist, thread_t, wait);
    } else {
        slist_del(&send_th->wait);
    }
/*************************/
    // slist_head_t *mslist = slist_first(&ipc->wait_send); //TODO从链表中找到与th相同的
    // slist_del(mslist);
    // thread_t *send_th = container_of(mslist, thread_t, wait);
    recv_len_c = MIN(THREAD_MSG_BUG_LEN, send_th->msg.len);
    memcpy(recv_addr, send_th->msg.msg, recv_len_c); //!< 拷贝数据
    send_th->msg.flags |= MSG_BUF_HAS_DATA_FLAGS;
    send_th->msg.len = recv_len_c;
    if ((send_th->msg.flags & MSG_BUF_CALL_FLAGS))
    {
        th->msg.call_send = send_th; // 当前线程的callsend设置为发送方。
        ipc->rcv = send_th;          // 直接切换接收者，上下文切换之后发送者就变为接收者了
    }
    else
    {
        send_th->msg.call_send = NULL;
        ipc->rcv = NULL; //! 如果不是call，则清空接收者
    }
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
static msg_tag_t ipc_syscall(kobject_t *kobj, ram_limit_t *ram, entry_frame_t *f)
{
    assert(kobj);
    assert(f);
    msg_tag_t tag = msg_tag_init(f->r[0]);
    thread_t *th = thread_get_current();
    ipc_t *ipc = container_of(kobj, ipc_t, kobj);

    if (tag.prot != IPC_PROT)
    {
        return msg_tag_init3(0, 0, -EPROTO);
    }
    switch (tag.type)
    {
    case IPC_SEND:
    {
        return msg_tag_init3(0, 0, ipc_send(ipc, f));
    }
    break;
    case IPC_REVC:
    {
        return msg_tag_init3(0, 0, ipc_recv(ipc, f));
    }
    break;
    }
    return msg_tag_init3(0, 0, -ENOSYS);
}
static void ipc_init(ipc_t *ipc)
{
    kobject_init(&ipc->kobj);
    slist_init(&ipc->wait_send);
    spinlock_init(&ipc->lock);
    ipc->rcv = NULL;
    ipc->kobj.invoke_func = ipc_syscall;
}
static ipc_t *ipc_create(ram_limit_t *lim)
{
    ipc_t *ipc = mm_limit_alloc(lim, sizeof(ipc_t));

    if (!ipc)
    {
        return NULL;
    }
    ipc_init(ipc);
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
