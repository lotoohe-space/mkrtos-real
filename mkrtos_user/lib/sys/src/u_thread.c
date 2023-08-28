#include "u_prot.h"
#include "u_types.h"
enum thread_op
{
    SET_EXEC_REGS,
    RUN_THREAD,
    BIND_TASK,
};
msg_tag_t thread_exec_regs(obj_handler_t obj, umword_t pc, umword_t sp, umword_t ram)
{
    register volatile umword_t r0 asm("r0");

    syscall(obj, msg_tag_init3(SET_EXEC_REGS, 3, THREAD_PROT).raw,
            obj,
            pc,
            sp,
            ram,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t thread_run(obj_handler_t obj)
{
    register volatile umword_t r0 asm("r0");

    syscall(obj, msg_tag_init3(RUN_THREAD, 1, THREAD_PROT).raw,
            obj,
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
            obj,
            tk_obj,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}