
#include <stdio.h>
#include "u_log.h"
#include "ns_cli.h"
#include "u_rpc_svr.h"
#include "u_prot.h"
#include "u_env.h"
#include "u_drv.h"
#include "cons_cli.h"
#include "fs_rpc.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
int main(int argv, char *args[])
{
    obj_handler_t hd;
    int ret;
    printf("args[0]:%s\n", args[0]);

    ret = rpc_meta_init(TASK_THIS, &hd);
    assert(ret >= 0);
    fs_svr_init();
    ns_register("/vfs_test", hd, 0);
    printf("cpiofs mount success\n");

    fs_svr_loop();
    return 0;
}
