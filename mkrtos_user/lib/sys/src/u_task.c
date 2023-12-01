#include "u_types.h"
#include "u_prot.h"
#include "u_arch.h"
#include "u_task.h"
enum task_op_code
{
    TASK_OBJ_MAP,
    TASK_OBJ_UNMAP,
    TASK_ALLOC_RAM_BASE,
    TASK_OBJ_VALID,
    TASK_SET_PID,
    TASK_GET_PID,
};
msg_tag_t task_set_pid(obj_handler_t dst_task, umword_t pid)
{
    register volatile umword_t r0 asm("r0");

    mk_syscall(syscall_prot_create(TASK_SET_PID, TASK_PROT, dst_task).raw,
               pid,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : "r0");
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t task_get_pid(obj_handler_t dst_task, umword_t *pid)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");

    mk_syscall(syscall_prot_create(TASK_GET_PID, TASK_PROT, dst_task).raw,
               0,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : "r0", "r1");
    if (pid)
    {
        *pid = r1;
    }

    return msg_tag_init(r0);
}
msg_tag_t task_obj_valid(obj_handler_t dst_task, obj_handler_t obj_inx)
{
    register volatile umword_t r0 asm("r0");

    mk_syscall(syscall_prot_create(TASK_OBJ_VALID, TASK_PROT, dst_task).raw,
               0,
               obj_inx,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : "r0");
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}

msg_tag_t task_map(obj_handler_t dst_task, obj_handler_t src_obj, obj_handler_t dst_obj, uint8_t attrs)
{
    register volatile umword_t r0 asm("r0");

    mk_syscall(syscall_prot_create(TASK_OBJ_MAP, TASK_PROT, dst_task).raw,
               0,
               src_obj,
               dst_obj,
               attrs,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : "r0");
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}

msg_tag_t task_unmap(obj_handler_t task_han, vpage_t vpage)
{
    register volatile umword_t r0 asm("r0");

    mk_syscall(syscall_prot_create(TASK_OBJ_UNMAP, TASK_PROT, task_han).raw,
               0,
               vpage.raw,
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

    mk_syscall(syscall_prot_create(TASK_ALLOC_RAM_BASE, TASK_PROT, task_han).raw,
               0,
               size,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : "r0", "r1");
    if (alloc_addr)
    {
        *alloc_addr = r1;
    }

    return msg_tag_init(r0);
}
