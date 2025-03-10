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
#include "cons.h"
#include "ns.h"
#include "ns_svr.h"
#include "parse_cfg.h"
#include "pm.h"
#include "syscall_backend.h"
#include "test/test.h"
#include "u_app_loader.h"
#include "u_factory.h"
#include "u_hd_man.h"
#include "u_ipc.h"
#include "u_irq_sender.h"
#include "u_log.h"
#include "u_prot.h"
#include "u_rpc_svr.h"
#include "u_sleep.h"
#include "u_task.h"
#include "u_thread.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <u_fast_ipc.h>
#include "nsfs.h"
#include "tty.h"
#define DEFAULT_INIT_CFG "init.cfg"

#define STACK_NUM 4
#define STACK_COM_ITME_SIZE ((1024+512) * STACK_NUM)
ATTR_ALIGN(8)
static uint8_t stack_coms[STACK_COM_ITME_SIZE * STACK_NUM];
static uint8_t msg_buf_coms[MSG_BUG_LEN * STACK_NUM];
static obj_handler_t com_th_obj[STACK_NUM];
static void fast_ipc_init(void)
{
    for (int i = 0; i < STACK_NUM; i++)
    {
        com_th_obj[i] = handler_alloc();
        assert(com_th_obj[i] != HANDLER_INVALID);
    }
    u_fast_ipc_init(stack_coms,
                    msg_buf_coms, STACK_NUM, STACK_COM_ITME_SIZE, com_th_obj);
}
int main(int argc, char *args[])
{
    int ret;
    uenv_t *env;

    fast_ipc_init();
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_init");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_init");

    ulog_write_str(LOG_PROT, "init..\n");
    u_env_default_init();
    env = u_get_global_env();
    rpc_meta_init_def(TASK_THIS, &env->ns_hd);
    namespace_init(env->ns_hd);
    pm_init();
    parse_cfg_init();

    fs_ns_mkdir("/dev");
    fs_ns_mkdir("/sys");
#if defined(MKRTOS_TEST_MODE)
    printf("test_main..\n");
    test_main();
#endif
    tty_svr_init();
    ret = parse_cfg(DEFAULT_INIT_CFG, env);
    printf("run app num is %d.\n", ret);
    while (1)
    {
        u_sleep_ms(U_SLEEP_ALWAYS);
    }
    return 0;
}
