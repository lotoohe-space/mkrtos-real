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
#include <stdio.h>

/*run_app*/
RPC_GENERATION_OP2(pm_t, PM_PROT, PM_RUN_APP, run_app,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
{
    int16_t ret = -1;

    path->data[path->len - 1] = 0;
    ret = pm_rpc_run_app(path->data, flags->data);
    return ret;
}

RPC_GENERATION_DISPATCH2(pm_t, PM_PROT, PM_RUN_APP, run_app,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)

/*dispatch*/
RPC_DISPATCH1(pm_t, PM_PROT, typeof(PM_RUN_APP), FS_OPEN, run_app)

void pm_svr_obj_init(pm_t *pm)
{
    rpc_svr_obj_init(&pm->svr_obj, rpc_pm_t_dispatch, PM_PROT);
}
