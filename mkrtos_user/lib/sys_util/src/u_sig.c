
#include <u_thread_util.h>
#include <u_util.h>
#include <u_rpc_svr.h>
#include <u_factory.h>
#include <u_sleep.h>
#include <u_sig.h>
#include <pm_cli.h>
#include <rpc_prot.h>
#include <assert.h>

static sig_call_back sig_cb_func;
static ATTR_ALIGN(8) uint8_t sig_stack[1024];
static uint8_t sig_msg_buf[MSG_BUG_LEN];
static obj_handler_t sig_th;
static obj_handler_t sig_ipc;
static uint8_t sig_init_flags;

int pm_sig_watch(pid_t pid, int flags)
{
    int ret = pm_watch_pid(sig_ipc, pid, flags);

    return ret;
}

void pm_sig_func_set(sig_call_back sig_func)
{
    sig_cb_func = sig_func;
}

/**
 * @brief 信号的回调函数
 *
 */
static void sig_func(void)
{
    assert(rpc_creaite_bind_ipc(sig_th, NULL, &sig_ipc) >= 0);

    while (1)
    {
        msg_tag_t tag = thread_ipc_wait(ipc_timeout_create2(0, 0), NULL);
        if (msg_tag_get_val(tag) < 0)
        {
            continue;
        }
        int ret = 0;
        ipc_msg_t *ipc = (ipc_msg_t *)sig_msg_buf;

        switch (ipc->msg_buf[0])
        {
        case PM_SIG_NOTIFY:
            if (sig_cb_func)
            {
                ret = sig_cb_func(ipc->msg_buf[2], ipc->msg_buf[1]);
                tag = msg_tag_init4(0, 0, 0, ret);
            }
            break;
        }
        thread_ipc_reply(tag, ipc_timeout_create2(0, 0));
    }
    while (1)
    {
        u_sleep_ms(1000);
    }
}
void sig_init(void)
{
    if (sig_init_flags)
    {
        return;
    }
    u_thread_create(&sig_th, sig_stack, sizeof(sig_stack), sig_msg_buf, sig_func);
    u_thread_run(sig_th, 3);
}
