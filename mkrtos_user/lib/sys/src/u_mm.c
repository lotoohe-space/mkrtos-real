#include "u_mm.h"
#include "u_types.h"
#include "u_prot.h"

enum mm_op
{
    MM_ALLOC,
    MM_FREE,
    MM_MOD_ATTRS,
};

void *mm_alloc_page(obj_handler_t obj_inx, umword_t pnf_nr, uint8_t attrs)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");
    register volatile umword_t r2 asm("r2");
    register volatile umword_t r3 asm("r3");
    syscall(syscall_prot_create(MM_ALLOC, MM_PROT, obj_inx),
            msg_tag_init3(0, ipc_type_create_3(MSG_NONE_TYPE, 2, 0).raw, MM_PROT).raw,
            pnf_nr,
            attrs,
            0,
            0,
            0);
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
            msg_tag_init3(0, ipc_type_create_3(MSG_NONE_TYPE, 2, 0).raw, MM_PROT).raw,
            addr,
            pfn_nr,
            0,
            0,
            0);
}
