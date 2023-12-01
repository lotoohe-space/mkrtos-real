/**
 * @file main.c
 * @author zhangzheng (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */
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
#include "u_rpc_svr.h"
#include "pm.h"

#include "test/test.h"
#include "u_rpc_svr.h"
#include "namespace.h"
#include "ns_svr.h"
#include "syscall_backend.h"
#include "parse_cfg.h"
#include <assert.h>
#include <stdio.h>

#define DEFAULT_INIT_CFG "init.cfg"

static void test(void)
{
#if 0
    ulog_test();
    factory_test();
    printf_test();
    thread_exit_test();
    map_test();

    mm_test();
    app_test();
    mpu_test();
    thread_press_test();
    kobj_create_press_test();
    u_sleep_ms(1000);
    ipc_test();
    ipc_obj_test();
    pthread_cond_lock_test();
    pthread_lock_test();
#endif
}
int main(int argc, char *args[])
{
    int ret;
    uenv_t *env;

    ulog_write_str(LOG_PROT, "init..\n");
    test();
    env = u_get_global_env();
    rpc_meta_init(THREAD_MAIN, &env->ns_hd);
    namespace_init();
    pm_init();


    ret = parse_cfg(DEFAULT_INIT_CFG, env);
    printf("run app num is %d.\n", ret);
    namespace_pre_alloc_map_fd();
    namespace_loop();
    return 0;
}
