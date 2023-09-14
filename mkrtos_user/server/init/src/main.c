
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

extern void futex_init(void);
int main(int argc, char *args[])
{
    uint8_t data[10];
    // futex_init();
    ulog_write_str(LOG_PROT, "init..\n");
    // while (1)
    // {
    //     int len = ulog_read_bytes(LOG_PROT, data, sizeof(data) - 1);
    //     if (len > 0)
    //     {
    //         data[len] = 0;
    //         printf("%s\n", data);
    //     }
    // }
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
    ipc_timeout_test();
#endif
    app_test();
    task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, TASK_THIS)); // 删除当前task，以及申请得所有对象
    printf("exit init.\n");
    return 0;
}
