#include "u_types.h"
#include "u_prot.h"
#include "u_factory.h"
#include "u_task.h"
#include "u_hd_man.h"
#include "u_share_mem.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
AUTO_CALL(102)
void sharea_mem_test(void)
{
    addr_t addr;
    umword_t size;
    obj_handler_t hd = handler_alloc();
    assert(hd != HANDLER_INVALID);
    msg_tag_t tag = facotry_create_share_mem(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd), 1024);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = share_mem_map(hd, 3, &addr, &size);
    assert(msg_tag_get_prot(tag) >= 0);
    memset((void *)addr, 0, size);
    // share_mem_unmap(hd);
    handler_free_umap(hd);
}
