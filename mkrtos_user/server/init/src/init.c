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
#define DEFAULT_INIT_CFG "init.cfg"

#define STACK_SIZE 2048
#define STASCK_NUM 4
static ATTR_ALIGN(8) uint8_t com_stack[512];
static ATTR_ALIGN(8) uint8_t cons_stack[STASCK_NUM][STACK_SIZE];
static uint8_t cons_msg_buf[STASCK_NUM][MSG_BUG_LEN];
static umword_t cons_stack_bitmap;
static uint8_t cons_msg_buf_main[MSG_BUG_LEN];
static inline umword_t arch_get_sp(void)
{
    umword_t ret;
    __asm__ __volatile__(
        "mov     %0, sp\n"
        : "=r"(ret)
        :
        :);
    return ret;
}
#define SET_SP(sp)                                    \
    do                                                \
    {                                                 \
        __asm__ __volatile__("mov sp, %0" ::"r"(sp)); \
        __asm__ __volatile__(""                       \
                             :                        \
                             :                        \
                             : "sp");                 \
    } while (0)

int setsp(int i, void *stack, msg_tag_t tag, int arg0, int arg1);

void last_process(int j, msg_tag_t tag, int arg0, int arg1)
{
    thread_msg_buf_set(-1, (void *)(cons_msg_buf[j]));
    task_com_unlock(TASK_THIS);

    // printf("j:%d sp:0x%x\n", j, arch_get_sp());
    // printf("j:%d comm tag:%x r0:%d r1:%d\n", j, tag.raw, arg0, arg1);
    // printf("%s\n", cons_msg_buf[j]);
    // strcpy((void *)cons_msg_buf[j], "okay");

    // u_sleep_ms(100);

    // *((uint8_t *)0) = 0;
    task_com_lock(TASK_THIS);
    memcpy(cons_msg_buf_main, cons_msg_buf[j], MSG_BUG_LEN);
    tag = msg_tag_init4(0, 2, 0, 0);
    thread_ipc_fast_replay(tag, -1, j);
}
static void init_com_point_test_func(msg_tag_t tag, int arg0, int arg1, int arg2)
{
    int i;
    for (i = 0; i < STASCK_NUM; i++)
    {
        if ((cons_stack_bitmap & (1 << i)) == 0)
        {
            cons_stack_bitmap |= (1 << i);
            break;
        }
    }
    memcpy(cons_msg_buf[i], cons_msg_buf_main, MSG_BUG_LEN);
    setsp(i, &cons_stack[i][STACK_SIZE - 8], tag, arg0, arg1);
}

int main(int argc, char *args[])
{
    int ret;
    uenv_t *env;

#if 0
    thread_run(-1, 4);
#endif
    task_set_com_point(TASK_THIS, &init_com_point_test_func, (addr_t)com_stack,
                       sizeof(com_stack), &cons_stack_bitmap, STASCK_NUM, cons_msg_buf_main);

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
