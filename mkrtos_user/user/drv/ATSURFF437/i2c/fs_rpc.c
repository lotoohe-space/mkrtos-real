#include "cons_cli.h"
#include "cpiofs.h"
#include "fs_svr.h"
#include "rpc_prot.h"
#include "u_env.h"
#include "u_log.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_sys.h"
#include "mk_char_dev.h"
#include "mk_i2c_drv.h"
#include "fs_rpc.h"
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
static fs_t fs;

typedef struct file_desc
{
    pid_t pid;
    mk_char_dev_iter_t iter;
    void *ptr_dev;
    off_t offset;
    int open_type;
} file_desc_t;

#define CPIO_FS_FD_NR 8
static file_desc_t fds[CPIO_FS_FD_NR];

static file_desc_t *fd_alloc(int pid, void *ptr_dev, int open_type, int *fd)
{
    for (int i = 0; i < CPIO_FS_FD_NR; i++)
    {
        if (fds[i].ptr_dev == NULL)
        {
            fds[i].pid = pid;
            fds[i].ptr_dev = ptr_dev;
            fds[i].open_type = open_type;
            if (fd)
            {
                *fd = i;
            }
            return fds + i;
        }
    }
    return NULL;
}
static file_desc_t *fd_get(int pid, int fd)
{
    if (fd < 0 || fd >= CPIO_FS_FD_NR)
    {
        return NULL;
    }
    if (fds[fd].ptr_dev == NULL)
    {
        return NULL;
    }
    if (fds[fd].pid != pid)
    {
        return NULL;
    }
    return &fds[fd];
}
static void fd_free(int fd)
{
    if (fd < 0 || fd >= CPIO_FS_FD_NR)
    {
        return;
    }
    if (fds[fd].ptr_dev)
    {
        if (fds[fd].pid == thread_get_src_pid())
        {
            fds[fd].ptr_dev = NULL;
            fds[fd].pid = -1;
        }
    }
}

int fs_svr_open(const char *path, int flags, int mode)
{
    int pid = thread_get_src_pid();
    msg_tag_t tag;
    char *o_path;
    mk_char_dev_t *char_dev;
    int fd;
    int ret = 0;
    int is_open_dir = 0;

    printf("%s path is %s\n", __func__, path);
    if (path[0] == '/' && path[1] == '\0')
    {
        path++;
        is_open_dir = 1;
    }
    if (is_open_dir == 0)
    {
        if (path[0] == '\0')
        {
            char_dev = mk_char_get_first();
        }
        else
        {
            char_dev = mk_char_find_char_dev(path);
        }
        if (!char_dev)
        {
            printf("[pin] not dev %s.\n", path);
            return -ENODEV;
        }
        if (char_dev->ops->open)
        {
            ret = char_dev->ops->open(char_dev->dev);
        }
        if (ret < 0)
        {
            return ret;
        }
        // 找到指定文件
        file_desc_t *fdp = fd_alloc(pid, char_dev, is_open_dir, &fd);

        if (!fdp)
        {
            printf("[pin] not fd %s.\n", path);
            if (char_dev->ops->release)
            {
                char_dev->ops->release(char_dev->dev);
            }
            return -ENOMEM;
        }
    }
    else
    {
        file_desc_t *fdp = fd_alloc(pid, NULL, is_open_dir, &fd);

        if (!fdp)
        {
            printf("[pin] not fd %s.\n", path);
            return -ENOMEM;
        }
    }
    printf("[pin] open success %s, fd is %d.\n", path, fd);
    return fd;
}

int fs_svr_read(int fd, void *buf, size_t len)
{
    int ret;
    file_desc_t *fdp = fd_get(thread_get_src_pid(), fd);

    if (!fdp)
    {
        return -ENOENT;
    }
    if (fdp->open_type != 0)
    {
        return -EACCES;
    }
    mk_char_dev_t *char_dev = fdp->ptr_dev;

    if (char_dev->ops->read)
    {
        ret = char_dev->ops->read(char_dev->dev, buf, len, &fdp->offset);
    }

    return ret;
}
int fs_svr_write(int fd, void *buf, size_t len)
{
    int ret;
    file_desc_t *fdp = fd_get(thread_get_src_pid(), fd);

    if (!fdp)
    {
        return -ENOENT;
    }
    if (fdp->open_type != 0)
    {
        return -EACCES;
    }
    mk_char_dev_t *char_dev = fdp->ptr_dev;

    if (char_dev->ops->write)
    {
        ret = char_dev->ops->write(char_dev->dev, buf, len, &fdp->offset);
    }
}
void fs_svr_close(int fd)
{
    file_desc_t *fdp = fd_get(thread_get_src_pid(), fd);

    if (!fdp)
    {
        // return -ENOENT;
        return;
    }
    if (fdp->open_type == 0)
    {
        mk_char_dev_t *char_dev;

        char_dev = fdp->ptr_dev;
        if (char_dev->ops->release)
        {
            char_dev->ops->release(char_dev->dev);
        }
    }
    else
    {
        /*TODO:*/
    }
    fd_free(fd);
}
int fs_svr_lseek(int fd, int offs, int whence)
{
    file_desc_t *file = fd_get(thread_get_src_pid(), fd);
    int new_offs = 0;

    if (!file)
    {
        return -ENOENT;
    }
    if (file->open_type != 0)
    {
        return -EACCES;
    }
    switch (whence)
    {
    case SEEK_SET:
        new_offs = offs;
        break;
    case SEEK_END:
        return -EINVAL;
    case SEEK_CUR:
        return -EINVAL;
    default:
        return -EINVAL;
    }

    file->offset = new_offs;

    return 0;
}
int fs_svr_ftruncate(int fd, off_t off)
{
    return -ENOSYS;
}
void fs_svr_sync(int fd)
{
}

int fs_svr_readdir(int fd, dirent_t *dir)
{
    int pid = thread_get_src_pid();
    file_desc_t *file = fd_get(pid, fd);
    int new_offs = 0;
    mk_char_dev_t *char_dev;

    if (!file)
    {
        return -ENOENT;
    }
    if (file->open_type == 0)
    {
        return -EACCES;
    }

    if (file->ptr_dev == NULL)
    {
        char_dev = mk_char_get_iter_first(&file->iter);
    }
    else
    {
        char_dev = mk_char_get_iter_next(&file->iter);
    }
    if (char_dev == NULL)
    {
        return -ENOENT;
    }
    printf("[pin] readdir %s.\n", mk_dev_get_dev_name(char_dev->dev));
    file->ptr_dev = char_dev; // TODO: ref add.
    dir->d_reclen = sizeof(*dir);
    strncpy(dir->d_name, mk_dev_get_dev_name(char_dev->dev), MK_DEV_NAME_LEN);
    dir->d_name[MK_DEV_NAME_LEN - 1] = 0;
    dir->d_type = DT_CHR;

    return sizeof(*dir);
}
int fs_svr_ioctl(int fd, int req, void *arg)
{
    file_desc_t *file = fd_get(thread_get_src_pid(), fd);
    int new_offs = 0;
    int ret = -EIO;
    // printf("%s fd:%d req:%d arg:0x%x\n", __func__, fd, req, (umword_t)arg);
    if (!file)
    {
        return -ENOENT;
    }
    if (file->open_type != 0)
    {
        return -EACCES;
    }
    mk_char_dev_t *char_dev = file->ptr_dev;

    if (char_dev->ops->ioctl)
    {
        ret = char_dev->ops->ioctl(char_dev->dev, req, (umword_t)arg);
    }
    return ret;
}
int fs_svr_mkdir(char *path)
{
    return -ENOSYS;
}
int fs_svr_unlink(const char *path)
{
    return -ENOSYS;
}
int fs_svr_renmae(char *oldname, char *newname)
{
    return -ENOSYS;
}
int fs_svr_fstat(int fd, void *stat)
{
    file_desc_t *file = fd_get(thread_get_src_pid(), fd);

    if (!file)
    {
        return -ENOENT;
    }
    return -ENOSYS;
}
int fs_svr_symlink(const char *src, const char *dst)
{
    return -ENOSYS;
}
int fs_svr_fsync(int fd)
{
    return -ENOSYS;
}
int fs_svr_rmdir(char *path)
{
    return -ENOSYS;
}
int fs_svr_rename(char *old, char *new)
{
    return -ENOSYS;
}
int fs_svr_stat(const char *path, void *_buf)
{
    if (path == NULL || _buf == NULL)
    {
        return -EINVAL;
    }
    return -ENOSYS;
}

ssize_t fs_svr_readlink(const char *path, char *buf, size_t bufsize)
{
    return -ENOSYS;
}
int fs_svr_statfs(const char *path, struct statfs *buf)
{
    return -ENOSYS;
}

static const fs_operations_t ops =
    {
        .fs_svr_open = fs_svr_open,
        .fs_svr_read = fs_svr_read,
        .fs_svr_write = fs_svr_write,
        .fs_svr_close = fs_svr_close,
        .fs_svr_readdir = fs_svr_readdir,
        .fs_svr_lseek = fs_svr_lseek,
        .fs_svr_ftruncate = fs_svr_ftruncate,
        .fs_svr_fstat = fs_svr_fstat,
        .fs_svr_ioctl = fs_svr_ioctl,
        // .fs_svr_fcntl = fs_svr_fcntl,
        .fs_svr_fsync = fs_svr_fsync,
        .fs_svr_unlink = fs_svr_unlink,
        .fs_svr_symlink = fs_svr_symlink,
        .fs_svr_mkdir = fs_svr_mkdir,
        .fs_svr_rmdir = fs_svr_rmdir,
        .fs_svr_rename = fs_svr_rename,
        .fs_svr_stat = fs_svr_stat,
        .fs_svr_readlink = fs_svr_readlink,
};
void fs_svr_init(void)
{
    msg_tag_t tag;

    fs_init(&fs, &ops);
    meta_reg_svr_obj(&fs.svr, FS_PROT);
}
