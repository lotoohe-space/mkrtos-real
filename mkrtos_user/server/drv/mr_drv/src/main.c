#include "drv_rpc.h"
#include <u_types.h>
#include <u_prot.h>
#include <assert.h>
#include <ns_cli.h>
#include <stdio.h>
#include <u_factory.h>
#include <u_rpc_svr.h>
#include <mr_api.h>
int main(int argc, char *args[])
{
    obj_handler_t ipc_hd;
    int ret;
    printf("mr drv init...\n");
    ret = rpc_creaite_bind_ipc(THREAD_MAIN, NULL, &ipc_hd);
    assert(ret >= 0);
    ns_register("/dev", ipc_hd);
    printf("mr drv start success...\n");

    mr_auto_init();
    drv_svr_init(ipc_hd);
    drv_svr_loop();
    return 0;
}
