#include "u_types.h"
#include "syscall_backend.h"
#include "u_prot.h"
#include "u_ipc.h"

#include <pthread_impl.h>

long be_set_tid_address(va_list ap)
{
    int *val;
#define ARG0 int *
    ARG_1_BE(ap, val, ARG0);
#undef ARG0
    struct pthread *pt = pthread_self();

    if (pt)
    {
        pt->ctid = val;
    }
    return THREAD_MAIN;
}
long be_set_thread_area(va_list ap)
{
    void *p;
#define ARG0 int *
    ARG_1_BE(ap, p, ARG0);
#undef ARG0
    umword_t msg;
    umword_t len;
    ipc_msg_t *i_msg;

    thread_msg_buf_get(-1, &msg, &len);

    i_msg = (ipc_msg_t *)msg;
    i_msg->user[0] = (umword_t)p;
    return 0;
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

