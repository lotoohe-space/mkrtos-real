#include "u_prot.h"
#include "u_types.h"
#include "u_thread.h"
enum thread_op
{
    SET_EXEC_REGS,
    RUN_THREAD,
    BIND_TASK,
    MSG_BUG_GET,
    MSG_BUG_SET,
};
msg_tag_t thread_msg_buf_set(obj_handler_t obj, void *msg)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");
    register volatile umword_t r2 asm("r2");

    syscall(syscall_prot_create(MSG_BUG_SET, THREAD_PROT, obj),
            0,
            msg,
            0,
            0,
            0,
            0);
    return msg_tag_init(r0);
}
msg_tag_t thread_msg_buf_get(obj_handler_t obj, umword_t *msg, umword_t *len)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");
    register volatile umword_t r2 asm("r2");

    syscall(syscall_prot_create4(MSG_BUG_GET, THREAD_PROT, obj, TRUE),
            0,
            0,
            0,
            0,
            0,
            0);
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
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(SET_EXEC_REGS, THREAD_PROT, obj),
            0,
            pc,
            sp,
            ram,
            cp_stack,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t thread_run(obj_handler_t obj, uint8_t prio)
{
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(RUN_THREAD, THREAD_PROT, obj),
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
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(BIND_TASK, THREAD_PROT, obj),
            0,
            tk_obj,
            0,
            0,
            0, 0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}