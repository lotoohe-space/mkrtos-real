
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
#include "test.h"
#include "u_rpc.h"
#include "cons_cli.h"
#include <assert.h>
#include <stdio.h>

int main(int argc, char *args[])
{
    printf("argc:%d args[0]:%s\n", argc, args[0]);
#if 0
    malloc_test();
    rpc_test();
    ns_test();
    irq_test();
#endif
    fs_test();
    return 0;
}