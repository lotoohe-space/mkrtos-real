/**
 * @file futex_backend.c
 * @author zhangzheng (1358745329@qq.com)
 * @brief 该文件用ipc模拟一个futex锁
 * @version 0.1
 * @date 2023-09-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "u_ipc.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_hd_man.h"
#include "u_log.h"
#include "futex_queue.h"
#include "u_ipc.h"
#include "u_sys.h"
#include "u_futex.h"
#include <pthread.h>
#include <futex.h>
#include <assert.h>
#include <limits.h>
#include <pthread_impl.h>
#include <time.h>
int be_futex(uint32_t *uaddr, int futex_op, uint32_t val,
             const struct timespec *timeout, uint32_t uaddr2, uint32_t val3)
{
    struct pthread *pt = __pthread_self();
    ipc_timeout_t to = ipc_timeout_create2(0, 0);
    umword_t total = 0;
    sys_info_t sys_info;

    if (timeout && !(futex_op & FUTEX_REQUEUE))
    {
        to = ipc_timeout_create2(timeout->tv_sec * 1000 + timeout->tv_nsec / 1000 / 1000, 0);
        total = timeout->tv_sec * 1000 + timeout->tv_nsec / 1000 / 1000;
    }
    umword_t st_val;

_try_again:

    sys_read_info(SYS_PROT, &sys_info);
    st_val = sys_info.sys_tick;
    msg_tag_t tag = futex_ctrl(FUTEX_PROT, uaddr, futex_op, val, (void *)timeout, uaddr2, val3, pt->tid);
    if (msg_tag_get_val(tag) == -EWTIMEDOUT)
    {
        umword_t en_val;

        sys_read_info(SYS_PROT, &sys_info);
        en_val = sys_info.sys_tick;
        if (en_val - st_val < total)
        {
            total = total - (en_val - st_val);

            to = ipc_timeout_create2(total, 0);
            goto _try_again;
        }
    }
    return msg_tag_get_val(tag);
}
