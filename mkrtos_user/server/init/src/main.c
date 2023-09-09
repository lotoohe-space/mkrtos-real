
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include <assert.h>
#include <stdio.h>
#include "u_ipc.h"
#include "u_hd_man.h"

int main(int argc, char *args[])
{
    ulog_write_str(LOG_PROT, "init..\n");
#if 0
    mm_test();
    ulog_test();
    factory_test();
    mpu_test();
    printf_test();
    thread_test();
    thread_exit_test();
    ipc_test();
    map_test();
#endif
    // ipc_timeout_test();
    app_test();
    // while (1)
    // ;
    // factory_create_ipc(FACTORY_PROT, vpage_create_raw3(0, 0, 10));
    // factory_create_thread(FACTORY_PROT, vpage_create_raw3(0, 0, 11));
    // factory_create_task(FACTORY_PROT, vpage_create_raw3(0, 0, 12));
    task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, TASK_THIS)); // 删除当前task，以及申请得所有对象
    printf("exit init.\n");
    return 0;
}
