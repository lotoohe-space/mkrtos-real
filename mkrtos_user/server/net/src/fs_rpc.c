
#include <errno.h>
#include <lwip/sockets.h>
#include <fs_svr.h>
#include <u_prot.h>
#include <u_sys.h>
#include <rpc_prot.h>
static fs_t fs;
extern void fd_man_unreg(int fd);
int fs_svr_open(const char *path, int flags, int mode)
{
    return -ENOSYS;
}
int fs_svr_read(int fd, void *buf, size_t len)
{
    return lwip_read(fd, buf, len);
}
int fs_svr_write(int fd, void *buf, size_t len)
{
    return lwip_write(fd, buf, len);
}
void fs_svr_close(int fd)
{
    lwip_close(fd);
    fd_man_unreg(fd);
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
int fs_svr_unlink(const char *path)
{
    return -ENOSYS;
}
int fs_svr_renmae(char *oldname, char *newname)
{
    return -ENOSYS;
}
int fs_svr_fstat(int fd, void *_stat)
{
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
    return -ENOSYS;
}
int fs_svr_ioctl(int fd, int req, void *arg)
{
    return lwip_ioctl(fd, req, arg /*TODO:数据拷贝，这里会造成系统崩溃*/);
}
ssize_t fs_svr_readlink(const char *path, char *buf, size_t bufsize)
{
    return -ENOSYS;
}
int fs_svr_statfs(const char *path, struct statfs *buf)
{
    return -ENOSYS;
}
int fs_svr_fcntl(int fd, int cmd, void *arg)
{
    return lwip_fcntl(fd, cmd, (int)arg);
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
        .fs_svr_fcntl = fs_svr_fcntl,
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
