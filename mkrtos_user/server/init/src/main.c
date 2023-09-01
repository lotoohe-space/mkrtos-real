
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
    mpu_test();
    printf_test();
    thread_test();
    ipc_test();
    thread_exit_test();
#endif
    app_test();
    task_unmap(TASK_THIS, TASK_THIS);
    printf("exit init.\n");
    return 0;
}
