#include "rpc_prot.h"
#include "net_drv_cli.h"
#include "net_drv_types.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_arch.h"
#include "u_rpc_buf.h"
#include "u_hd_man.h"
#include "u_rpc_buf.h"
#include <u_env.h>
#include <stdio.h>

RPC_GENERATION_CALL2(net_drv_t, NET_DRV_PROT, NET_DRV_WRITE, write,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

RPC_GENERATION_CALL2(net_drv_t, NET_DRV_PROT, NET_DRV_READ, read,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

int net_drv_cli_write(obj_handler_t dm9000_obj, obj_handler_t shm_obj, int len)
{
    rpc_obj_handler_t_t rpc_hd = {
        .data = shm_obj,
    };
    rpc_int_t rpc_len = {
        .data = len,
    };

    msg_tag_t tag = net_drv_t_write_call(dm9000_obj, &rpc_hd, &rpc_len);

    return msg_tag_get_val(tag);
}
int net_drv_cli_read(obj_handler_t dm9000_obj, obj_handler_t shm_obj)
{
    rpc_obj_handler_t_t rpc_hd = {
        .data = shm_obj,
    };
    rpc_int_t rpc_len = {
        .data = 0,
    };
    msg_tag_t tag = net_drv_t_read_call(dm9000_obj, &rpc_hd, &rpc_len);

    return msg_tag_get_val(tag);
}
