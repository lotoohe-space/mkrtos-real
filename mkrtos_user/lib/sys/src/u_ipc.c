#include "u_types.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_util.h"
#include "u_arch.h"

enum ipc_op
{
    IPC_CALL,   //!< 客户端CALL操作
    IPC_WAIT,   //!< 服务端等待接收信息
    IPC_REPLY,  //!< 服务端回复信息
    IPC_BIND,   //!< 绑定服务端线程
    IPC_UNBIND, //!< 解除绑定
    IPC_SEND,   //!< 发送数据
};
msg_tag_t ipc_bind(obj_handler_t obj, obj_handler_t tag_th, umword_t user_obj)
{
    register volatile umword_t r0 asm("r0");

    mk_syscall(syscall_prot_create(IPC_BIND, IPC_PROT, obj).raw,
               0,
               tag_th,
               user_obj,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : "r0");
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t ipc_wait(obj_handler_t obj, umword_t *user_obj)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");

    mk_syscall(syscall_prot_create(IPC_WAIT, IPC_PROT, obj).raw,
               0,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : "r0", "r1");
    if (user_obj)
    {
        *user_obj = r1;
    }
    return msg_tag_init(r0);
}
msg_tag_t ipc_reply(obj_handler_t obj, msg_tag_t in_tag)
{
    register volatile umword_t r0 asm("r0");

    mk_syscall(syscall_prot_create(IPC_REPLY, IPC_PROT, obj).raw,
               in_tag.raw,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : "r0");
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t ipc_call(obj_handler_t obj, msg_tag_t in_tag, ipc_timeout_t timeout)
{
    register volatile umword_t r0 asm("r0");

    mk_syscall(syscall_prot_create(IPC_CALL, IPC_PROT, obj).raw,
               in_tag.raw,
               timeout.raw,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : "r0");
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t ipc_send(obj_handler_t obj, msg_tag_t in_tag, ipc_timeout_t timeout)
{
    register volatile umword_t r0 asm("r0");

    mk_syscall(syscall_prot_create(IPC_SEND, IPC_PROT, obj).raw,
               in_tag.raw,
               timeout.raw,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : "r0");
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
