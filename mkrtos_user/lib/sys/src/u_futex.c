#include "u_types.h"
#include "u_prot.h"
#include "u_futex.h"
#include "u_arch.h"
#include "u_thread.h"
#include "u_ipc.h"
#include "u_err.h"
enum futex_op
{
    FUTEX_CTRL,
};
MK_SYSCALL
msg_tag_t futex_ctrl(obj_handler_t obj, uint32_t *uaddr, int futex_op, uint32_t val,
                     umword_t timeout, uint32_t uaddr2, uint32_t val3, int tid)
{
    ipc_msg_t *msg;

    thread_msg_buf_get(-1, (umword_t *)(&msg), NULL);

    if (msg == NULL)
    {
        return msg_tag_init4(0, 0, 0, -MK_ENOBUFS);
    }
    msg->msg_buf[0] = (umword_t)uaddr;
    msg->msg_buf[1] = (umword_t)futex_op;
    msg->msg_buf[2] = (umword_t)val;
    msg->msg_buf[3] = (umword_t)timeout;
    msg->msg_buf[4] = (umword_t)uaddr2;
    msg->msg_buf[5] = (umword_t)val3;
    msg->msg_buf[6] = (umword_t)tid;
    {
        register volatile umword_t r0 asm(ARCH_REG_0);
        mk_syscall(syscall_prot_create(FUTEX_CTRL, FUTEX_PROT, obj).raw,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0);
        msg_tag_t tag = msg_tag_init(r0);

        return tag;
    }
}