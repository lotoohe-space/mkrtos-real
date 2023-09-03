#include "u_types.h"
#include "u_prot.h"
#include "u_ipc.h"
enum ipc_op
{
    IPC_SEND, //!< 发送IPC消息
    IPC_REVC, //!< 接受IPC消息
};
msg_tag_t ipc_recv(obj_handler_t obj, umword_t flags)
{
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(IPC_REVC, IPC_PROT, obj),
            msg_tag_init3(0, ipc_type_create_3(MSG_NONE_TYPE, 1, 0).raw, IPC_PROT).raw,
            flags,
            0,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t ipc_send(obj_handler_t obj, umword_t len, umword_t flags)
{
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(IPC_SEND, IPC_PROT, obj),
            msg_tag_init3(0, ipc_type_create_3(MSG_NONE_TYPE, 2, 0).raw, IPC_PROT).raw,
            len,
            flags,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}