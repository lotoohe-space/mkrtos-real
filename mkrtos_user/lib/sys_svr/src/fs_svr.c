#include "rpc_prot.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_hd_man.h"
#include "fs_svr.h"
#include <stdio.h>

RPC_GENERATION_OP3(fs_t, FS_OPEN, open,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, mode)
{
    path->data[path->len - 1] = 0;
    int ret = fs_svr_open((char *)(path->data), flags->data, mode->data);
    return ret;
}

RPC_GENERATION_DISPATCH3(fs_t, FS_OPEN, open,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, mode)

RPC_DISPATCH1(fs_t, typeof(FS_OPEN), FS_OPEN, open)

void fs_init(fs_t *fs)
{
    rpc_svr_obj_init(&fs->svr, rpc_fs_t_dispatch, FS_PROT);
}
