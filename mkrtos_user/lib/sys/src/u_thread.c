#include "u_prot.h"
#include "u_types.h"
#include "u_thread.h"
#include "u_ipc.h"
enum thread_op
{
    SET_EXEC_REGS,
    RUN_THREAD,
    BIND_TASK,
    MSG_BUG_GET,
    MSG_BUG_SET,
    YIELD,
    DO_IPC,
};
enum IPC_TYPE
{
    IPC_CALL,
    IPC_REPLY,
    IPC_WAIT,
    IPC_RECV,
    IPC_SEND,
};
msg_tag_t thread_ipc_wait(ipc_timeout_t timeout, umword_t *obj, obj_handler_t ipc_obj)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);
    mk_syscall(syscall_prot_create4(DO_IPC, THREAD_PROT, -1, TRUE).raw,
               0,
               IPC_WAIT,
               0,
               timeout.raw,
               ipc_obj,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1);
    if (obj)
    {
        *obj = r1;
    }
    return msg_tag_init(r0);
}
msg_tag_t thread_ipc_reply(msg_tag_t in_tag, ipc_timeout_t timeout)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    mk_syscall(syscall_prot_create4(DO_IPC, THREAD_PROT, -1, TRUE).raw,
               in_tag.raw,
               IPC_REPLY,
               0,
               timeout.raw,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0);
    return msg_tag_init(r0);
}
msg_tag_t thread_ipc_send(msg_tag_t in_tag, obj_handler_t target_th_obj, ipc_timeout_t timeout)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    mk_syscall(syscall_prot_create4(DO_IPC, THREAD_PROT, target_th_obj, TRUE).raw,
               in_tag.raw,
               IPC_SEND,
               0,
               timeout.raw,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0);
    return msg_tag_init(r0);
}
msg_tag_t thread_ipc_call(msg_tag_t in_tag, obj_handler_t target_th_obj, ipc_timeout_t timeout)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    mk_syscall(syscall_prot_create4(DO_IPC, THREAD_PROT, target_th_obj, TRUE).raw,
               in_tag.raw,
               IPC_CALL,
               0,
               timeout.raw,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0);
    return msg_tag_init(r0);
}
msg_tag_t thread_yield(obj_handler_t obj)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create4(YIELD, THREAD_PROT, obj, TRUE).raw,
               0,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1, ARCH_REG_2);
    return msg_tag_init(r0);
}
msg_tag_t thread_msg_buf_set(obj_handler_t obj, void *msg)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);
    register volatile umword_t r2 asm(ARCH_REG_2);

    mk_syscall(syscall_prot_create(MSG_BUG_SET, THREAD_PROT, obj).raw,
               0,
               msg,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1, ARCH_REG_2);
    return msg_tag_init(r0);
}
msg_tag_t thread_msg_buf_get(obj_handler_t obj, umword_t *msg, umword_t *len)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);
    register volatile umword_t r2 asm(ARCH_REG_2);

    mk_syscall(syscall_prot_create4(MSG_BUG_GET, THREAD_PROT, obj, TRUE).raw,
               0,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1, ARCH_REG_2);
    if (msg)
    {
        *msg = r1;
    }
    if (len)
    {
        *len = r2;
    }

    return msg_tag_init(r0);
}
msg_tag_t thread_exec_regs(obj_handler_t obj, umword_t pc, umword_t sp, umword_t ram, umword_t cp_stack)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(SET_EXEC_REGS, THREAD_PROT, obj).raw,
               0,
               pc,
               sp,
               ram,
               cp_stack,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t thread_run(obj_handler_t obj, uint8_t prio)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create4(RUN_THREAD, THREAD_PROT, obj, TRUE).raw,
               0,
               prio,
               0,
               0,
               0,
               0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t thread_bind_task(obj_handler_t obj, obj_handler_t tk_obj)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(BIND_TASK, THREAD_PROT, obj).raw,
               0,
               tk_obj,
               0,
               0,
               0, 0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}