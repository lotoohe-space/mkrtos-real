
#include "u_prot.h"
#include "u_types.h"
#include "u_ipc.h"
#include "u_vmam.h"
enum share_mem_op
{
    SHARE_MEM_MAP,
    SHARE_MEM_UNMAP,
};
MK_SYSCALL
msg_tag_t share_mem_map(obj_handler_t obj, vma_addr_t vaddr, umword_t *addr, umword_t *size)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);
    register volatile umword_t r2 asm(ARCH_REG_2);
    mk_syscall(syscall_prot_create4(SHARE_MEM_MAP, SHARE_MEM_PROT, obj, FALSE).raw,
               vaddr.raw,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1, ARCH_REG_2);
    if (addr)
    {
        *addr = r1;
    }
    if (size)
    {
        *size = r2;
    }
    return msg_tag_init(r0);
}
MK_SYSCALL
msg_tag_t share_mem_unmap(obj_handler_t obj)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    mk_syscall(syscall_prot_create4(SHARE_MEM_UNMAP, SHARE_MEM_PROT, obj, FALSE).raw,
               0,
               0,
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
