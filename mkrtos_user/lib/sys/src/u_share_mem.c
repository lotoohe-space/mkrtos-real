
#include "u_prot.h"
#include "u_types.h"
#include "u_ipc.h"
enum share_mem_op
{
    SHARE_MEM_MAP,
    SHARE_MEM_UNMAP,
};

msg_tag_t share_mem_map(obj_handler_t obj, uint8_t attrs, umword_t *addr, umword_t *size)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");
    register volatile umword_t r2 asm("r2");
    mk_syscall(syscall_prot_create4(SHARE_MEM_MAP, SHARE_MEM_PROT, obj, FALSE).raw,
               attrs,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : "r0", "r1", "r2");
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
msg_tag_t share_mem_unmap(obj_handler_t obj)
{
    register volatile umword_t r0 asm("r0");
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
                     : "r0");
    return msg_tag_init(r0);
}
