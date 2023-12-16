
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "rpc_prot.h"
#include "u_env.h"
#include "u_prot.h"
#include "u_hd_man.h"
#include "pm_svr.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

RPC_GENERATION_CALL2(pm_t, PM_PROT, PM_RUN_APP, run_app,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
int pm_run_app(const char *path, int flags)
{
    rpc_ref_array_uint32_t_uint8_t_32_t rpc_path = {
        .data = (uint8_t *)path,
        .len = strlen(path) + 1,
    };
    rpc_int_t rpc_flags = {
        .data = flags,
    };
    msg_tag_t tag = pm_t_run_app_call(u_get_global_env()->ns_hd, &rpc_path, &rpc_flags);

    return msg_tag_get_val(tag);
}

// RPC_GENERATION_CALL2(pm_t, PM_PROT, PM_KILL_TASK, kill_task,
//                      rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, pid,
//                      rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
msg_tag_t pm_t_kill_task_call(obj_handler_t hd, rpc_int_t *var0, rpc_int_t *var1)
{
    void *buf;
    ipc_msg_t *msg_ipc;
    thread_msg_buf_get(2, (umword_t *)(&buf), ((void *)0));
    msg_ipc = (ipc_msg_t *)buf;
    int off = 0;
    int off_buf = 0;
    int ret = -1;
    size_t op_val = ((uint16_t)1);
    rpc_memcpy(msg_ipc->msg_buf, &op_val, sizeof(op_val));
    off += rpc_align(sizeof(op_val), __alignof(((uint16_t)1)));
    do
    {
        if (1 == 1)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_int_t(var0, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 2)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_int_t(var0, (uint8_t *)((uint8_t *)msg_ipc->map_buf), off_buf);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_buf = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 1)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_int_t(var1, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 2)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_int_t(var1, (uint8_t *)((uint8_t *)msg_ipc->map_buf), off_buf);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_buf = ret;
            }
        }
    } while (0);
    msg_tag_t tag = thread_ipc_call(((msg_tag_t){.flags = (0), .msg_buf_len = ((((off) / ((sizeof(void *)))) + (((off) % ((sizeof(void *)))) ? 1 : 0))), .map_buf_len = ((((off_buf) / ((sizeof(void *)))) + (((off_buf) % ((sizeof(void *)))) ? 1 : 0))), .prot = (0x0005)}), hd, ipc_timeout_create2(0, 0));
    if (((int16_t)((tag).prot)) < 0)
    {
        return tag;
    }
    off = 0;
    do
    {
        if (1 == 1)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_cli_buf_to_msg_rpc_int_t(var0, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 1)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_cli_buf_to_msg_rpc_int_t(var1, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    return tag;
}
int pm_kill_task(int pid, int flags)
{
    rpc_int_t rpc_pid = {
        .data = pid,
    };
    rpc_int_t rpc_flags = {
        .data = flags,
    };
    msg_tag_t tag = pm_t_kill_task_call(u_get_global_env()->ns_hd, &rpc_pid, &rpc_flags);

    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL3(pm_t, PM_PROT, PM_WATCH_PID, watch_pid,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, sig_hd,
                     rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, pid,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
int pm_watch_pid(obj_handler_t sig_hd, pid_t pid, int flags)
{
    rpc_obj_handler_t_t rpc_sig_hd = {
        .data = sig_hd,
    };
    rpc_umword_t_t rpc_pid = {
        .data = pid,
    };
    rpc_int_t rpc_flags = {
        .data = flags,
    };

    msg_tag_t tag = pm_t_watch_pid_call(u_get_global_env()->ns_hd, &rpc_sig_hd, &rpc_pid, &rpc_flags);

    return msg_tag_get_val(tag);
}