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
msg_tag_t thread_msg_bug_set(obj_handler_t obj, void *msg)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");
    register volatile umword_t r2 asm("r2");

    syscall(obj, msg_tag_init3(MSG_BUG_SET, 0, THREAD_PROT).raw,
            msg,
            0,
            0,
            0,
            0);
    return msg_tag_init(r0);
}
msg_tag_t thread_msg_bug_get(obj_handler_t obj, umword_t *msg, umword_t *len)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");
    register volatile umword_t r2 asm("r2");

    syscall(obj, msg_tag_init3(MSG_BUG_GET, 0, THREAD_PROT).raw,
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
msg_tag_t thread_exec_regs(obj_handler_t obj, umword_t pc, umword_t sp, umword_t ram)
{
    register volatile umword_t r0 asm("r0");

    syscall(obj, msg_tag_init3(SET_EXEC_REGS, 3, THREAD_PROT).raw,
            pc,
            sp,
            ram,
            0, 0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t thread_run(obj_handler_t obj)
{
    register volatile umword_t r0 asm("r0");

    syscall(obj, msg_tag_init3(RUN_THREAD, 1, THREAD_PROT).raw,
            0,
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

    syscall(obj, msg_tag_init3(BIND_TASK, 1, THREAD_PROT).raw,
            tk_obj,
            0,
            0,
            0, 0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}