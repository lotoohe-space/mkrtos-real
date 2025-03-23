
#include "fs_backend.h"
#include "fd_map.h"
#include "fs_cli.h"
#include "syscall_backend.h"
#include "u_sleep.h"
#include <assert.h>
#include <cons_cli.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <u_env.h>
#include <u_log.h>
#include <u_prot.h>
#include <u_sema.h>
#include <u_task.h>
#include <u_util.h>
#include <poll.h>
#include <u_path.h>
#include "kstat.h"
#include "svr_path.h"
#include "u_mutex.h"
#include "u_hd_man.h"
#include "ns_cli.h"
#define FS_PATH_LEN 64
static char cur_path[FS_PATH_LEN] = "/";

static u_mutex_t lock_cons;
// AUTO_CALL(101)
void fs_backend_init(void)
{

    umword_t cur_pid;
    msg_tag_t tag;
    char *pwd;

    tag = task_get_pid(TASK_THIS, (umword_t *)(&cur_pid));
    assert(msg_tag_get_val(tag) >= 0);
    if (cur_pid != 0)
    {
        assert(be_open(TTY_PATCH, O_RDWR, 0) >= 0);
        assert(be_open(TTY_PATCH, O_RDWR, 0) >= 0);
        assert(be_open(TTY_PATCH, O_RDWR, 0) >= 0);
    }
    else
    {
        // init进程直接分配假的fd即可，不用调用be_open打开
        assert(fd_map_alloc(0, 0, FD_TTY) >= 0);
        assert(fd_map_alloc(0, 1, FD_TTY) >= 0);
        assert(fd_map_alloc(0, 2, FD_TTY) >= 0);
        assert(u_mutex_init(&lock_cons, handler_alloc()) >= 0);
    }
    pwd = getenv("PWD");
    if (pwd)
    {
        be_chdir(pwd);
    }
}
const char *fs_backend_cur_path(void)
{
    return cur_path;
}
void fs_cons_lock(void)
{
    u_mutex_lock(&lock_cons, 0, NULL);
}
void fs_cons_unlock(void)
{
    u_mutex_unlock(&lock_cons);
}
void fs_cons_write_unlock(void *buf, size_t size)
{
    ulog_write_bytes(u_get_global_env()->log_hd, buf, size);
}
void fs_cons_write(void *buf, size_t size)
{
    u_mutex_lock(&lock_cons, 0, NULL);
    ulog_write_bytes(u_get_global_env()->log_hd, buf, size);
    u_mutex_unlock(&lock_cons);
}
#define SHM_DEV_PATH "/dev/shm/"
enum path_type
{
    PATH_SHM,
    PATH_FS,
};
static enum path_type be_path_type(const char *path)
{
    if (memcmp(SHM_DEV_PATH, path, 9) == 0)
    {
        return PATH_SHM;
    }
    return PATH_FS;
}
int be_open(const char *path, int flags, mode_t mode)
{
    int fd;

    if (path == NULL)
    {
        return -ENOENT;
    }
    char new_path[FS_PATH_LEN]; // FIXME:动态申请
    u_rel_path_to_abs(cur_path, path, new_path);

    if (be_path_type(new_path) == PATH_SHM)
    {
        // 共享内存的路径是固定的
        fd = be_shm_open(new_path, flags, mode);
        if (fd < 0)
        {
            if (fd == -EISDIR)
            {
                goto next;
            }
        }
        return fd;
    }
next:
    fd = fs_open(new_path, flags, mode);
    if (fd < 0)
    {
        return fd;
    }
    int user_fd = fd_map_alloc(0, fd, FD_FS);

    if (user_fd < 0)
    {
        be_close(user_fd);
    }
    return user_fd;
}
long sys_be_open(va_list ap)
{
    void *path;
    long flags;
    long mode;

    ARG_3_BE(ap, path, void *, flags, long, mode, long);

    return be_open(path, flags, mode);
}
long sys_be_open_at(va_list ap)
{
    long resv;
    void *path;
    long flags;
    long mode;

    ARG_4_BE(ap, resv, long, path, void *, flags, long, mode, long);

    return be_open(path, flags, mode);
}
int be_close(int fd)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_free(fd, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    switch (u_fd.type)
    {
    case FD_FS:
    {
        return fs_close(u_fd.priv_fd);
    }
    break;
    default:
        return -ENOSYS;
    }
    return 0;
}
long sys_be_close(va_list ap)
{
    long fd;

    ARG_1_BE(ap, fd, long);

    return be_close(fd);
}
long be_read(long fd, char *buf, long size)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(fd, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    switch (u_fd.type)
    {
    case FD_TTY:
        return -ENOSYS;
    break;
    case FD_FS:
    {
        return fs_read(u_fd.priv_fd, buf, size);
    }
    break;
    default:
        return -ENOSYS;
    }

    return -ENOSYS;
}
long be_write(long fd, char *buf, long size)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(fd, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    switch (u_fd.type)
    {
    case FD_TTY:
    {
        pid_t pid;

        task_get_pid(TASK_THIS, (umword_t *)(&pid));
        if (pid == 0)
        {
            fs_cons_write(buf, size);
        }
        else
        {
            return -ENOSYS;
        }
        return size;
    }
    break;
    case FD_FS:
    {
        return fs_write(u_fd.priv_fd, buf, size);
    }
    break;
    default:
        return -ENOSYS;
    }
    return -ENOSYS;
}
long be_readv(long fd, const struct iovec *iov, long iovcnt)
{
    long wlen = 0;
    for (int i = 0; i < iovcnt; i++)
    {
        fd_map_entry_t u_fd;
        int ret = fd_map_get(fd, &u_fd);

        if (ret < 0)
        {
            return -EBADF;
        }
        switch (u_fd.type)
        {
        case FD_TTY:
        {
            return -ENOSYS;
        }
        break;
        case FD_FS:
        {
            int rsize = fs_read(u_fd.priv_fd, iov[i].iov_base, iov[i].iov_len);

            wlen += rsize;
        }
        break;
        default:
            return -ENOSYS;
        }
    }
    return wlen;
}
long be_writev(long fd, const struct iovec *iov, long iovcnt)
{
    long wlen = 0;
    fd_map_entry_t u_fd;
    int ret;

    ret = fd_map_get(fd, &u_fd);
    if (ret < 0)
    {
        return -EBADF;
    }
    for (int i = 0; i < iovcnt; i++)
    {
        switch (u_fd.type)
        {
        case FD_TTY:
        {
            pid_t pid;

            task_get_pid(TASK_THIS, (umword_t *)(&pid));
            if (pid == 0)
            {
                fs_cons_write(iov[i].iov_base, iov[i].iov_len);
            }
            else
            {
                return -ENOSYS;
            }
            wlen += iov[i].iov_len;
        }
        break;
        case FD_FS:
        {
            int wsize = fs_write(u_fd.priv_fd, iov[i].iov_base, iov[i].iov_len);

            wlen += wsize;
        }
        break;
        default:
            return -ENOSYS;
        }
    }
    return wlen;
}
long sys_be_read(va_list ap)
{
    long fd;
#define ARG0 char *
    ARG0 buf;
    long size;

    ARG_3_BE(ap, fd, long, buf, ARG0, size, long);
#undef ARG0

    return be_read(fd, buf, size);
}
long sys_be_write(va_list ap)
{
    long fd;
#define ARG0 char *
    ARG0 buf;
    long size;

    ARG_3_BE(ap, fd, long, buf, ARG0, size, long);
#undef ARG0

    return be_write(fd, buf, size);
}
long sys_be_writev(va_list ap)
{
    long fd;
#define ARG1 const struct iovec *
    ARG1 iov;
    long iovcnt;
    ARG_3_BE(ap, fd, long, iov, ARG1, iovcnt, long);
#undef ARG1
    return be_writev(fd, iov, iovcnt);
}
long be_ioctl(long fd, long req, void *args)
{
    int ret;
    fd_map_entry_t u_fd;
    ret = fd_map_get(fd, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    switch (u_fd.type)
    {
    case FD_TTY:
    {
        return -ENOSYS;
    }
    break;
    case FD_FS:
    {
        ret = fs_ioctl(u_fd.priv_fd, req, args);
    }
    break;
    default:
        ret = -ENOSYS;
        break;
    }
    return ret;
}
long sys_be_ioctl(va_list ap)
{
    long fd;
    long request;
#define ARG2 void *
    ARG2 args;
    ARG_3_BE(ap, fd, long, request, long, args, ARG2);
#undef ARG2
    return be_ioctl(fd, request, args);
}
long be_lseek(long fd, long offset, long whence)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(fd, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    switch (u_fd.type)
    {
    case FD_TTY:
    {
        return -ENOSYS;
    }
    break;
    case FD_FS:
    {
        return fs_lseek(u_fd.priv_fd, offset, whence);
    }
    break;
    default:
        return -ENOSYS;
    }
    return 0;
}
long sys_be_lseek(va_list ap)
{
    long fd;
    long offset;
    long whence;
    ARG_3_BE(ap, fd, long, offset, long, whence, long);

    return be_lseek(fd, offset, whence);
}
long be_mkdir(const char *path, mode_t mode)
{
    char new_path[FS_PATH_LEN]; // FIXME:动态申请
    u_rel_path_to_abs(cur_path, path, new_path);
    return fs_mkdir((char *)new_path);
}
long be_symlink(const char *src, const char *dst)
{
    char new_src_path[FS_PATH_LEN]; // FIXME:动态申请
    char new_dst_path[FS_PATH_LEN]; // FIXME:动态申请
    u_rel_path_to_abs(cur_path, src, new_src_path);
    u_rel_path_to_abs(cur_path, dst, new_dst_path);
    return fs_symlink(new_src_path, new_dst_path);
}
long be_getdents(long fd, char *buf, size_t size)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(fd, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    switch (u_fd.type)
    {
    case FD_TTY:
    {
        return -ENOSYS;
    }
    break;
    case FD_FS:
    {
        ret = fs_readdir(u_fd.priv_fd, (struct dirent *)buf);
    }
    break;
    default:
        return -ENOSYS;
    }
    return ret;
}

// int stat(const char *pathname, struct stat *buf);
long be_stat(const char *path, void *_buf)
{
    struct kstat *buf = _buf;
    char new_src_path[FS_PATH_LEN]; // FIXME:动态申请
    u_rel_path_to_abs(cur_path, path, new_src_path);
    return fs_stat((char *)new_src_path, buf);
}
long be_fstat(int fd, void *_buf)
{
    struct kstat *buf = _buf;
    fd_map_entry_t u_fd;
    int ret = fd_map_get(fd, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return fs_fstat(u_fd.priv_fd, (void *)buf);
}
long be_unlink(const char *path)
{
    char new_src_path[FS_PATH_LEN]; // FIXME:动态申请
    u_rel_path_to_abs(cur_path, path, new_src_path);

    int path_len = strlen(path);
    char *parent_last_path;

    if (new_src_path[path_len - 1] == '/')
    {
        new_src_path[path_len - 1] = '\0';
    }
    
    parent_last_path = strrchr(new_src_path, '/');
    if (parent_last_path)
    {
        if (parent_last_path == new_src_path)
        {
            return -EINVAL;
        }
        *parent_last_path = '\0';
    }
    obj_handler_t hd;
    int ret = ns_query(new_src_path, &hd);

    if (ret < 0)
    {
        return ret;
    }
    *parent_last_path = '/';
    return fs_unlink(hd, ret == 0 ? new_src_path : parent_last_path);
}
long be_poll(struct pollfd *fds, uint32_t n, int timeout)
{
    for (int i = 0; i < n; i++)
    {
        if (fds[0].fd >= 3)
        {
            /*TODO:暂时只支持TTY*/
            return -ENOSYS;
        }
        /*FIXME:性能优化*/
        if (fds[0].events & POLLIN)
        {
            // char buf;
            int len;
            int ret;
            int time = 0;

            if (timeout == -1)
            {
            again1:
                ret = be_ioctl(fds[0].fd, FIONREAD, &len);
                if (ret < 0)
                {
                    return ret;
                }
                if (len == 0)
                {
                    u_sleep_ms(1);
                    goto again1;
                }
                return 1;
            }
            else
            {
            again:
                ret = be_ioctl(fds[0].fd, FIONREAD, &len);
                if (ret < 0)
                {
                    return ret;
                }
                if (len == 0)
                {
                    u_sleep_ms(1);
                    time++;
                    if (time >= timeout)
                    {
                        /*timeover*/
                        return 0;
                    }
                    goto again;
                }
                else
                {
                    fds[0].revents |= POLLIN;
                    return 1;
                }
            }
        }
    }
    return 0;
}

long sys_be_getdents(va_list ap)
{
    long fd;
    char *buf;
    size_t size;
    ARG_3_BE(ap, fd, long, buf, char *, size, long);

    return be_getdents(fd, buf, size);
}
long be_ftruncate(int fd, off_t off)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(fd, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    switch (u_fd.type)
    {
    case FD_TTY:
    {
        return -ENOSYS;
    }
    break;
    case FD_FS:
    {
        ret = fs_ftruncate(u_fd.priv_fd, off);
    }
    break;
    case FD_SHM:
    {
        ret = be_inner_shm_ftruncate(u_fd.priv_fd, off);
    }
    break;
    default:
        return -ENOSYS;
    }
    return ret;
}
long sys_be_ftruncate(va_list ap)
{
    long fd;
    off_t off;
    int ret;

    ARG_2_BE(ap, fd, long, off, off_t);

    ret = be_ftruncate(fd, off);
    return ret;
}
int be_fcntl(int fd, int cmd, void *arg)
{
    return fs_fcntl(fd, cmd, arg);
}
int be_access(const char *filename, int amode)
{
    // char new_src_path[FS_PATH_LEN]; // FIXME:动态申请
    // u_rel_path_to_abs(cur_path, path, new_src_path);
    return -ENOSYS;
}
long be_chdir(const char *path)
{
    int ret;
    struct kstat buf;
    char new_src_path[FS_PATH_LEN]; // FIXME:动态申请
    u_rel_path_to_abs(cur_path, path, new_src_path);

    ret = fs_stat((char *)new_src_path, &buf);
    if (ret < 0)
    {
        return ret;
    }
    if (!S_ISDIR(buf.st_mode))
    {
        return -ENOTDIR;
    }
    strncpy(cur_path, new_src_path, FS_PATH_LEN);
    cur_path[FS_PATH_LEN - 1] = '\0';
    return ret;
}
long be_getcwd(char *path, size_t size)
{
    strncpy(path, cur_path, size);
    path[size - 1] = '\0';
    return strlen(path);
}
