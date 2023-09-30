#include "u_mm.h"
#include "u_types.h"
#include "u_prot.h"

enum mm_op
{
    MM_ALLOC,
    MM_FREE,
    MM_ALIGN_ALLOC, //!< 直接暂用一个region
    MM_ALIGN_FREE,
    MM_MOD_ATTRS,
};
void *mm_alloc_page(obj_handler_t obj_inx, umword_t pnf_nr, uint8_t attrs)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");
    register volatile umword_t r2 asm("r2");
    register volatile umword_t r3 asm("r3");
    syscall(syscall_prot_create(MM_ALLOC, MM_PROT, obj_inx),
            0,
            pnf_nr,
            attrs,
            0,
            0,
            0);
    asm __volatile__(""
                     :
                     :
                     : "r0", "r1", "r2", "r3");
    {
        msg_tag_t tag = msg_tag_init(r0);

        if (msg_tag_get_prot(tag) < 0)
        {
            return NULL;
        }
        return (void *)r1;
    }
    return NULL; /*TODO:*/
}
void mm_free_page(obj_handler_t obj_inx, void *addr, umword_t pfn_nr)
{
    syscall(syscall_prot_create(MM_FREE, MM_PROT, obj_inx),
            0,
            addr,
            pfn_nr,
            0,
            0,
            0);
}
msg_tag_t mm_align_alloc(obj_handler_t obj_inx, void *addr, umword_t size)
{
    register volatile umword_t r0 asm("r0");

    syscall(syscall_prot_create(MM_ALIGN_ALLOC, MM_PROT, obj_inx),
            0,
            addr,
            size,
            0,
            0,
            0);

    return msg_tag_init(r0);
}
