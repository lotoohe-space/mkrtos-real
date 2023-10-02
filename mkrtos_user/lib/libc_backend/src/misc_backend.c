#include "u_types.h"
#include "syscall_backend.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_task.h"
#include "u_env.h"
#include <pthread_impl.h>

long be_set_tid_address(int *val)
{
    struct pthread *pt = pthread_self();

    if (pt)
    {
        pt->ctid = val;
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
void be_exit(int code)
{
    /*TODO:暂时先这样*/
    task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, TASK_THIS));
    ulog_write_str(u_get_global_env()->log_hd, "It shouldn't go here.\n");
}
void sys_exit(va_list ap)
{
    be_exit(0);
}