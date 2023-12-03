
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_env.h"
#include "u_hd_man.h"
#include "u_ns.h"
#include "ns_cli.h"
#include <assert.h>

void ns_test(void)
{
    obj_handler_t tmp_ipc_hd;

    tmp_ipc_hd = handler_alloc();
    assert(tmp_ipc_hd != HANDLER_INVALID);
    msg_tag_t tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(0, 0, tmp_ipc_hd));

    assert(ns_register("shell", tmp_ipc_hd, FILE_NODE) >= 0);
    obj_handler_t rcv_ipc_hd;

    assert(ns_query("shell", &rcv_ipc_hd) >= 0);

    handler_free_umap(rcv_ipc_hd);
}