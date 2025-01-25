#include "rpc_prot.h"
#include "blk_drv_svr.h"
#include "blk_drv_types.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_arch.h"
#include "u_rpc_buf.h"
#include "u_hd_man.h"
#include "u_rpc_buf.h"
#include <stdio.h>

RPC_GENERATION_OP3(blk_drv_t, BLK_DRV_PROT, BLK_DRV_WRITE, write,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, inx)
{
    return blk_drv_write(vpage_create_raw(shm_obj->data).addr, len->data, inx->data);
}
RPC_GENERATION_DISPATCH3(blk_drv_t, BLK_DRV_PROT, BLK_DRV_WRITE, write,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, svr_hd,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, type,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, inx)

RPC_GENERATION_OP3(blk_drv_t, BLK_DRV_PROT, BLK_DRV_READ, read,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, inx)
{
    return blk_drv_read(vpage_create_raw(shm_obj->data).addr, len->data, inx->data);
}

RPC_GENERATION_DISPATCH3(blk_drv_t, BLK_DRV_PROT, BLK_DRV_READ, read,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, inx)

RPC_GENERATION_OP1(blk_drv_t, BLK_DRV_PROT, BLK_DRV_MAP, map,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_INOUT, RPC_TYPE_BUF, cli_hd)
{
    int ret = blk_drv_map(&cli_hd->data);
    if (ret < 0)
    {
        printf("blk drv aailed to request map");
    }
    return ret;
}
RPC_GENERATION_DISPATCH1(blk_drv_t, BLK_DRV_PROT, BLK_DRV_MAP, map,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_INOUT, RPC_TYPE_BUF, cli_hd)

/*info*/
RPC_GENERATION_OP1(blk_drv_t, BLK_DRV_PROT, BLK_DRV_INFO, info,
                   rpc_blk_drv_info_t_t, rpc_blk_drv_info_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, info)
{
    return blk_drv_info(&info->data);
}
RPC_GENERATION_DISPATCH1(blk_drv_t, BLK_DRV_PROT, BLK_DRV_INFO, info,
                         rpc_blk_drv_info_t_t, rpc_blk_drv_info_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, len)

RPC_DISPATCH4(blk_drv_t, BLK_DRV_PROT, typeof(BLK_DRV_READ), BLK_DRV_WRITE, write, BLK_DRV_READ, read, BLK_DRV_MAP, map, BLK_DRV_INFO, info)

void blk_drv_init(blk_drv_t *ns)
{
    rpc_svr_obj_init(&ns->svr, rpc_blk_drv_t_dispatch, BLK_DRV_PROT);
}
