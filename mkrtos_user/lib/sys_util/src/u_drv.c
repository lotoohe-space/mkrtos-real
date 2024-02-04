

#include "u_types.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_sleep.h"

// AUTO_CALL(101)
int u_drv_init(void)
{
    msg_tag_t tag;

    tag = mm_align_alloc(MM_PROT, (void *)0x40000000, 0x50000000 - 0x40000000);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
   
    return 0;
}
