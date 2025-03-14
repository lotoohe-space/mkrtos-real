/**
 * @file u_sleep.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "u_types.h"
#include "u_hd_man.h"
#include "u_prot.h"
#include "u_task.h"
#include "u_arch.h"
#include "u_ipc.h"
#include "u_factory.h"
#include "u_thread.h"
#include <sys/types.h>
#include <assert.h>
static obj_handler_t hd = HANDLER_INVALID;

void u_sleep_ms(size_t ms)
{
    umword_t sleep_tick = ROUND_UP(ms, (1000 / CONFIG_SYS_SCHE_HZ));
    // thread_ipc_wait(ipc_timeout_create2(0, sleep_tick), NULL, -1);
    thread_sleep(sleep_tick);
}
