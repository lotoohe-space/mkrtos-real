
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
#include <assert.h>
#include <stdio.h>

int main(int argc, char *args[])
{
    printf("argc:%d args[0]:%s\n", argc, args[0]);
    ulog_write_str(u_get_global_env()->log_hd, "MKRTOS:\n");
#if 0
    malloc_test();
    rpc_test();
    ns_test();
#endif
    fs_test();
    irq_test();
    task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, TASK_THIS));
    ulog_write_str(u_get_global_env()->log_hd, "Error.\n");
    return 0;
}
