#include "rpc_prot.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_hd_man.h"
#include "fs_svr.h"
#include "u_rpc.h"
#include "fs_types.h"
#include <stdio.h>
#include <sys/stat.h>
#include "kstat.h"
typedef struct kstat kstat_t;
RPC_TYPE_DEF_ALL(kstat_t)
/*open*/
RPC_GENERATION_OP3(fs_t, FS_PROT, FS_OPEN, open,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, mode)
{
    path->data[path->len - 1] = 0;
    int ret = fs_svr_open((char *)(path->data), flags->data, mode->data);
    return ret;
}

RPC_GENERATION_DISPATCH3(fs_t, FS_PROT, FS_OPEN, open,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, mode)
/*close*/
RPC_GENERATION_OP1(fs_t, FS_PROT, FS_CLOSE, close,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd)
{
    fs_svr_close(fd->data);
    return 0;
}

RPC_GENERATION_DISPATCH1(fs_t, FS_PROT, FS_CLOSE, close,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd)

/*read*/
RPC_GENERATION_OP3(fs_t, FS_PROT, FS_READ, read,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
{
    int ret = fs_svr_read(fd->data, buf->data, len->data);

    if (ret >= 0)
    {
        buf->len = ret;
    }
    return ret;
}

RPC_GENERATION_DISPATCH3(fs_t, FS_PROT, FS_READ, read,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

/*write*/
RPC_GENERATION_OP3(fs_t, FS_PROT, FS_WRITE, write,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, buf,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
{
    int ret = fs_svr_write(fd->data, buf->data, len->data);
    return ret;
}
RPC_GENERATION_DISPATCH3(fs_t, FS_PROT, FS_WRITE, write,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, buf,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

/*readdir*/
RPC_GENERATION_OP2(fs_t, FS_PROT, FS_READDIR, readdir,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_dirent_t_t, rpc_dirent_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, dir)
{
    return fs_svr_readdir(fd->data, &dir->data);
}

RPC_GENERATION_DISPATCH2(fs_t, FS_PROT, FS_READDIR, readdir,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_dirent_t_t, rpc_dirent_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, dir)

/*lseek*/
RPC_GENERATION_OP3(fs_t, FS_PROT, FS_LSEEK, lseek,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, offs,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, whence)
{
    int ret = fs_svr_lseek(fd->data, offs->data, whence->data);
    return ret;
}

RPC_GENERATION_DISPATCH3(fs_t, FS_PROT, FS_LSEEK, lseek,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, whence)

/*ftruncate*/
RPC_GENERATION_OP2(fs_t, FS_PROT, FS_FTRUNCATE, ftruncate,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_int64_t_t, rpc_int64_t_t, RPC_DIR_IN, RPC_TYPE_DATA, offs)
{
    int ret = fs_svr_ftruncate(fd->data, offs->data);
    return ret;
}

RPC_GENERATION_DISPATCH2(fs_t, FS_PROT, FS_FTRUNCATE, ftruncate,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_int64_t_t, rpc_int64_t_t, RPC_DIR_IN, RPC_TYPE_DATA, offs)
/*fstat*/
RPC_GENERATION_OP2(fs_t, FS_PROT, FS_FSTAT, fstat,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_kstat_t_t, rpc_kstat_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, statbuf)
{
    int ret = fs_svr_fstat(fd->data, &statbuf->data);
    return ret;
}
RPC_GENERATION_DISPATCH2(fs_t, FS_PROT, FS_FSTAT, fstat,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_kstat_t_t, rpc_kstat_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, statbuf)

/*ioctl*/
RPC_GENERATION_OP3(fs_t, FS_PROT, FS_IOCTL, ioctl,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, req,
                   rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, arg)

{
    int ret = fs_svr_ioctl(fd->data, req->data, (void *)arg->data /*TODO:可能传递的内存指针*/);

    return ret;
}
 __attribute__((__weak__)) int fs_svr_ioctl(int fd, int req, void *arg)
{
    printf("%s is not support.\n", __func__);
    return -ENOSYS;
}
RPC_GENERATION_DISPATCH3(fs_t, FS_PROT, FS_IOCTL, ioctl,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, req,
                         rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, arg)

/*fcntl*/
RPC_GENERATION_OP3(fs_t, FS_PROT, FS_FCNTL, fcntl,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, cmd,
                   rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, arg)
{
    int ret = fs_svr_fcntl(fd->data, cmd->data, (void *)arg->data /*TODO:可能传递的内存指针*/);

    return ret;
}
RPC_GENERATION_DISPATCH3(fs_t, FS_PROT, FS_FCNTL, fcntl,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, cmd,
                         rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, arg)

/*fsync*/
RPC_GENERATION_OP1(fs_t, FS_PROT, FS_FSYNC, fsync,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd)
{
    return fs_svr_fsync(fd->data);
}

RPC_GENERATION_DISPATCH1(fs_t, FS_PROT, FS_FSYNC, fsync,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd)
/*int unlink(const char *path)*/
RPC_GENERATION_OP1(fs_t, FS_PROT, FS_UNLINK, unlink,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path)
{
    path->data[path->len - 1] = 0;
    return fs_svr_unlink(path->data);
}
RPC_GENERATION_DISPATCH1(fs_t, FS_PROT, FS_UNLINK, unlink,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path)
// int symlink(const char *existing, const char *new)
RPC_GENERATION_OP2(fs_t, FS_PROT, FS_SYMLINK, symlink,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, src,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, dst)
{
    src->data[src->len - 1] = 0;
    dst->data[dst->len - 1] = 0;

    return fs_svr_symlink(src->data, src->data);
}
RPC_GENERATION_DISPATCH2(fs_t, FS_PROT, FS_SYMLINK, symlink,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, src,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, dst)

/*int mkdir(char *path)*/
RPC_GENERATION_OP1(fs_t, FS_PROT, FS_MKDIR, mkdir,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, dir)
{
    dir->data[dir->len - 1] = 0;
    return fs_svr_mkdir(dir->data);
}
RPC_GENERATION_DISPATCH1(fs_t, FS_PROT, FS_MKDIR, mkdir,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, dir)
/*int rmdir(char *path)*/
RPC_GENERATION_OP1(fs_t, FS_PROT, FS_RMDIR, rmdir,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, dir)
{
    dir->data[dir->len - 1] = 0;
    return fs_svr_rmdir(dir->data);
}
RPC_GENERATION_DISPATCH1(fs_t, FS_PROT, FS_RMDIR, rmdir,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, dir)

/*int rename(char *old, char *new)*/
RPC_GENERATION_OP2(fs_t, FS_PROT, FS_RENAME, rename,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, old,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, new)
{
    old->data[old->len - 1] = 0;
    new->data[new->len - 1] = 0;

    return fs_svr_rename(old->data, new->data);
}
RPC_GENERATION_DISPATCH2(fs_t, FS_PROT, FS_RENAME, rename,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, old,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, new)

// int stat(const char *restrict path, struct stat *restrict buf)
RPC_GENERATION_OP2(fs_t, FS_PROT, FS_STAT, stat,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_kstat_t_t, rpc_kstat_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf)
{
    path->data[path->len - 1] = 0;
    return fs_svr_stat(path->data, &buf->data);
}
RPC_GENERATION_DISPATCH2(fs_t, FS_PROT, FS_STAT, stat,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                         rpc_kstat_t_t, rpc_kstat_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf)

// ssize_t readlink(const char *restrict path, char *restrict buf, size_t bufsize)
RPC_GENERATION_OP3(fs_t, FS_PROT, FS_READLINK, readlink,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, bufsize)
{
    path->data[path->len - 1] = 0;
    // buf->data[buf->len - 1] = 0;

    return fs_svr_readlink(path->data, buf->data, MIN(bufsize->data, buf->len));
}
RPC_GENERATION_DISPATCH3(fs_t, FS_PROT, FS_READLINK, readlink,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, bufsize)

// int statfs(const char *path, struct statfs *buf)
RPC_GENERATION_OP2(fs_t, FS_PROT, FS_STATFS, statfs,
                   rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_statfs_t_t, rpc_statfs_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf)
{
    path->data[path->len - 1] = 0;
    return fs_svr_statfs(path->data, &buf->data);
}
RPC_GENERATION_DISPATCH2(fs_t, FS_PROT, FS_STATFS, statfs,
                         rpc_ref_file_array_t, rpc_file_array_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                         rpc_statfs_t_t, rpc_statfs_t_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf)

/*dispatch*/
RPC_DISPATCH18(fs_t, FS_PROT, typeof(FS_OPEN), FS_OPEN, open, FS_READ, read,
               FS_WRITE, write, FS_CLOSE, close, FS_LSEEK, lseek, FS_FTRUNCATE, ftruncate,
               FS_FSYNC, fsync, FS_READDIR, readdir, FS_MKDIR, mkdir, FS_UNLINK,
               unlink, FS_RENAME, rename, FS_FSTAT, fstat, FS_SYMLINK, symlink,
               FS_RMDIR, rmdir, FS_STAT, stat,
               FS_READLINK, readlink, FS_STATFS, statfs, FS_IOCTL, ioctl)

void fs_init(fs_t *fs)
{
    rpc_svr_obj_init(&fs->svr, rpc_fs_t_dispatch, FS_PROT);
}
