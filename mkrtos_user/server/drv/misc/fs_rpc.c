#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "fs_svr.h"
#include "ff.h"
#include "u_log.h"
#include "u_env.h"
#include <stdio.h>
#include <fcntl.h>
static fs_t fs;

void fs_svr_init(obj_handler_t ipc)
{
    fs_init(&fs);
    fs.ipc = ipc;
}
typedef struct file_desc
{

} file_desc_t;

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
void fs_svr_sync(int fd)
{
}
int fs_svr_readdir(int fd, dirent_t *dir)
{
    return -ENOSYS;
}
int fs_svr_mkdir(char *path)
{
    return -ENOSYS;
}
int fs_svr_unlink(char *path)
{
    return -ENOSYS;
}
int fs_svr_renmae(char *oldname, char *newname)
{
    return -ENOSYS;
}
int fs_svr_fstat(int fd, stat_t *stat)
{
    return -ENOSYS;
}

void fs_svr_loop(void)
{
    rpc_loop(fs.ipc, &fs.svr);
}