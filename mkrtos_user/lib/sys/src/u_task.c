#include "u_task.h"
#include "u_arch.h"
#include "u_prot.h"
#include "u_types.h"

enum task_op_code
{
    TASK_OBJ_MAP,
    TASK_OBJ_UNMAP,
    TASK_ALLOC_RAM_BASE,
    TASK_OBJ_VALID,
    TASK_SET_PID,
    TASK_GET_PID,
    TASK_COPY_DATA,
    TASK_SET_OBJ_NAME,
    TASK_COPY_DATA_TO, //!< 从当前task拷贝数据到目的task
    TASK_SET_COM_POINT,
    TASK_COM_UNLOCK,
    TASK_COM_LOCK,
};

msg_tag_t task_set_obj_name(obj_handler_t dst_task, obj_handler_t obj, const char *name)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    umword_t data0 = ((umword_t *)name)[0];
    umword_t data1 = ((umword_t *)name)[1];

    mk_syscall(syscall_prot_create(TASK_SET_OBJ_NAME, TASK_PROT, dst_task).raw,
               obj,
               data0,
               data1,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0);
    msg_tag_t tag = msg_tag_init(r0);
}
msg_tag_t task_set_pid(obj_handler_t dst_task, umword_t pid)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

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
                     : ARCH_REG_0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t task_get_pid(obj_handler_t dst_task, umword_t *pid)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);

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
                     : ARCH_REG_0, ARCH_REG_1);
    if (pid)
    {
        *pid = r1;
    }

    return msg_tag_init(r0);
}
msg_tag_t task_obj_valid(obj_handler_t dst_task, obj_handler_t obj_inx, int *obj_type)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);

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
                     : ARCH_REG_0);
    if (obj_type)
    {
        *obj_type = r1;
    }

    return msg_tag_init(r0);
}

msg_tag_t task_map(obj_handler_t dst_task, obj_handler_t src_obj, obj_handler_t dst_obj, uint8_t attrs)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

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
                     : ARCH_REG_0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}

msg_tag_t task_unmap(obj_handler_t task_han, vpage_t vpage)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

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
msg_tag_t task_alloc_ram_base(obj_handler_t task_han, umword_t size, addr_t *alloc_addr,int mem_block)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);

    mk_syscall(syscall_prot_create(TASK_ALLOC_RAM_BASE, TASK_PROT, task_han).raw,
               0,
               size,
               mem_block,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1);
    if (alloc_addr)
    {
        *alloc_addr = r1;
    }

    return msg_tag_init(r0);
}
msg_tag_t task_copy_data(obj_handler_t task_obj, void *st_addr, umword_t size)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(TASK_COPY_DATA, TASK_PROT, task_obj).raw,
               st_addr,
               size,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1);

    return msg_tag_init(r0);
}
msg_tag_t task_copy_data_to(obj_handler_t task_obj, obj_handler_t dst_task_obj, void *st_addr, void *dst_addr, umword_t size)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(TASK_COPY_DATA_TO, TASK_PROT, task_obj).raw,
               dst_task_obj,
               st_addr,
               dst_addr,
               size,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1);

    return msg_tag_init(r0);
}
msg_tag_t task_set_com_point(obj_handler_t task_obj, void *com_point_func, addr_t stack, umword_t stack_size, void *bitmap, int bitmap_len, void *msg_buf)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(TASK_SET_COM_POINT, TASK_PROT, task_obj).raw,
               com_point_func,
               stack,
               stack_size,
               bitmap,
               bitmap_len,
               msg_buf);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1);

    return msg_tag_init(r0);
}
msg_tag_t task_com_unlock(obj_handler_t task_obj)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(TASK_COM_UNLOCK, TASK_PROT, task_obj).raw,
               0,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1);

    return msg_tag_init(r0);
}

msg_tag_t task_com_lock(obj_handler_t task_obj)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(TASK_COM_LOCK, TASK_PROT, task_obj).raw,
               0,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1);

    return msg_tag_init(r0);
}
