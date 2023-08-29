#include "u_types.h"
#include "u_prot.h"
#include "u_factory.h"
msg_tag_t factory_create_thread(obj_handler_t obj, obj_handler_t tgt_obj_handler)
{
    register volatile umword_t r0 asm("r0");

    syscall(obj, msg_tag_init3(0, 2, FACTORY_PROT).raw,
            THREAD_PROT,
            tgt_obj_handler,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t factory_create_task(obj_handler_t obj, obj_handler_t tgt_obj_handler)
{
    register volatile umword_t r0 asm("r0");

    syscall(obj, msg_tag_init3(0, 2, FACTORY_PROT).raw,
            TASK_PROT,
            tgt_obj_handler,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t factory_create_ipc(obj_handler_t obj, obj_handler_t tgt_obj_handler)
{
    register volatile umword_t r0 asm("r0");

    syscall(obj, msg_tag_init3(0, 2, FACTORY_PROT).raw,
            IPC_PROT,
            tgt_obj_handler,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
