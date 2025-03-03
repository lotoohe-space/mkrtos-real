#include "cons_cli.h"
#include "fs_svr.h"
#include "rpc_prot.h"
#include "u_env.h"
#include "u_log.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_sig.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "kstat.h"
#include "appfs_open.h"

static fs_t fs;

static int fs_sig_call_back(pid_t pid, umword_t sig_val)
{
    switch (sig_val)
    {
    case KILL_SIG:
        appfs_task_free(pid);
        break;
    }
    return 0;
}

int fs_svr_open(const char *path, int flags, int mode)
{
    int fd;
    pid_t pid = thread_get_src_pid();

    fd = appfs_open(path, flags, mode);

    if (fd >= 0)
    {
#ifdef CONFIG_USING_SIG
        int w_ret = pm_sig_watch(pid, 0 /*TODO:现在只有kill */);
        if (w_ret < 0)
        {
            printf("pm wath pid %d err.\n", w_ret);
        }
#endif
    }

    return fd;
}

int fs_svr_read(int fd, void *buf, size_t len)
{
    int ret;

    ret = appfs_read(fd, buf, len);

    return ret;
}
int fs_svr_write(int fd, void *buf, size_t len)
{
    int ret;

    ret = appfs_write(fd, buf, len);
    return ret;
}
void fs_svr_close(int fd)
{
    appfs_close(fd);
}
int fs_svr_readdir(int fd, dirent_t *dir)
{
    int ret;

    ret = appfs_readdir(fd, dir);
    return ret;
}
int fs_svr_lseek(int fd, int offs, int whence)
{
    int ret;

    ret = appfs_lseek(fd, offs, whence);
    return ret;
}
int fs_svr_ftruncate(int fd, off_t off)
{
    int ret;

    ret = appfs_truncate(fd, off);
    return ret;
}
int fs_svr_fstat(int fd, void *_stat)
{
    int ret;

    ret = appfs_fstat(fd, _stat);
    return ret;
}
int fs_svr_ioctl(int fd, int req, void *arg)
{
    int ret;

    ret = appfs_ioctl(fd, req, (unsigned long)arg);
    return ret;
}
int fs_svr_fsync(int fd)
{
    return 0;
}
int fs_svr_unlink(const char *path)
{
    int ret;

    ret = appfs_remove(path);
    return ret;
}
int fs_svr_symlink(const char *existing, const char *new)
{
    return -ENOSYS;
}
int fs_svr_mkdir(char *path)
{
    return -ENOSYS;
}
int fs_svr_rmdir(char *path)
{
    return -ENOSYS;
}
int fs_svr_rename(char *oldname, char *newname)
{
    return -ENOSYS;
}
int fs_svr_stat(const char *path, void *_buf)
{
    return appfs_stat(path, _buf);
}
ssize_t fs_svr_readlink(const char *path, char *buf, size_t bufsize)
{
    return -ENOSYS;
}

int fs_svr_statfs(const char *path, struct statfs *buf)
{
    return -ENOSYS;
}
#if 0
void fs_svr_loop(void)
{
    rpc_loop();
}
#endif
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
    fs_init(&fs, &ops);
    meta_reg_svr_obj(&fs.svr, FS_PROT);
#ifdef CONFIG_USING_SIG
    pm_sig_func_set(fs_sig_call_back);
#endif
}
