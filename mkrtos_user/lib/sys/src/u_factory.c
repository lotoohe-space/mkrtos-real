#include "u_types.h"
#include "u_prot.h"
#include "u_factory.h"

enum
{
    FACTORY_CREATE_KOBJ
};

msg_tag_t factory_create_thread(obj_handler_t obj, obj_handler_t tgt_obj_handler)
{
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(FACTORY_CREATE_KOBJ, FACTORY_PROT, obj),
            0,
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

    syscall(syscall_prot_create(FACTORY_CREATE_KOBJ, FACTORY_PROT, obj),
            0,
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

    syscall(syscall_prot_create(FACTORY_CREATE_KOBJ, FACTORY_PROT, obj),
            0,
            IPC_PROT,
            tgt_obj_handler,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
