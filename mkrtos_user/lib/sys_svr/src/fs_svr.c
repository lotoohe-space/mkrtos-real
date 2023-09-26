#include "rpc_prot.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_hd_man.h"
#include "fs_svr.h"
#include <stdio.h>

/*open*/
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
/*read*/
RPC_GENERATION_OP3(fs_t, FS_READ, read,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
{
    int ret = fs_svr_read(fd->data, buf->data, len->data);

    if (ret >= 0)
    {
        buf->len = ret;
    }
    return ret;
}

RPC_GENERATION_DISPATCH3(fs_t, FS_READ, read,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

/*write*/
RPC_GENERATION_OP3(fs_t, FS_WRITE, write,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, buf,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
{
    int ret = fs_svr_write(fd->data, buf->data, len->data);
    return ret;
}

RPC_GENERATION_DISPATCH3(fs_t, FS_WRITE, write,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, buf,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

/*close*/
RPC_GENERATION_OP1(fs_t, FS_CLOSE, close,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd)
{
    fs_svr_close(fd->data);
    return 0;
}

RPC_GENERATION_DISPATCH1(fs_t, FS_CLOSE, close,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd)

/*lseek*/
RPC_GENERATION_OP3(fs_t, FS_LSEEK, lseek,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, offs,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, whence)
{
    int ret = fs_svr_lseek(fd->data, offs->data, whence->data);
    return ret;
}

RPC_GENERATION_DISPATCH3(fs_t, FS_LSEEK, lseek,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, whence)

/*dispatch*/
RPC_DISPATCH5(fs_t, typeof(FS_OPEN), FS_OPEN, open, FS_READ, read,
              FS_WRITE, write, FS_CLOSE, close, FS_LSEEK, lseek)

void fs_init(fs_t *fs)
{
    rpc_svr_obj_init(&fs->svr, rpc_fs_t_dispatch, FS_PROT);
}
