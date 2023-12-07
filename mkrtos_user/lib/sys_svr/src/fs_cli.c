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

/*open*/
// RPC_GENERATION_CALL3(fs_t, FS_PROT, FS_OPEN, open,
//                      rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
//                      rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
//                      rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, mode)
msg_tag_t fs_t_open_call(obj_handler_t hd, rpc_ref_array_uint32_t_uint8_t_32_t *var0, rpc_int_t *var1, rpc_int_t *var2)
{
    void *buf;
    ipc_msg_t *msg_ipc;
    thread_msg_buf_get(2, (umword_t *)(&buf), ((void *)0));
    msg_ipc = (ipc_msg_t *)buf;
    int off = 0;
    int off_buf = 0;
    int ret = -1;
    size_t op_val = ((uint16_t)0);
    rpc_memcpy(msg_ipc->msg_buf, &op_val, sizeof(op_val));
    off += rpc_align(sizeof(op_val), __alignof(((uint16_t)0)));
    do
    {
        if (1 == 1)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_ref_array_uint32_t_uint8_t_32_t(var0, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 2)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_ref_array_uint32_t_uint8_t_32_t(var0, (uint8_t *)((uint8_t *)msg_ipc->map_buf), off_buf);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_buf = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 1)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_int_t(var1, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 2)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_int_t(var1, (uint8_t *)((uint8_t *)msg_ipc->map_buf), off_buf);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_buf = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 1)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_int_t(var2, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 2)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_int_t(var2, (uint8_t *)((uint8_t *)msg_ipc->map_buf), off_buf);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_buf = ret;
            }
        }
    } while (0);
    msg_tag_t tag = thread_ipc_call(((msg_tag_t){.flags = (0), .msg_buf_len = ((((off) / ((sizeof(void *)))) + (((off) % ((sizeof(void *)))) ? 1 : 0))), .map_buf_len = ((((off_buf) / ((sizeof(void *)))) + (((off_buf) % ((sizeof(void *)))) ? 1 : 0))), .prot = (0x0002)}), hd, ipc_timeout_create2(0, 0));
    if (((int16_t)((tag).prot)) < 0)
    {
        return tag;
    }
    off = 0;
    do
    {
        if (1 == 1)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_cli_buf_to_msg_rpc_ref_array_uint32_t_uint8_t_32_t(var0, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 1)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_cli_buf_to_msg_rpc_int_t(var1, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 1)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_cli_buf_to_msg_rpc_int_t(var2, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    return tag;
}
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
/*read*/
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
/*write*/
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
/*close*/
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
/*lseek*/
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
/*readdir*/
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
