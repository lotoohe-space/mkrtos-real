
#include <u_types.h>
#include <u_prot.h>
#include <u_vmam.h>

enum
{
    VMA_ALLOC,
    VMA_FREE,
    VMA_GRANT,
};

msg_tag_t u_vmam_alloc(obj_handler_t obj, vma_addr_t addr, size_t size, addr_t paddr, addr_t *vaddr)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);

    mk_syscall(syscall_prot_create4(VMA_ALLOC, VMA_PROT, obj, FALSE).raw,
               addr.raw,
               size,
               paddr,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1);
    if (vaddr)
    {
        *vaddr = r1;
    }

    return msg_tag_init(r0);
}
msg_tag_t u_vmam_grant(obj_handler_t obj, obj_handler_t dst_task_obj, 
    addr_t src_addr, addr_t dst_addr, size_t size)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create4(VMA_GRANT, VMA_PROT, obj, FALSE).raw,
               dst_task_obj,
               src_addr,
               dst_addr,
               size,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0);

    return msg_tag_init(r0);
}

msg_tag_t u_vmam_free(obj_handler_t obj, addr_t addr, size_t size)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create4(VMA_FREE, VMA_PROT, obj, FALSE).raw,
               addr,
               size,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0);
    return msg_tag_init(r0);
}
