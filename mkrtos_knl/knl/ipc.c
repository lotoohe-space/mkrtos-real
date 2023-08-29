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
} ipc_t;

enum ipc_op
{
    IPC_SEND, //!< 发送IPC消息
    IPC_REVC, //!< 接受IPC消息
};

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
        umword_t status;
        size_t send_len_c = -1;

        void *send_addr = th->msg.msg;
        th->msg.len = f->r[1];
        if (!ipc->rcv)
        {
            status = spinlock_lock(&ipc->lock);
            if (!ipc->rcv)
            {
                slist_add_append(&ipc->wait_send, &th->wait);
                thread_suspend(th);
            }
            spinlock_set(&ipc->lock, status);
            if (th->msg.flags & MSG_BUF_HAS_DATA_FLAGS)
            {
                th->msg.flags &= ~MSG_BUF_HAS_DATA_FLAGS;
                return msg_tag_init3(0, 0, th->msg.len);
            }
        }
    again_send:
        if (ipc->rcv->status != THREAD_SUSPEND)
        {
            if (ipc->rcv->status == THREAD_DEAD)
            {
                return msg_tag_init3(0, 0, -EACCES);
            }
            status = spinlock_lock(&ipc->lock);
            slist_add_append(&ipc->wait_send, &th->wait);
            thread_suspend(th);
            spinlock_set(&ipc->lock, status);
            goto again_send;
        }
        else
        {
            status = spinlock_lock(&ipc->lock);
            if (ipc->rcv->status == THREAD_SUSPEND)
            {
                send_len_c = MIN(THREAD_MSG_BUG_LEN, th->msg.len);

                // 接收线程正在等待中
                memcpy(ipc->rcv->msg.msg, th->msg.msg, send_len_c); //!< 拷贝数据
                ipc->rcv->msg.flags |= MSG_BUF_HAS_DATA_FLAGS;
                ipc->rcv->msg.len = send_len_c;
                thread_ready(ipc->rcv, TRUE); //!< 直接唤醒接受者
                ipc->rcv = NULL;
            }
            spinlock_set(&ipc->lock, status);
        }
        return msg_tag_init3(0, 0, send_len_c);
    }
    break;
    case IPC_REVC:
    {
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
                return msg_tag_init3(0, 0, -EAGAIN);
            }
            spinlock_set(&ipc->lock, status);
        }
        void *recv_addr = th->msg.msg;
        if (0)
        {
            return msg_tag_init3(0, 0, -EINVAL);
        }
    again_recv:
        // 没有数据则睡眠
        while (slist_is_empty(&ipc->wait_send))
        {
            thread_suspend(th);
            if (th->msg.flags & MSG_BUF_HAS_DATA_FLAGS)
            {
                th->msg.flags &= ~MSG_BUF_HAS_DATA_FLAGS;
                // 已经有数据了，直接返
                return msg_tag_init3(0, 0, th->msg.len);
            }
        }

        size_t recv_len_c = 0;
        umword_t status = spinlock_lock(&ipc->lock);
        if (slist_is_empty(&ipc->wait_send))
        {
            spinlock_set(&ipc->lock, status);
            goto again_recv;
        }
        slist_head_t *mslist = slist_first(&ipc->wait_send);
        slist_del(mslist);
        thread_t *send_th = container_of(mslist, thread_t, wait);
        recv_len_c = MIN(THREAD_MSG_BUG_LEN, send_th->msg.len);
        memcpy(recv_addr, send_th->msg.msg, recv_len_c); //!< 拷贝数据
        ipc->rcv = NULL;
        send_th->msg.flags |= MSG_BUF_HAS_DATA_FLAGS;
        send_th->msg.len = recv_len_c;
        spinlock_set(&ipc->lock, status);
        thread_ready(send_th, TRUE); //!< 唤醒发送线程
        return msg_tag_init3(0, 0, recv_len_c);
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
