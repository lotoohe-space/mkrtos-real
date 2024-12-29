
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

RPC_GENERATION_CALL2(TRUE, pm_t, PM_PROT, PM_RUN_APP, run_app,
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

RPC_GENERATION_CALL2(TRUE, pm_t, PM_PROT, PM_KILL_TASK, kill_task,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, pid,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)

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
RPC_GENERATION_CALL3(TRUE, pm_t, PM_PROT, PM_WATCH_PID, watch_pid,
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
RPC_GENERATION_CALL5(TRUE, pm_t, PM_PROT, PM_COPY_DATA, copy_data,
                     rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, src_pid,
                     rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, dst_pid,
                     rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, src_addr,
                     rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, dst_addr,
                     rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
int pm_copy_data(pid_t src_pid, pid_t dst_pid, addr_t src_addr, addr_t dst_addr, size_t len)
{
    rpc_umword_t_t rpc_src_pid = {
        .data = src_pid,
    };
    rpc_umword_t_t rpc_dst_pid = {
        .data = dst_pid,
    };
    rpc_umword_t_t rpc_src_addr = {
        .data = src_addr,
    };
    rpc_umword_t_t rpc_dst_addr = {
        .data = dst_addr,
    };
    rpc_umword_t_t rpc_len = {
        .data = len,
    };
    msg_tag_t tag = pm_t_copy_data_call(u_get_global_env()->ns_hd, &rpc_src_pid, &rpc_dst_pid,
                                        &rpc_src_addr, &rpc_dst_addr, &rpc_len);

    return msg_tag_get_val(tag);
}