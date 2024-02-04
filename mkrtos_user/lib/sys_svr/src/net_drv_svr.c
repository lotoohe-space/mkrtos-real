#include "rpc_prot.h"
#include "net_drv_svr.h"
#include "net_drv_types.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_arch.h"
#include "u_rpc_buf.h"
#include "u_hd_man.h"
#include "u_rpc_buf.h"
#include <stdio.h>

RPC_GENERATION_OP2(net_drv_t, NET_DRV_PROT, NET_DRV_WRITE, write,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
{
    return net_drv_write(rpc_hd_get(0), len->data);
}
RPC_GENERATION_DISPATCH2(net_drv_t, NET_DRV_PROT, NET_DRV_WRITE, write,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, svr_hd,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, type)

RPC_GENERATION_OP2(net_drv_t, NET_DRV_PROT, NET_DRV_READ, read,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
{
    return net_drv_read(rpc_hd_get(0), len->data);
}

RPC_GENERATION_DISPATCH2(net_drv_t, NET_DRV_PROT, NET_DRV_READ, read,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
RPC_DISPATCH2(net_drv_t, NET_DRV_PROT, typeof(NET_DRV_READ), NET_DRV_WRITE, write, NET_DRV_READ, read)

void net_drv_init(net_drv_t *ns)
{
    rpc_svr_obj_init(&ns->svr, rpc_net_drv_t_dispatch, NET_DRV_PROT);
}
