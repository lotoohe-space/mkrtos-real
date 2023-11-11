#include "u_log.h"
#include "ns_cli.h"
#include "u_rpc_svr.h"
#include "u_prot.h"
#include "u_env.h"
#include "u_drv.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main(int args, char *argv[])
{
    obj_handler_t ipc_hd;
    int ret;

    u_drv_init();
    ret = rpc_creaite_bind_ipc(THREAD_MAIN, NULL, &ipc_hd);
    assert(ret >= 0);
    ns_register("/dev", ipc_hd);
    fs_svr_init(ipc_hd);
    ulog_write_str(u_get_global_env()->log_hd, "drv framework success\n");
    fs_svr_loop();
    return 0;
}
