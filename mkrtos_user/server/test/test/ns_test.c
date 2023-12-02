
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
#if 0
    int ret;
    obj_handler_t tmp_ipc_hd;

    tmp_ipc_hd = handler_alloc();
    assert(tmp_ipc_hd != HANDLER_INVALID);
    msg_tag_t tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(0, 0, tmp_ipc_hd));
    assert(msg_tag_get_val(tag) >= 0);
    ret = cli_ns_register("shell", tmp_ipc_hd);
    assert(ret >= 0);
    ret = cli_ns_query("shell", &tmp_ipc_hd);
    assert(ret >= 0);
    ulog_write_str(u_get_global_env()->log_hd, "ns test success.\n");
#endif
    obj_handler_t tmp_ipc_hd;

    tmp_ipc_hd = handler_alloc();
    assert(tmp_ipc_hd != HANDLER_INVALID);
    msg_tag_t tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(0, 0, tmp_ipc_hd));

    assert(ns_register("shell", tmp_ipc_hd) >= 0);
    obj_handler_t rcv_ipc_hd;

    assert(ns_query("shell", &rcv_ipc_hd) >= 0);

    handler_free_umap(rcv_ipc_hd);
}