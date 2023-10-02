
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_sleep.h"
#include "u_ipc.h"
#include "u_hd_man.h"
#include "u_irq_sender.h"
#include "u_app_loader.h"

#include "test/test.h"
#include "u_rpc_svr.h"
#include "namespace.h"
#include "ns_svr.h"
#include <assert.h>
#include <stdio.h>
extern void futex_init(void);
int main(int argc, char *args[])
{
    // futex_init();
    ulog_write_str(LOG_PROT, "init..\n");
#if 0
    ulog_test();
    factory_test();
    printf_test();
    thread_test();
    thread_exit_test();
    map_test();
    ipc_timeout_test();
    mm_test();
    app_test();
    mpu_test();
    ipc_test();
    thread_press_test();
    kobj_create_press_test();
#endif
    uenv_t env = *u_get_global_env();
    obj_handler_t ipc_hd;
    int ret = rpc_creaite_bind_ipc(THREAD_MAIN, NULL, &ipc_hd);
    assert(ret >= 0);
    env.ns_hd = ipc_hd;
    namespace_init(ipc_hd);
    u_sleep_init();
    ret = app_load("app", &env);
    if (ret < 0)
    {
        printf("app load fail, 0x%x\n", ret);
        // ulog_write_str(LOG_PROT, "app load fail.\n");
    }
    ret = app_load("fatfs", &env);
    if (ret < 0)
    {
        printf("app load fail, 0x%x\n", ret);
    }
    // u_sleep_ms(500);

    // u_sleep_ms(500);

    ret = app_load("hello", &env);
    if (ret < 0)
    {
        printf("app load fail, 0x%x\n", ret);
    }
    namespace_pre_alloc_map_fd();
    namespace_loop();
    // task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, TASK_THIS)); // 删除当前task，以及申请得所有对象
    // printf("exit init.\n");
    // ulog_write_str(LOG_PROT, "app load fail.\n");
    return 0;
}
