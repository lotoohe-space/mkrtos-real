#pragma once

#include "pm_cli.h"
#include "u_rpc_svr.h"
#include "u_slist.h"
#include "u_types.h"

typedef struct sig_op
{
    int (*kill)(int flags, int resv);
    int (*to_kill)(int flags, int resv);
} sig_op_t;

typedef struct sig
{
    rpc_svr_obj_t svr_obj;
    const sig_op_t *op;
} sig_t;

void sig_svr_obj_init(sig_t *sig);
