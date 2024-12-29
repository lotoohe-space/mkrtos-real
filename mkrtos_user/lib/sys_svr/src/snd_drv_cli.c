#include "rpc_prot.h"
#include "snd_drv_cli.h"
#include "snd_drv_types.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_arch.h"
#include "u_rpc_buf.h"
#include "u_hd_man.h"
#include "u_rpc_buf.h"
#include <u_env.h>
#include <stdio.h>
#include <assert.h>

RPC_GENERATION_CALL2(TRUE, snd_drv_t, SND_DRV_PROT, SND_DRV_WRITE, write,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
int snd_drv_cli_write(obj_handler_t dm9000_obj, obj_handler_t shm_obj, int len)
{
    rpc_obj_handler_t_t rpc_hd = {
        .data = shm_obj,
    };
    rpc_int_t rpc_len = {
        .data = len,
    };

    msg_tag_t tag = snd_drv_t_write_call(dm9000_obj, &rpc_hd, &rpc_len);

    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL2(TRUE, snd_drv_t, SND_DRV_PROT, SND_DRV_READ, read,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, shm_obj,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

int snd_drv_cli_read(obj_handler_t dm9000_obj, obj_handler_t shm_obj)
{
    rpc_obj_handler_t_t rpc_hd = {
        .data = shm_obj,
    };
    rpc_int_t rpc_len = {
        .data = 0,
    };
    msg_tag_t tag = snd_drv_t_read_call(dm9000_obj, &rpc_hd, &rpc_len);

    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL1(TRUE, snd_drv_t, SND_DRV_PROT, SND_DRV_MAP, map,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_INOUT, RPC_TYPE_BUF, cli_hd)

int snd_drv_cli_map(obj_handler_t dm9000_obj, obj_handler_t *sem_obj)
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
    msg_tag_t tag = snd_drv_t_map_call(dm9000_obj, &rpc_sem_obj);

    if (msg_tag_get_val(tag) < 0)
    {
        handler_free(newfd);
        return msg_tag_get_val(tag);
    }
    *sem_obj = newfd;
    return msg_tag_get_val(tag);
}
