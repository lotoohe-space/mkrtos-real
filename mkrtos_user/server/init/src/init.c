/**
 * @file main.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-11-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "u_log.h"
#include "u_prot.h"
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
#include "cons.h"
#include "test/test.h"
#include "u_rpc_svr.h"
#include "namespace.h"
#include "ns_svr.h"
#include "syscall_backend.h"
#include "parse_cfg.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <u_fast_ipc.h>

#define DEFAULT_INIT_CFG "init.cfg"

#define STACK_COM_ITME_SIZE (1024+512)
ATTR_ALIGN(8)
uint8_t stack_coms[STACK_COM_ITME_SIZE];
uint8_t msg_buf_coms[MSG_BUG_LEN];
void fast_ipc_init(void)
{
    u_fast_ipc_init(stack_coms, msg_buf_coms, 1, STACK_COM_ITME_SIZE);
}
int main(int argc, char *args[])
{
    int ret;
    uenv_t *env;

#if 0
    thread_run(-1, 4);
#endif
    fast_ipc_init();
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_init");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_init");

    ulog_write_str(LOG_PROT, "init..\n");
    u_env_default_init();
    env = u_get_global_env();
    rpc_meta_init(THREAD_MAIN, &env->ns_hd);
    namespace_init(env->ns_hd);
    pm_init();
    console_init();
    parse_cfg_init();

#if defined(MKRTOS_TEST_MODE)
    printf("test_main..\n");
    test_main();
#endif

    ret = parse_cfg(DEFAULT_INIT_CFG, env);
    printf("run app num is %d.\n", ret);
    namespace_loop();
    return 0;
}
