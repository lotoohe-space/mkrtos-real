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

RPC_GENERATION_CALL2(sig_t, SIG_PORT, SIG_KILL, kill,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, resv)
int sig_kill(obj_handler_t sig_hd, int flags, int pid)
{
    rpc_int_t rpc_flags = {
        .data = flags,
    };

    rpc_int_t rpc_pid = {
        .data = pid,
    };

    msg_tag_t tag = sig_t_kill_call(sig_hd, &rpc_flags, &rpc_pid);

    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL2(sig_t, SIG_PORT, SIG_TOKILL, to_kill,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, resv)

int sig_to_kill(obj_handler_t sig_hd, int flags, int resv)
{

    rpc_int_t rpc_flags = {
        .data = flags,
    };

    rpc_int_t rpc_resv = {
        .data = resv,
    };

    msg_tag_t tag = sig_t_to_kill_call(sig_hd, &rpc_flags, &rpc_resv);

    return msg_tag_get_val(tag);
}
