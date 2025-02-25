#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "ns_types.h"
#include "rpc_prot.h"
#include "u_env.h"
#include "u_prot.h"
#include "u_hd_man.h"
#include "ns_cli.h"
#include "u_rpc.h"
#include "fs_types.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include "kstat.h"
typedef struct kstat kstat_t;
RPC_TYPE_DEF_ALL(kstat_t)
/*open*/
RPC_GENERATION_CALL3(fs_t, FS_PROT, FS_OPEN, open,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, mode)
sd_t fs_open(const char *path, int flags, int mode)
{
    assert(path);
    obj_handler_t hd;
    int ret = ns_query(path, &hd, 0x1);

    if (ret < 0)
    {
        return ret;
    }

    rpc_ref_file_array_t rpc_path = {
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
/*read*/
RPC_GENERATION_CALL3(fs_t, FS_PROT, FS_READ, read,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf,
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

        r_once_len = MIN(FS_RPC_BUF_LEN, len - rlen);
        rpc_ref_file_array_t rpc_buf = {
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
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, buf,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

int fs_write(sd_t _fd, void *buf, size_t len)
{
    obj_handler_t hd = mk_sd_init_raw(_fd).hd;
    int fd = mk_sd_init_raw(_fd).fd;

    rpc_int_t rpc_fd = {
        .data = fd,
    };
    int wlen = 0;
    while ((wlen < len) || len == 0)
    {
        int w_once_len = 0;

        w_once_len = MIN(FS_RPC_BUF_LEN, len - wlen);
        rpc_ref_file_array_t rpc_buf = {
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
        if (len == 0)
        {
            break;
        }
    }

    return wlen;
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
/*ftruncate*/
RPC_GENERATION_CALL2(fs_t, FS_PROT, FS_FTRUNCATE, ftruncate,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                     rpc_int64_t_t, rpc_int64_t_t, RPC_DIR_IN, RPC_TYPE_DATA, offs)
int fs_ftruncate(sd_t _fd, off_t off)
{
    obj_handler_t hd = mk_sd_init_raw(_fd).hd;
    int fd = mk_sd_init_raw(_fd).fd;

    rpc_int_t rpc_fd = {
        .data = fd,
    };
    rpc_int64_t_t rpc_offs = {
        .data = off,
    };
    msg_tag_t tag = fs_t_ftruncate_call(hd, &rpc_fd, &rpc_offs);

    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }

    return msg_tag_get_val(tag);
}
/*fstat*/
// int fstat(int fd, struct stat *statbuf);
RPC_GENERATION_CALL2(fs_t, FS_PROT, FS_FSTAT, fstat,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                     rpc_kstat_t_t, rpc_kstat_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, statbuf)
int fs_fstat(sd_t _fd, kstat_t *stat)
{
    obj_handler_t hd = mk_sd_init_raw(_fd).hd;
    int fd = mk_sd_init_raw(_fd).fd;

    rpc_int_t rpc_fd = {
        .data = fd,
    };
    rpc_kstat_t_t rpc_statbuf;
    msg_tag_t tag;

    if (!stat)
    {
        return -EINVAL;
    }
    tag = fs_t_fstat_call(hd, &rpc_fd, &rpc_statbuf);

    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    *stat = rpc_statbuf.data;

    return msg_tag_get_val(tag);
}
// int ioctl(int fd, int req, void *arg)
RPC_GENERATION_CALL3(fs_t, FS_PROT, FS_IOCTL, ioctl,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, req,
                     rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, arg)
int fs_ioctl(sd_t _fd, int req, void *arg)
{
    obj_handler_t hd = mk_sd_init_raw(_fd).hd;
    int fd = mk_sd_init_raw(_fd).fd;

    rpc_int_t rpc_fd = {
        .data = fd,
    };
    rpc_int_t rpc_req = {
        .data = req,
    };
    rpc_umword_t_t rpc_arg = {
        .data = (umword_t)arg,
    };
    msg_tag_t tag;

    if (!stat)
    {
        return -EINVAL;
    }
    tag = fs_t_ioctl_call(hd, &rpc_fd, &rpc_req, &rpc_arg);

    return msg_tag_get_val(tag);
}
// int fcntl(int fd, int cmd, void *arg)
RPC_GENERATION_CALL3(fs_t, FS_PROT, FS_FCNTL, fcntl,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, cmd,
                     rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, arg)
int fs_fcntl(sd_t _fd, int cmd, void *arg)
{
    obj_handler_t hd = mk_sd_init_raw(_fd).hd;
    int fd = mk_sd_init_raw(_fd).fd;

    rpc_int_t rpc_fd = {
        .data = fd,
    };
    rpc_int_t rpc_cmd = {
        .data = cmd,
    };
    rpc_umword_t_t rpc_arg = {
        .data = (umword_t)arg,
    };
    msg_tag_t tag;

    if (!stat)
    {
        return -EINVAL;
    }
    tag = fs_t_fcntl_call(hd, &rpc_fd, &rpc_cmd, &rpc_arg);

    return msg_tag_get_val(tag);
}

/*fsync*/
RPC_GENERATION_CALL1(fs_t, FS_PROT, FS_FSYNC, fsync,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd)
int fs_fsync(sd_t _fd)
{
    obj_handler_t hd = mk_sd_init_raw(_fd).hd;
    int fd = mk_sd_init_raw(_fd).fd;

    rpc_int_t rpc_fd = {
        .data = fd,
    };
    msg_tag_t tag = fs_t_fsync_call(hd, &rpc_fd);

    return msg_tag_get_val(tag);
}

/*int unlink(const char *path)*/
RPC_GENERATION_CALL1(fs_t, FS_PROT, FS_UNLINK, unlink,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path)
int fs_unlink(const char *path)
{
    obj_handler_t hd;
    int ret = ns_query(path, &hd, 0);

    if (ret < 0)
    {
        return ret;
    }

    rpc_ref_file_array_t rpc_path = {
        .data = (uint8_t *)(&path[ret]),
        .len = strlen(&path[ret]) + 1,
    };

    msg_tag_t tag = fs_t_unlink_call(hd, &rpc_path);

    return msg_tag_get_val(tag);
}
// int symlink(const char *existing, const char *new)
RPC_GENERATION_CALL2(fs_t, FS_PROT, FS_SYMLINK, symlink,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, src,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, dst)

int fs_symlink(const char *src, const char *dst)
{
    obj_handler_t src_hd;
    obj_handler_t dst_hd;

    int src_ret = ns_query(src, &src_hd, 0x1);

    if (src_ret < 0)
    {
        return src_ret;
    }
    int dst_ret = ns_query(dst, &dst_hd, 0x1);

    if (dst_ret < 0)
    {
        return dst_ret;
    }
    if (src_hd != dst_hd)
    {
        return -EINVAL;
    }
    rpc_ref_file_array_t rpc_src = {
        .data = (uint8_t *)(&src[src_ret]),
        .len = strlen(&src[src_ret]) + 1,
    };
    rpc_ref_file_array_t rpc_dst = {
        .data = (uint8_t *)(&dst[dst_ret]),
        .len = strlen(&dst[dst_ret]) + 1,
    };
    msg_tag_t tag;

    tag = fs_t_symlink_call(src_hd, &rpc_src, &rpc_dst);

    return msg_tag_get_val(tag);
}

/*int mkdir(char *path)*/
RPC_GENERATION_CALL1(fs_t, FS_PROT, FS_MKDIR, mkdir,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, dir)

int fs_mkdir(char *path)
{
    obj_handler_t hd;
    int ret = ns_query(path, &hd, 0x1);

    if (ret < 0)
    {
        return ret;
    }

    rpc_ref_file_array_t rpc_path = {
        .data = (uint8_t *)(&path[ret]),
        .len = strlen(&path[ret]) + 1,
    };

    msg_tag_t tag = fs_t_mkdir_call(hd, &rpc_path);

    return msg_tag_get_val(tag);
}

/*int rmdir(char *path)*/
RPC_GENERATION_CALL1(fs_t, FS_PROT, FS_RMDIR, rmdir,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, dir)

int fs_rmdir(char *path)
{
    obj_handler_t hd;
    int ret = ns_query(path, &hd, 0x1);

    if (ret < 0)
    {
        return ret;
    }

    rpc_ref_file_array_t rpc_path = {
        .data = (uint8_t *)(&path[ret]),
        .len = strlen(&path[ret]) + 1,
    };

    msg_tag_t tag = fs_t_rmdir_call(hd, &rpc_path);

    return msg_tag_get_val(tag);
}
/*int rename(char *old, char *new)*/
RPC_GENERATION_CALL2(fs_t, FS_PROT, FS_RENAME, rename,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, old,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, new)

int fs_rename(char *old, char *new)
{
    obj_handler_t src_hd;
    obj_handler_t dst_hd;

    int src_ret = ns_query(old, &src_hd, 0x1);

    if (src_ret < 0)
    {
        return src_ret;
    }
    int dst_ret = ns_query(new, &dst_hd, 0x1);

    if (dst_ret < 0)
    {
        return dst_ret;
    }
    if (src_hd != dst_hd)
    {
        return -EINVAL;
    }
    rpc_ref_file_array_t rpc_src = {
        .data = (uint8_t *)(&old[src_ret]),
        .len = strlen(&old[src_ret]) + 1,
    };
    rpc_ref_file_array_t rpc_dst = {
        .data = (uint8_t *)(&new[dst_ret]),
        .len = strlen(&new[dst_ret]) + 1,
    };
    msg_tag_t tag;

    tag = fs_t_rename_call(src_hd, &rpc_src, &rpc_dst);

    return msg_tag_get_val(tag);
}
// int stat(const char *restrict path, struct stat *restrict buf)
RPC_GENERATION_CALL2(fs_t, FS_PROT, FS_STAT, stat,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                     rpc_kstat_t_t, rpc_kstat_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf)

int fs_stat(char *path, void *_buf)
{
    kstat_t *buf = (kstat_t *)_buf;
    obj_handler_t hd;
    if (!buf)
    {
        return -EINVAL;
    }
    int ret = ns_query(path, &hd, 0x0);

    if (ret < 0)
    {
        return ret;
    }
    rpc_ref_file_array_t rpc_path = {
        .data = (uint8_t *)(&path[ret]),
        .len = strlen(&path[ret]) + 1,
    };
    rpc_kstat_t_t rpc_buf;
    msg_tag_t tag;

    tag = fs_t_stat_call(hd, &rpc_path, &rpc_buf);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    *buf = rpc_buf.data;
    return msg_tag_get_val(tag);
}
// ssize_t readlink(const char *restrict path, char *restrict buf, size_t bufsize)
RPC_GENERATION_CALL3(fs_t, FS_PROT, FS_READLINK, readlink,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, bufsize)

int fs_readlink(const char *path, char *buf, int bufsize)
{
    obj_handler_t hd;
    if (!buf)
    {
        return -EINVAL;
    }
    int ret = ns_query(path, &hd, 0x1);

    if (ret < 0)
    {
        return ret;
    }
    rpc_ref_file_array_t rpc_path = {
        .data = (uint8_t *)(&path[ret]),
        .len = strlen(&path[ret]) + 1,
    };
    rpc_ref_file_array_t rpc_buf = {
        .data = buf,
        .len = bufsize,
    };
    rpc_int_t rpc_bufsize = {
        .data = bufsize,
    };
    msg_tag_t tag;

    tag = fs_t_readlink_call(hd, &rpc_path, &rpc_buf, &rpc_bufsize);
    return msg_tag_get_val(tag);
}

// static int __statfs(const char *path, struct statfs *buf)
RPC_GENERATION_CALL2(fs_t, FS_PROT, FS_STATFS, statfs,
                     rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                     rpc_statfs_t_t, rpc_statfs_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf)

int fs_statfs(const char *path, statfs_t *buf)
{
    obj_handler_t hd;
    if (!buf)
    {
        return -EINVAL;
    }
    int ret = ns_query(path, &hd, 0x1);

    if (ret < 0)
    {
        return ret;
    }
    rpc_ref_file_array_t rpc_path = {
        .data = (uint8_t *)(&path[ret]),
        .len = strlen(&path[ret]) + 1,
    };
    rpc_statfs_t_t rpc_buf;
    msg_tag_t tag;

    tag = fs_t_statfs_call(hd, &rpc_path, &rpc_buf);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    *buf = rpc_buf.data;
    return msg_tag_get_val(tag);
}
