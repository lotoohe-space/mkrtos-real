
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include <assert.h>
#include <stdio.h>
#include "u_ipc.h"

int main(int argc, char *args[])
{
    ulog_write_str(LOG_PROT, "init..\n");
#if 0
    mm_test();
    ulog_test();
    factory_test();
    app_test();
    mpu_test();
    printf_test();
    thread_test();
#endif
    ipc_test();
    printf("exit init.\n");
    while (1)
        ;
    // task_unmap(TASK_PROT, TASK_PROT);
    return 0;
}
