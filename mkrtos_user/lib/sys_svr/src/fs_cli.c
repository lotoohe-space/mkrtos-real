#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "ns_types.h"
#include "rpc_prot.h"
#include "u_env.h"
#include "u_prot.h"
#include "u_hd_man.h"
#include "ns_cli.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

RPC_GENERATION_CALL3(fs_t, FS_PROT, FS_OPEN, open,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, mode)
sd_t fs_open(const char *path, int flags, int mode)
{
    obj_handler_t hd;
    int ret = ns_query(path, &hd);

    if (ret < 0)
    {
        return ret;
    }

    rpc_ref_array_uint32_t_uint8_t_32_t rpc_path = {
        .data = (uint8_t *)(&path[ret]),
        .len = strlen(&path[ret]) + 1,
    };
    rpc_int_t rpc_flags = {
        .data = flags,
    };
    rpc_int_t rpc_mode = {
        .data = mode,
    };
    msg_tag_t tag = fs_t_open_call(hd, &rpc_path, &rpc_flags, &rpc_mode);

    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }

    return mk_sd_init2(hd, msg_tag_get_val(tag)).raw;
}
RPC_GENERATION_CALL3(fs_t, FS_PROT, FS_READ, read,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

int fs_read(sd_t _fd, void *buf, size_t len)
{
    obj_handler_t hd = mk_sd_init_raw(_fd).hd;
    int fd = mk_sd_init_raw(_fd).fd;

    rpc_int_t rpc_fd = {
        .data = fd,
    };

    int rlen = 0;
    while (rlen < len)
    {
        int r_once_len = 0;

        r_once_len = MIN(32, len - rlen);
        rpc_ref_array_uint32_t_uint8_t_32_t rpc_buf = {
            .data = buf + rlen,
            .len = r_once_len,
        };
        rpc_int_t rpc_len = {
            .data = r_once_len,
        };
        msg_tag_t tag = fs_t_read_call(hd, &rpc_fd, &rpc_buf, &rpc_len);

        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        rlen += msg_tag_get_val(tag);
        if (msg_tag_get_val(tag) != r_once_len)
        {
            break;
        }
    }

    return rlen;
}
RPC_GENERATION_CALL3(fs_t, FS_PROT, FS_WRITE, write,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, buf,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

int fs_write(sd_t _fd, void *buf, size_t len)
{
    obj_handler_t hd = mk_sd_init_raw(_fd).hd;
    int fd = mk_sd_init_raw(_fd).fd;

    rpc_int_t rpc_fd = {
        .data = fd,
    };
    int wlen = 0;
    while (wlen < len)
    {
        int w_once_len = 0;

        w_once_len = MIN(32, len - wlen);
        rpc_ref_array_uint32_t_uint8_t_32_t rpc_buf = {
            .data = buf + wlen,
            .len = w_once_len,
        };
        rpc_int_t rpc_len = {
            .data = w_once_len,
        };
        msg_tag_t tag = fs_t_write_call(hd, &rpc_fd, &rpc_buf, &rpc_len);

        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        wlen += msg_tag_get_val(tag);
        if (msg_tag_get_val(tag) != w_once_len)
        {
            break;
        }
    }

    return wlen;
}
RPC_GENERATION_CALL1(fs_t, FS_PROT, FS_CLOSE, close,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd)
int fs_close(sd_t _fd)
{
    obj_handler_t hd = mk_sd_init_raw(_fd).hd;
    int fd = mk_sd_init_raw(_fd).fd;

    rpc_int_t rpc_fd = {
        .data = fd,
    };
    msg_tag_t tag = fs_t_close_call(hd, &rpc_fd);

    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }

    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL3(fs_t, FS_PROT, FS_LSEEK, lseek,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, offs,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, whence)

int fs_lseek(sd_t _fd, int offs, int whence)
{
    obj_handler_t hd = mk_sd_init_raw(_fd).hd;
    int fd = mk_sd_init_raw(_fd).fd;

    rpc_int_t rpc_fd = {
        .data = fd,
    };
    rpc_int_t rpc_offs = {
        .data = offs,
    };
    rpc_int_t rpc_whence = {
        .data = whence,
    };
    msg_tag_t tag = fs_t_lseek_call(hd, &rpc_fd, &rpc_offs, &rpc_whence);

    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }

    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL2(fs_t, FS_PROT, FS_READDIR, readdir,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                     rpc_dirent_t_t, rpc_dirent_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, dir)

int fs_readdir(sd_t _fd, dirent_t *dirent)
{
    obj_handler_t hd = mk_sd_init_raw(_fd).hd;
    int fd = mk_sd_init_raw(_fd).fd;

    rpc_int_t rpc_fd = {
        .data = fd,
    };
    rpc_dirent_t_t rpc_dient = {
        .data.d_ino = 0,
    };
    msg_tag_t tag = fs_t_readdir_call(hd, &rpc_fd, &rpc_dient);

    if (msg_tag_get_val(tag) >= 0)
    {
        if (dirent)
        {
            *dirent = rpc_dient.data;
        }
    }

    return msg_tag_get_val(tag);
}
