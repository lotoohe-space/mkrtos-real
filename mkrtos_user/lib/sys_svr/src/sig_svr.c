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
#include "sig_svr.h"
#include "rpc_prot.h"
#include "u_hd_man.h"
#include "u_rpc.h"
#include "u_rpc_buf.h"
#include "u_rpc_svr.h"
#include <stdio.h>

RPC_GENERATION_OP2(sig_t, SIG_PORT, SIG_KILL, kill,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, resv)
{
    if (!obj->op->kill)
    {
        return -ENOSYS;
    }
    return obj->op->kill(flags->data, resv->data);
}

RPC_GENERATION_DISPATCH2(sig_t, SIG_PORT, SIG_KILL, kill,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, resv)

RPC_GENERATION_OP2(sig_t, SIG_PORT, SIG_TOKILL, to_kill,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, resv)
{
    if (!obj->op->to_kill)
    {
        return -ENOSYS;
    }
    return obj->op->to_kill(flags->data, resv->data);
}

RPC_GENERATION_DISPATCH2(sig_t, SIG_PORT, SIG_TOKILL, to_kill,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, resv)

/*dispatch*/
RPC_DISPATCH2(sig_t, SIG_PORT, typeof(SIG_KILL), SIG_KILL, kill, SIG_TOKILL, to_kill)

void sig_svr_obj_init(sig_t *sig)
{
    rpc_svr_obj_init(&sig->svr_obj, rpc_sig_t_dispatch, SIG_PORT);
}
