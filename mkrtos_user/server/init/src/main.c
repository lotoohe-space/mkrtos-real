
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
#include "syscall_backend.h"
#include <assert.h>
#include <stdio.h>
extern void futex_init(void);
int main(int argc, char *args[])
{
    ulog_write_str(LOG_PROT, "init..\n");
#if 0
    ulog_test();
    factory_test();
    printf_test();
    thread_test();
    thread_exit_test();
    map_test();
    mm_test();
    app_test();
    mpu_test();
    thread_press_test();
    kobj_create_press_test();
    pthread_cond_lock_test();
    pthread_lock_test();
    u_sleep_ms(1000);
    ipc_test();
    ipc_obj_test();
#endif
    uenv_t env = *u_get_global_env();
    env.ns_hd = namespace_init();
    int ret;
    ret = ret;
    // ret = app_load("mr_drv", &env);
    // if (ret < 0)
    // {
    //     printf("%s load fail, 0x%x\n", "mr_drv", ret);
    // }
    ret = app_load("rtthread_drv", &env);
    if (ret < 0)
    {
        printf("%s load fail, 0x%x\n", "mr_drv", ret);
    }
    // ret = app_load("hello", &env);
    // if (ret < 0)
    // {
    //     printf("%s load fail, 0x%x\n", "hello", ret);
    // }
    // ret = app_load("app", &env);
    // if (ret < 0)
    // {
    //     printf("app load fail, 0x%x\n", ret);
    //     // ulog_write_str(LOG_PROT, "app load fail.\n");
    // }

    // ret = app_load("fatfs", &env);
    // if (ret < 0)
    // {
    //     printf("app load fail, 0x%x\n", ret);
    // }
    // u_sleep_ms(500);

    namespace_pre_alloc_map_fd();
    namespace_loop();
    return 0;
}
