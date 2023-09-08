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
    spinlock_t lock;        //!< 操作的锁TODO: 使用内核对象锁
    slist_head_t wait_send; //!< 发送等待队列
    thread_t *svr_th;       //!< 服务端 TODO:增加引用计数
    thread_t *last_cli_th;  //!< 上一次发送数据的客户端TODO:增加引用计数
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
};

static void add_wait_unlock(slist_head_t *wait_send, thread_t *th)
{
    slist_add_append(wait_send, &th->wait);
    thread_suspend(th);
}
static void wake_up_th(ipc_t *ipc)
{
    slist_head_t *mslist = slist_first(&ipc->wait_send);
    thread_t *th = container_of(mslist, thread_t, wait);

    slist_del(mslist);
    thread_ready(th, TRUE);
}

static int ipc_data_copy(thread_t *dst_th, thread_t *src_th, msg_tag_t tag)
{
    void *src = dst_th->msg.msg;
    void *dst = src_th->msg.msg;
    ipc_msg_t *src_ipc;
    ipc_msg_t *dst_ipc;

    src_ipc = src;
    dst_ipc = dst;

    if (tag.map_buf_len > 0)
    {
        int map_len = tag.map_buf_len;
        task_t *src_tk = thread_get_bind_task(src_th);
        task_t *dst_tk = thread_get_bind_task(dst_th);
        for (int i = 0; i < map_len; i++)
        {
            int ret = obj_map_src_dst(&dst_tk->obj_space, &src_tk->obj_space,
                                      dst_ipc->map_buf[i], src_ipc->map_buf[i], dst_tk->lim);

            if (ret < 0)
            {
                return ret;
            }
        }
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
static int ipc_call(ipc_t *ipc, thread_t *th, entry_frame_t *f, msg_tag_t tag)
{
    umword_t status;
    int ret = -1;

    assert(th != ipc->svr_th);
__check:
    status = spinlock_lock(&ipc->lock);
    if (ipc->svr_th->status != THREAD_SUSPEND)
    {
        add_wait_unlock(&ipc->wait_send, th);
        spinlock_set(&ipc->lock, status);
        goto __check;
    }
    //!< 发送数据给svr_th
    ret = ipc_data_copy(th, ipc->svr_th, tag); //!< 拷贝数据
    if (ret < 0)
    {
        //!< 拷贝失败
        spinlock_set(&ipc->lock, status);
        return ret;
    }
    ipc->svr_th->msg.len = tag.msg_buf_len;
    thread_ready(ipc->svr_th, TRUE); //!< 直接唤醒接受者
    thread_suspend(th);              //!< 发送后客户端直接进入等待状态
    ipc->last_cli_th = th;           //!< 设置上一次发送的客户端
    spinlock_set(&ipc->lock, status);
    ipc->last_cli_th = NULL;
    return th->msg.len;
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
    //!< 发送数据给svr_th
    ipc_data_copy(ipc->last_cli_th, th, tag); //!< 拷贝数据
    ipc->last_cli_th->msg.len = tag.msg_buf_len;
    thread_ready(ipc->last_cli_th, TRUE); //!< 直接唤醒接受者
    spinlock_set(&ipc->lock, status);
    return 0;
}

/**
 * @brief 服务端用于接收数据
 *
 * @param ipc
 * @param th
 * @param f
 * @param tag
 */
static void ipc_wait(ipc_t *ipc, thread_t *th, entry_frame_t *f, msg_tag_t tag)
{
    assert(ipc->svr_th == th);
    umword_t status;

    status = spinlock_lock(&ipc->lock);
    thread_suspend(th);
    if (!slist_is_empty(&ipc->wait_send))
    {
        wake_up_th(ipc);
    }
    spinlock_set(&ipc->lock, status);
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
            tag = msg_tag_init4(0, 0, 0, ipc_call(ipc, th, f, in_tag));
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
            ipc_wait(ipc, th, f, in_tag);
            tag = msg_tag_init4(0, 0, 0, 0);
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
            int ret = ipc_reply(ipc, th, f, in_tag);
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
            ipc->svr_th = container_of(source_kobj, thread_t, kobj);
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
    if (ipc->svr_th)
    {
        ref_counter_dec_and_release(&ipc->svr_th->ref, &ipc->svr_th->kobj);
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
