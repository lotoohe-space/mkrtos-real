
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_hd_man.h"
#include <assert.h>
#include <stdio.h>
#include "test.h"
void kobj_create_press_test(void)
{
    int i = 1000;
    while (i--)
    {
        obj_handler_t hd = handler_alloc();
        msg_tag_t tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd));
        assert(msg_tag_get_prot(tag) >= 0);
        handler_free_umap(hd);
    }
}
