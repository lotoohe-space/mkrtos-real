/**
 * @file futex_backend.c
 * @author ATShining (1358745329@qq.com)
 * @brief futex锁
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
#include <syscall_backend.h>
#include <features.h>
#undef hidden
#undef weak
#define hidden
#include <pthread_impl.h>
#include <time.h>
#include <errno.h>
int be_futex(uint32_t *uaddr, int futex_op, uint32_t val,
             const struct timespec *timeout, uint32_t uaddr2, uint32_t val3)
{
    struct pthread *pt = __pthread_self();
    ipc_timeout_t to = ipc_timeout_create2(0, 0);
    umword_t total = 0;
    umword_t st_val;
    sys_info_t sys_info;
    msg_tag_t tag;

    if (timeout && !(futex_op & FUTEX_REQUEUE))
    {
        to = ipc_timeout_create2(timeout->tv_sec * 1000 + timeout->tv_nsec / 1000 / 1000, 0);
        total = timeout->tv_sec * 1000 + timeout->tv_nsec / 1000 / 1000;
    }

_try_again:
    sys_read_info(SYS_PROT, &sys_info, 0);
    st_val = sys_info.sys_tick;
    tag = futex_ctrl(FUTEX_PROT, uaddr, futex_op, val, total, uaddr2, val3, pt->tid);
    if (msg_tag_get_val(tag) == -ETIMEDOUT)
    {
        umword_t en_val;

        sys_read_info(SYS_PROT, &sys_info, 0);
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

long sys_futex(va_list ap)
{
    uint32_t *uaddr;
    long futex_op;
    long val;
    const struct timespec *timeout;
    long uaddr2;
    long val3;

    ARG_6_BE(ap, uaddr, typeof(uaddr), futex_op, typeof(futex_op), val, typeof(val),
             timeout, typeof(struct timespec *), uaddr2, typeof(uaddr2), val3, typeof(val3));

    return be_futex(uaddr, futex_op, val, timeout, uaddr2, val3);
}
