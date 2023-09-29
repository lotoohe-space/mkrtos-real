#include "u_types.h"
#include "u_prot.h"
#include "u_factory.h"
#include "u_task.h"
#include "u_hd_man.h"
#include <assert.h>
#include <stdio.h>
void factory_test(void)
{
    obj_handler_t hd = handler_alloc();
    assert(hd != HANDLER_INVALID);
    msg_tag_t tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd));
    if (msg_tag_get_prot(tag) < 0)
    {
        printf("factory_create_ipc no memory\n");
        return;
    }
    handler_free_umap(hd);
}
