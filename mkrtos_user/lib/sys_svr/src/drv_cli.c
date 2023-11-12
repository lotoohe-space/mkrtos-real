#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "drv_types.h"
#include "rpc_prot.h"
#include "u_env.h"
#include "u_prot.h"
#include "u_hd_man.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

RPC_GENERATION_CALL2(drv_t, DRV_OPEN, open,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, name,
                     rpc_uint32_t_t, rpc_uint32_t_t, RPC_DIR_IN, RPC_TYPE_DATA, oflags)
RPC_GENERATION_CALL1(drv_t, DRV_CLOSE, close,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, desc)
RPC_GENERATION_CALL3(drv_t, DRV_READ, read,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, desc,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_OUT, RPC_TYPE_DATA, data,
                     rpc_uint32_t_t, rpc_uint32_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size)
RPC_GENERATION_CALL3(drv_t, DRV_WRITE, write,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, desc,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, data,
                     rpc_uint32_t_t, rpc_uint32_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size)
RPC_GENERATION_CALL3(drv_t, DRV_IOCTL, ioctl,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, desc,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, data,
                     rpc_uint32_t_t, rpc_uint32_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size)

int dev_fs_open(const char *path, uint32_t oflags)
{
    obj_handler_t hd;
    int ret = ns_query(path, &hd);

    if (ret < 0)
    {
        return ret;
    }

    rpc_ref_array_uint32_t_uint8_t_32_t rpc_path = {
        .data = &path[ret],
        .len = strlen(&path[ret]) + 1,
    };
    rpc_uint32_t_t rpc_flags = {
        .data = oflags,
    };
    msg_tag_t tag = drv_t_open_call(hd, &rpc_path, &rpc_flags);

    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }

    return mk_sd_init2(hd, msg_tag_get_val(tag)).raw;
}
int dev_fs_close(int desc)
{
    obj_handler_t hd;
    int fd;

    hd = mk_sd_get_hd(mk_sd_init_raw(desc));
    fd = mk_sd_get_fd(mk_sd_init_raw(desc));

    rpc_int_t rpc_desc = {
        .data = fd,
    };
    msg_tag_t tag = drv_t_close_call(hd, &rpc_desc);

    return msg_tag_get_val(tag);
}
int dev_fs_read(int desc, void *buf, size_t size)
{
    obj_handler_t hd;
    int fd;

    hd = mk_sd_get_hd(mk_sd_init_raw(desc));
    fd = mk_sd_get_fd(mk_sd_init_raw(desc));

    rpc_int_t rpc_desc = {
        .data = fd,
    };
    rpc_ref_array_uint32_t_uint8_t_32_t rpc_data = {
        .data = buf,
        .len = size,
    };
    rpc_size_t_t rpc_flags = {
        .data = size,
    };
    msg_tag_t tag = drv_t_read_call(hd, &rpc_desc, &rpc_data, &rpc_flags);

    return msg_tag_get_val(tag);
}
int dev_fs_write(int desc, const void *buf, size_t size)
{
    obj_handler_t hd;
    int fd;

    hd = mk_sd_get_hd(mk_sd_init_raw(desc));
    fd = mk_sd_get_fd(mk_sd_init_raw(desc));

    rpc_int_t rpc_desc = {
        .data = fd,
    };
    rpc_ref_array_uint32_t_uint8_t_32_t rpc_data = {
        .data = buf,
        .len = size,
    };
    rpc_size_t_t rpc_flags = {
        .data = size,
    };
    msg_tag_t tag = drv_t_write_call(hd, &rpc_desc, &rpc_data, &rpc_flags);

    return msg_tag_get_val(tag);
}
int dev_fs_ioctl(int desc, int cmd, void *args)
{
    obj_handler_t hd;
    int fd;

    hd = mk_sd_get_hd(mk_sd_init_raw(desc));
    fd = mk_sd_get_fd(mk_sd_init_raw(desc));

    rpc_int_t rpc_desc = {
        .data = fd,
    };
    rpc_int_t rpc_cmd = {
        .data = fd,
    };
    rpc_umword_t_t rpc_args = {
        .data = (umword_t)args,
    };
    msg_tag_t tag = drv_t_ioctl_call(hd, &rpc_desc, &rpc_cmd, &rpc_args);

    return msg_tag_get_val(tag);
}
