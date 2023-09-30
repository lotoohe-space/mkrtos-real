#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_hd_man.h"

void sleep_tick(int tick)
{
    obj_handler_t hd = handler_alloc();

    factory_create_ipc(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd));
    ipc_call(hd, msg_tag_init4(0, 0, 0, 0), ipc_timeout_create2(tick, 0));
    handler_free_umap(hd);
}
