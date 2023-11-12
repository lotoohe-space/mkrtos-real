#pragma once
#include "u_types.h"
#include "u_rpc_svr.h"

typedef struct drv
{
    rpc_svr_obj_t svr;
    obj_handler_t ipc;
} drv_t;
