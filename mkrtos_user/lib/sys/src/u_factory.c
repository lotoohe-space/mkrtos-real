#include "u_types.h"
#include "u_prot.h"
#include "u_factory.h"
#include "u_arch.h"

enum
{
    FACTORY_CREATE_KOBJ
};
msg_tag_t factory_create_irq_sender(obj_handler_t obj, vpage_t vpage)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(FACTORY_CREATE_KOBJ, FACTORY_PROT, obj).raw,
               0,
               IRQ_PROT,
               vpage.raw,
               0,
               0,
               0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t factory_create_thread(obj_handler_t obj, vpage_t vpage)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(FACTORY_CREATE_KOBJ, FACTORY_PROT, obj).raw,
               0,
               THREAD_PROT,
               vpage.raw,
               0,
               0,
               0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t factory_create_thread_vcpu(obj_handler_t obj, vpage_t vpage)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(FACTORY_CREATE_KOBJ, FACTORY_PROT, obj).raw,
               0,
               THREAD_PROT,
               vpage.raw,
               THREAD_CREATE_VM,
               0,
               0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t factory_create_task(obj_handler_t obj, vpage_t vpage)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(FACTORY_CREATE_KOBJ, FACTORY_PROT, obj).raw,
               0,
               TASK_PROT,
               vpage.raw,
               0,
               0,
               0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t factory_create_ipc(obj_handler_t obj, vpage_t vpage)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(FACTORY_CREATE_KOBJ, FACTORY_PROT, obj).raw,
               0,
               IPC_PROT,
               vpage.raw,
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
msg_tag_t facotry_create_share_mem(obj_handler_t obj, vpage_t vpage, share_mem_type_t mem_type, umword_t size)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(FACTORY_CREATE_KOBJ, FACTORY_PROT, obj).raw,
               0,
               SHARE_MEM_PROT,
               vpage.raw,
               mem_type,
               size,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t facotry_create_sema(obj_handler_t obj, vpage_t vpage, int cnt, int max)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(FACTORY_CREATE_KOBJ, FACTORY_PROT, obj).raw,
               0,
               SEMA_PROT,
               vpage.raw,
               cnt,
               max,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
