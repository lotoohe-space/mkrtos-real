#include "rpc_prot.h"
#include "snd_drv_svr.h"
#include "snd_drv_types.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_arch.h"
#include "u_rpc_buf.h"
#include "u_hd_man.h"
#include "u_rpc_buf.h"
#include <stdio.h>

RPC_GENERATION_OP2(snd_drv_t, SND_DRV_PROT, SND_DRV_WRITE, write,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
{
    return snd_drv_write(rpc_hd_get(0), len->data);
}
RPC_GENERATION_DISPATCH2(snd_drv_t, SND_DRV_PROT, SND_DRV_WRITE, write,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, svr_hd,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, type)

RPC_GENERATION_OP2(snd_drv_t, SND_DRV_PROT, SND_DRV_READ, read,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
{
    return snd_drv_read(rpc_hd_get(0), len->data);
}

RPC_GENERATION_DISPATCH2(snd_drv_t, SND_DRV_PROT, SND_DRV_READ, read,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

RPC_GENERATION_OP1(snd_drv_t, SND_DRV_PROT, SND_DRV_MAP, map,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_INOUT, RPC_TYPE_BUF, cli_hd)
{
    int ret = snd_drv_map(&cli_hd->data);
    if (ret < 0)
    {
        printf("snd drv aailed to request map");
    }
    return ret;
}
RPC_GENERATION_DISPATCH1(snd_drv_t, SND_DRV_PROT, SND_DRV_MAP, map,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_INOUT, RPC_TYPE_BUF, cli_hd)

RPC_DISPATCH3(snd_drv_t, SND_DRV_PROT, typeof(SND_DRV_READ), SND_DRV_WRITE, write, SND_DRV_READ, read, SND_DRV_MAP, map)

void snd_drv_init(snd_drv_t *ns)
{
    rpc_svr_obj_init(&ns->svr, rpc_snd_drv_t_dispatch, SND_DRV_PROT);
}
