#include "blk_drv_cli.h"
#include "blk_drv_types.h"
#include "rpc_prot.h"
#include "u_arch.h"
#include "u_hd_man.h"
#include "u_rpc.h"
#include "u_rpc_buf.h"
#include "u_rpc_svr.h"
#include <assert.h>
#include <stdio.h>
#include <u_env.h>

RPC_GENERATION_CALL3(blk_drv_t, BLK_DRV_PROT, BLK_DRV_WRITE, write,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, inx)
int blk_drv_cli_write(obj_handler_t dm9000_obj, obj_handler_t shm_obj, int len, int inx)
{
    rpc_obj_handler_t_t rpc_hd = {
        .data = shm_obj,
    };
    rpc_int_t rpc_len = {
        .data = len,
    };
    rpc_int_t rpc_inx = {
        .data = inx,
    };
    msg_tag_t tag = blk_drv_t_write_call(dm9000_obj, &rpc_hd, &rpc_len, &rpc_inx);

    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL3(blk_drv_t, BLK_DRV_PROT, BLK_DRV_READ, read,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, inx)

int blk_drv_cli_read(obj_handler_t dm9000_obj, obj_handler_t shm_obj, int len, int inx)
{
    rpc_obj_handler_t_t rpc_hd = {
        .data = shm_obj,
    };
    rpc_int_t rpc_len = {
        .data = len,
    };
    rpc_int_t rpc_inx = {
        .data = inx,
    };
    msg_tag_t tag = blk_drv_t_read_call(dm9000_obj, &rpc_hd, &rpc_len, &rpc_inx);

    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL1(blk_drv_t, BLK_DRV_PROT, BLK_DRV_MAP, map,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_INOUT, RPC_TYPE_BUF, cli_hd)

int blk_drv_cli_map(obj_handler_t dm9000_obj, obj_handler_t *sem_obj)
{
    assert(sem_obj);

    obj_handler_t newfd;

    newfd = handler_alloc();
    if (newfd == HANDLER_INVALID)
    {
        return -ENOENT;
    }
    rpc_obj_handler_t_t rpc_sem_obj = {
        .data = newfd,
        .del_map_flags = VPAGE_FLAGS_MAP,
    };
    msg_tag_t tag = blk_drv_t_map_call(dm9000_obj, &rpc_sem_obj);

    if (msg_tag_get_val(tag) < 0)
    {
        handler_free(newfd);
        return msg_tag_get_val(tag);
    }
    *sem_obj = newfd;
    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL1(blk_drv_t, BLK_DRV_PROT, BLK_DRV_INFO, info,
                     rpc_blk_drv_info_t_t, rpc_blk_drv_info_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, info)
int blk_drv_cli_info(obj_handler_t obj, blk_drv_info_t *info)
{
    assert(info);

    obj_handler_t newfd;

    newfd = handler_alloc();
    if (newfd == HANDLER_INVALID)
    {
        return -ENOENT;
    }
    rpc_blk_drv_info_t_t rpc_info;

    msg_tag_t tag = blk_drv_t_info_call(obj, &rpc_info);

    if (msg_tag_get_val(tag) < 0)
    {
        handler_free(newfd);
        return msg_tag_get_val(tag);
    }
    *info = rpc_info.data;
    return msg_tag_get_val(tag);
}