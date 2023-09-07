#include "u_types.h"
#include "u_prot.h"
#include "u_task.h"
enum task_op_code
{
    TASK_OBJ_MAP,
    TASK_OBJ_UNMAP,
    TASK_ALLOC_RAM_BASE,
};

msg_tag_t task_obj_valid(obj_handler_t dst_task, obj_handler_t obj_inx)
{
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(TASK_OBJ_MAP, TASK_PROT, dst_task),
            0,
            obj_inx,
            0,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}

msg_tag_t task_map(obj_handler_t dst_task, obj_handler_t src_obj, obj_handler_t dst_obj)
{
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(TASK_OBJ_MAP, TASK_PROT, dst_task),
            0,
            src_obj,
            dst_obj,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}

msg_tag_t task_unmap(obj_handler_t task_han, obj_handler_t obj)
{
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(TASK_OBJ_UNMAP, TASK_PROT, task_han),
            0,
            obj,
            0,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t task_alloc_ram_base(obj_handler_t task_han, umword_t size, addr_t *alloc_addr)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");

    syscall(syscall_prot_create(TASK_ALLOC_RAM_BASE, TASK_PROT, task_han),
            0,
            size,
            0,
            0,
            0,
            0);
    if (alloc_addr)
    {
        *alloc_addr = r1;
    }

    return msg_tag_init(r0);
}
