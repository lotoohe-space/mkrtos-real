/**
 * @file pm_svr.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-11-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "rpc_prot.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_hd_man.h"
#include "fs_svr.h"
#include "pm_svr.h"
#include "u_rpc_buf.h"
#include "fs_types.h"
#include <stdio.h>

/**
 * @brief 运行app
 * run_app
 */
RPC_GENERATION_OP6(pm_t, PM_PROT, PM_RUN_APP, run_app,
                   rpc_ref_array_uint32_t_uint8_t_64_t, rpc_array_uint32_t_uint8_t_64_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, mem_block,
                   rpc_ref_array_uint32_t_uint8_t_96_t, rpc_array_uint32_t_uint8_t_96_t, RPC_DIR_IN, RPC_TYPE_DATA, params,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, params_len,
                   rpc_ref_array_uint32_t_uint8_t_64_t, rpc_array_uint32_t_uint8_t_64_t, RPC_DIR_IN, RPC_TYPE_DATA, envs,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, envs_len
                   )
{
    int16_t ret = -1;

    path->data[path->len - 1] = 0;
    ret = pm_rpc_run_app(path->data, mem_block->data, params->data, params_len->data, envs->data, envs_len->data);
    return ret;
}

RPC_GENERATION_DISPATCH6(pm_t, PM_PROT, PM_RUN_APP, run_app,
                   rpc_ref_array_uint32_t_uint8_t_64_t, rpc_array_uint32_t_uint8_t_64_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, mem_block,
                   rpc_ref_array_uint32_t_uint8_t_96_t, rpc_array_uint32_t_uint8_t_96_t, RPC_DIR_IN, RPC_TYPE_DATA, params,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, params_len,
                   rpc_ref_array_uint32_t_uint8_t_64_t, rpc_array_uint32_t_uint8_t_64_t, RPC_DIR_IN, RPC_TYPE_DATA, envs,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, envs_len)

/*kill_task*/
RPC_GENERATION_OP3(pm_t, PM_PROT, PM_KILL_TASK, kill_task,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, pid,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, exit_code)
{
    int16_t ret = 0;
    ret = pm_rpc_kill_task(thread_get_src_pid(), pid->data, flags->data, exit_code->data);
    return ret;
}

RPC_GENERATION_DISPATCH3(pm_t, PM_PROT, PM_KILL_TASK, kill_task,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, pid,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, exit_code)
/*watch pid*/
RPC_GENERATION_OP3(pm_t, PM_PROT, PM_WATCH_PID, watch_pid,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, sig_hd,
                   rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, pid,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
{
    int16_t ret = 0;

    ret = pm_rpc_watch_pid(obj, vpage_create_raw(sig_hd->data).addr, pid->data, flags->data);
    return ret;
}

RPC_GENERATION_DISPATCH3(pm_t, PM_PROT, PM_WATCH_PID, watch_pid,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, sig_hd,
                         rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, pid,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
/*PM_COPY_TO_DATA*/
RPC_GENERATION_OP5(pm_t, PM_PROT, PM_COPY_DATA, copy_data,
                   rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, src_pid,
                   rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, dst_pid,
                   rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, src_addr,
                   rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, dst_addr,
                   rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
{
    int16_t ret = 0;

    ret = pm_rpc_copy_data(src_pid->data, dst_pid->data, src_addr->data, dst_addr->data, len->data);
    return ret;
}

RPC_GENERATION_DISPATCH5(pm_t, PM_PROT, PM_COPY_DATA, copy_data,
                         rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, src_pid,
                         rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, dst_pid,
                         rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, src_addr,
                         rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, dst_addr,
                         rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

RPC_GENERATION_OP2(pm_t, PM_PROT, PM_WATCH_PID, del_watch_pid,
                   rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, pid,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
{
    int16_t ret = 0;

    ret = pm_rpc_del_watch_pid(obj, pid->data, flags->data);
    return ret;
}
RPC_GENERATION_DISPATCH2(pm_t, PM_PROT, PM_WATCH_PID, del_watch_pid,
                         rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, pid,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
/*dispatch*/
RPC_DISPATCH4(pm_t, PM_PROT, typeof(PM_RUN_APP),
              PM_RUN_APP, run_app,
              PM_KILL_TASK, kill_task,
              PM_WATCH_PID, watch_pid,
              PM_COPY_DATA, copy_data)

void pm_svr_obj_init(pm_t *pm)
{
    rpc_svr_obj_init(&pm->svr_obj, rpc_pm_t_dispatch, PM_PROT);
    slist_init(&pm->watch_head);
}
