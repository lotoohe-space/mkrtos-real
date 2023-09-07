#include "u_types.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_util.h"
enum ipc_op
{
    IPC_CALL,   //!< 客户端CALL操作
    IPC_WAIT,   //!< 服务端等待接收信息
    IPC_REPLY,  //!< 服务端回复信息
    IPC_BIND,   //!< 绑定服务端线程
    IPC_UNBIND, //!< 解除绑定
};
msg_tag_t ipc_bind(obj_handler_t obj, obj_handler_t tag_th, umword_t user_obj)
{
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(IPC_BIND, IPC_PROT, obj),
            0,
            tag_th,
            user_obj,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t ipc_wait(obj_handler_t obj, umword_t *user_obj)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");

    syscall(syscall_prot_create(IPC_WAIT, IPC_PROT, obj),
            0,
            0,
            0,
            0,
            0,
            0);
    if (user_obj)
    {
        *user_obj = r1;
    }
    return msg_tag_init(r0);
}
msg_tag_t ipc_reply(obj_handler_t obj, umword_t len)
{
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(IPC_REPLY, IPC_PROT, obj),
            msg_tag_init4(0, ROUND_UP(len, WORD_BYTES), 0, 0).raw,
            0,
            0,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t ipc_call(obj_handler_t obj, umword_t len)
{
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(IPC_CALL, IPC_PROT, obj),
            msg_tag_init4(0, ROUND_UP(len, WORD_BYTES), 0, 0).raw,
            0,
            0,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}