#include "u_ipc.h"
#include "u_types.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_hd_man.h"
#include "u_log.h"
#include "pm_cli.h"
#include "u_sig.h"
#include "futex_queue.h"
#include "u_ipc.h"
#include "u_sys.h"
#include "u_futex.h"
#include <assert.h>
#include <limits.h>
#include "u_sleep.h"
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <syscall_backend.h>
#undef hidden
#undef weak
#define hidden
#include <features.h>
#include <pthread_impl.h>
#include <futex.h>
#include <atomic.h>
#define FUTEX_WAKE_CLEAR 10

void be_exit(long exit_code)
{
    struct pthread *pt = __pthread_self();
    int th_hd = pt->hd;
    int *old_ctid = (int *)(pt->ctid);

    if (th_hd != THREAD_MAIN)
    {
        if (old_ctid)
        {
            be_futex(old_ctid, FUTEX_WAKE_CLEAR, 1, 0, 0, 0);
        }
        if (pthread_cnt_dec() == 1)
        {
            /*删除整个进程*/
            goto del_task;
        }
        else
        {
            task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th_hd));
        }
    }
    else
    {
        umword_t pid;
    del_task:
        /*TODO:删除其它东西*/

        task_get_pid(TASK_THIS, &pid);
        pm_kill_task(pid, KILL_SIG, exit_code);
        task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, TASK_THIS)); //!< 删除当前task，以及申请得所有对象
        a_crash();                                                                 //!< 强制退出
    }
    while (1)
    {
        // thread_ipc_wait(ipc_timeout_create2(0, 0), NULL, -1);
        u_sleep_ms(U_SLEEP_ALWAYS);
    }
}

long sys_exit(va_list ap)
{
    long exit_code;

    ARG_1_BE(ap, exit_code, long);
    be_exit(exit_code);
    return 0;
}
