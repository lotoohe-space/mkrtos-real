#include "u_types.h"
#include "syscall_backend.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_task.h"
#include "u_env.h"
#include "u_log.h"
#include "u_thread.h"
#include "u_sys.h"
#include <errno.h>
#include <u_sleep.h>
#undef hidden
#undef weak
#define hidden
#include <features.h>
#include <pthread_impl.h>
long be_set_tid_address(int *val)
{
    struct pthread *pt = pthread_self();

    if (pt)
    {
        pt->ctid = (unsigned long)val;
    }
    return THREAD_MAIN;
}

long sys_set_tid_address(va_list ap)
{
    int *val;
#define ARG0 int *
    ARG_1_BE(ap, val, ARG0);
#undef ARG0

    return be_set_tid_address(val);
}
long be_set_thread_area(void *p)
{
    umword_t msg;
    umword_t len;
    ipc_msg_t *i_msg;

    thread_msg_buf_get(-1, &msg, &len);

    i_msg = (ipc_msg_t *)msg;
    i_msg->user[0] = (umword_t)p;
    return 0;
}
long sys_set_thread_area(va_list ap)
{
    void *p;
#define ARG0 int *
    ARG_1_BE(ap, p, ARG0);
#undef ARG0
    return be_set_thread_area(p);
}
unsigned long get_thread_area(void)
{
    umword_t msg;
    umword_t len;
    ipc_msg_t *i_msg;

    thread_msg_buf_get(-1, &msg, &len);
    i_msg = (ipc_msg_t *)msg;

    return i_msg->user[0];
}
long be_nanosleep(
    const struct timespec *req,
    struct timespec *rem)
{
    size_t ms;

    if (req == NULL)
    {
        return -EFAULT;
    }
    if (req->tv_nsec < 0 || req->tv_nsec > 999999999 || req->tv_sec < 0)
    {
        return -EINVAL;
    }
    ms = (req->tv_sec * 1000) + (req->tv_nsec / 1000000);
    u_sleep_ms(ms);
    return 0;
}
long be_clock_nanosleep(clockid_t clock_id,
                        long flags,
                        const struct timespec *req,
                        struct timespec *rem)
{
    size_t ms;

    if (req == NULL)
    {
        return -EFAULT;
    }
    if (req->tv_nsec < 0 || req->tv_nsec > 999999999 || req->tv_sec < 0)
    {
        return -EINVAL;
    }
    ms = (req->tv_sec * 1000000) + (req->tv_nsec / 1000);
    u_sleep_ms(ms);
    return 0;
}
long sys_clock_nanosleep(va_list ap)
{
    clockid_t clock_id;
    long flags;
    const struct timespec *req;
    struct timespec *rem;

    ARG_4_BE(ap, clock_id, clockid_t, flags, long, req, const struct timespec *, rem, struct timespec *);

    return be_clock_nanosleep(clock_id, flags, req, rem);
}
long be_clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    #define NCLOCKS 4
    if (clk_id >= NCLOCKS)
    {
        return -ENODEV;
    }
    sys_info_t info;
    sys_read_info(SYS_PROT, &info, 0);

    unsigned long ts = info.sys_tick;

    tp->tv_sec = ts / 1000;
    tp->tv_nsec = (ts % 1000) * 1000 * 1000;

    return 0;
}