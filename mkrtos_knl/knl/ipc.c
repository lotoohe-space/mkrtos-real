#include "ipc.h"
#include "types.h"
#include "init.h"
#include "prot.h"
#include "kobject.h"
#include "factory.h"
#include "thread.h"
#include "assert.h"

#define IPC_MSG_LEN 64

typedef struct ipc
{
    kobject_t kobj;
    thread_t *srv;
    thread_t *cli;
    uint8_t data[IPC_MSG_LEN];
    uint8_t flags; //!< 1：服务端发送 2：客户端发送 3：
} ipc_t;

enum ipc_op
{
    IPC_BIND_SRV,   //!< 绑定服务端
    IPC_BIND_CLI,   //!< 绑定客户端
    IPC_UNBIND_SRV, //!< 解绑服务端
    IPC_UNBIND_CLI, //!< 解绑客户端
    IPC_SEND,       //!< 发送IPC消息
    IPC_REVC,       //!< 接受IPC消息
};

static msg_tag_t
ipc_syscall(kobject_t *kobj, ram_limit_t *ram, entry_frame_t *f)
{
    assert(kobj);
    assert(ram);
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
    case IPC_BIND_SRV:
    {
        if (ipc->srv == NULL)
        {
            ipc->srv = th;
        }
        else
        {
            return msg_tag_init3(0, 0, -EACCES);
        }
    }
    break;
    case IPC_BIND_CLI:
    {
        if (ipc->cli == NULL)
        {
            ipc->cli = th;
        }
        else
        {
            return msg_tag_init3(0, 0, -EACCES);
        }
    }
    break;
    case IPC_UNBIND_SRV:
    {
        if (ipc->srv == th)
        {
            ipc->srv = NULL;
        }
        else
        {
            return msg_tag_init3(0, 0, -EACCES);
        }
    }
    break;
    case IPC_UNBIND_CLI:
    {
        if (ipc->cli == th)
        {
            ipc->cli = NULL;
        }
        else
        {
            return msg_tag_init3(0, 0, -EACCES);
        }
    }
    break;
    case IPC_SEND:
    {
        if (ipc->cli != th && ipc->srv != th)
        {
            return msg_tag_init3(0, 0, -EACCES);
        }
        thread_t *recv_th = NULL;

        if (ipc->cli == th)
        {
            recv_th = ipc->srv;
        }
        else if (ipc->srv == th)
        {
            recv_th = ipc->cli;
        }
        enum thread_state st = thread_get_status(recv_th);

        if (st == THREAD_SUSPEND)
        {
        }
        else
        {
            // 接收者没有再等待状态，当前线程挂起
        }
    }
    break;
    case IPC_REVC:
    {
    }
    break;
    }
    return msg_tag_init3(0, 0, -ENOSYS);
}
static void ipc_init(ipc_t *ipc)
{
    kobject_init(&ipc->kobj);
    ipc->kobj.invoke_func = ipc_syscall;
}

static kobject_t *ipc_create_func(ram_limit_t *lim, umword_t arg0, umword_t arg1,
                                  umword_t arg2, umword_t arg3)
{

    return NULL;
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
