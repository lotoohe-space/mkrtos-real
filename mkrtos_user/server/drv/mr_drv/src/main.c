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
    ns_register("/dev", drv_svr_init());
    printf("mr drv start success...\n");
    mr_auto_init();
    drv_svr_loop();
    return 0;
}
