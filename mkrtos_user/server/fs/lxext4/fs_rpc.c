#include "cons_cli.h"
#include "cpiofs.h"
#include "fs_svr.h"
#include "rpc_prot.h"
#include "u_env.h"
#include "u_log.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_sys.h"
#include "fs_rpc.h"
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

static fs_t fs;

void fs_svr_init(void)
{

    fs_init(&fs);
    meta_reg_svr_obj(&fs.svr, FS_PROT);
}
int fs_svr_open(const char *path, int flags, int mode)
{
    return -ENOSYS;
}

int fs_svr_read(int fd, void *buf, size_t len)
{
    return -ENOSYS;
}
int fs_svr_write(int fd, void *buf, size_t len)
{
    return -ENOSYS;
}
void fs_svr_close(int fd)
{
}
int fs_svr_lseek(int fd, int offs, int whence)
{
    return -ENOSYS;
}
int fs_svr_ftruncate(int fd, off_t off)
{
    return -ENOSYS;
}
int fs_svr_fsync(int fd)
{
    return -ENOSYS;
}
int fs_svr_readdir(int fd, dirent_t *dir)
{
    return -ENOSYS;
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
    return -ENOSYS;
}
int fs_svr_symlink(const char *src, const char *dst)
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

void fs_svr_loop(void)
{
    rpc_loop();
}