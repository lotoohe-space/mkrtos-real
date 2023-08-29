#include "u_types.h"
#include "u_prot.h"
enum ipc_op
{
    IPC_SEND, //!< 发送IPC消息
    IPC_REVC, //!< 接受IPC消息
};
msg_tag_t ipc_recv(obj_handler_t obj)
{
    register volatile umword_t r0 asm("r0");

    syscall(obj, msg_tag_init3(IPC_REVC, 2, IPC_PROT).raw,
            0,
            0,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t ipc_send(obj_handler_t obj, umword_t len)
{
    register volatile umword_t r0 asm("r0");

    syscall(obj, msg_tag_init3(IPC_SEND, 2, IPC_PROT).raw,
            len,
            0,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}