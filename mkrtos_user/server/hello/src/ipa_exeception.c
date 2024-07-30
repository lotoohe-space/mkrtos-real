
#include <pthread.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <u_hd_man.h>
#include <u_factory.h>
#include <pthread.h>
#include <u_thread.h>
#include <u_task.h>
#include <printf.h>
#include "guest_os.h"
#include "u_sleep.h"
#include "aarch64_exp.h"

static void *ipc_execetion_thread_handler(void *arg)
{
    msg_tag_t tag;
    guest_os_t *gos = arg;
    ipc_msg_t *msg;
    int ret;

    printf("%s:%d\n", __func__, __LINE__);
    thread_msg_buf_get(pthread_hd_get(pthread_self()), (umword_t *)(&msg), NULL);
    msg->user[3] = (umword_t)(&gos->vcpu);
    while (1)
    {
        tag = thread_ipc_wait(ipc_timeout_create2(0, 0), NULL, -1);
        if (msg_tag_get_val(tag) < 0)
        {
            continue;
        }
        ret = aarch64_exp_sync((cpu_regs_t *)(msg->data + IPC_VPUC_MSG_OFFSET),
                               msg->msg_buf[0], msg->msg_buf[1]);
        tag = msg_tag_init4(0, 0, 0, ret);
        thread_ipc_reply(tag, ipc_timeout_create2(0, 0));
    }
    return NULL;
}

int ipc_execetion_create(guest_os_t *gos)
{
    int ret = 0;
    assert(gos);

    mk_printf("%s:%d \n", __func__, __LINE__);
    ret = pthread_create(&gos->ipa_exeception_hd, NULL, ipc_execetion_thread_handler, gos);
    if (ret)
    {
        return ret;
    }

    gos->ipa_execption_ipc = pthread_hd_get(gos->ipa_exeception_hd);
    task_set_obj_name(TASK_THIS, gos->ipa_execption_ipc, "exec_th");

    u_sleep_ms(100);
    return 0;
}
