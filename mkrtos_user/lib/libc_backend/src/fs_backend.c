
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
AUTO_CALL(101)
void fs_backend_init(void)
{
    assert(fd_map_alloc(0, 0, FD_TTY) >= 0);
    assert(fd_map_alloc(0, 1, FD_TTY) >= 0);
    assert(fd_map_alloc(0, 2, FD_TTY) >= 0);
}
int be_open(const char *path, int flags, mode_t mode)
{
    if (path == NULL)
    {
        return -ENOENT;
    }
    int fd = fs_open(path, flags, mode);

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
    case FD_TTY:
    {
    }
    break;
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
static int be_tty_read(char *buf, long size)
{
    pid_t pid;
    int len;
    int r_len = 0;

    if (size == 0)
    {
        return 0;
    }
    task_get_pid(TASK_THIS, (umword_t *)(&pid));

    while (r_len < size)
    {
        if (pid == 0)
        {
            len = ulog_read_bytes(u_get_global_env()->log_hd, buf + r_len, size - r_len);
        }
        else
        {
            len = cons_read(buf + r_len, size - r_len);
        }
        if (len < 0)
        {
            return len;
        }
        else if (len == 0)
        {
            u_sema_down(SEMA_PROT, 0/*TODO:*/, NULL);
            continue;
        }
        r_len += len;
        break;
    }
    return r_len;
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
    {
        return be_tty_read(buf, size);
    }
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
            ulog_write_bytes(u_get_global_env()->log_hd, buf, size);
        }
        else
        {
            cons_write(buf, size);
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
            pid_t pid;
            int read_cn;

            task_get_pid(TASK_THIS, (umword_t *)(&pid));
            if (pid == 0)
            {
                read_cn = ulog_read_bytes(u_get_global_env()->log_hd, iov[i].iov_base, iov[i].iov_len);
            }
            else
            {
            again_read:
                read_cn = cons_read(iov[i].iov_base, iov[i].iov_len);
                if (read_cn < 0)
                {
                    return read_cn;
                }
                else if (read_cn == 0)
                {
                    u_sema_down(SEMA_PROT, 0, NULL);
                    cons_write_str(".\n");
                    goto again_read;
                }
            }
            wlen += read_cn;
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
            pid_t pid;

            task_get_pid(TASK_THIS, (umword_t *)(&pid));
            if (pid == 0)
            {
                ulog_write_bytes(u_get_global_env()->log_hd, iov[i].iov_base, iov[i].iov_len);
            }
            else
            {
                cons_write(iov[i].iov_base, iov[i].iov_len);
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
        struct winsize *wsz = args;

        wsz->ws_row = 30;
        wsz->ws_col = 30;
        wsz->ws_xpixel = 800;
        wsz->ws_ypixel = 480;
        ret = 0;
    }
    break;
    case FD_FS:
    {
        ret = fs_ioctl(u_fd.priv_fd, req, args);
    }
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
    return fs_mkdir((char *)path);
}
long be_symlink(const char *src, const char *dst)
{
    return fs_symlink(src, dst);
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
    return fs_stat((char *)path, buf);
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
    return fs_unlink(path);
}
long be_poll(struct pollfd *fds, nfds_t n, int timeout)
{
    for (int i = 0; i < n; i++)
    {
        if (fds[0].fd >= 3)
        {
            /*TODO:暂时只支持TTY*/
            return -1;
        }
        /*FIXME:性能优化*/
        if (fds[0].events & POLLIN)
        {
            char buf;
            int len;
            int time = 0;

            if (timeout == -1)
            {
                // u_sema_down(SEMA_PROT);
            again1:
                len = cons_read(&buf, 0);
                if (len <= 0)
                {
                    u_sleep_ms(1);
                    goto again1;
                }
                return 1;
            }
            else
            {
            again:
                len = cons_read(&buf, 0);
                if (len <= 0)
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
long sys_be_ftruncate(va_list ap)
{
    long fd;
    off_t off;
    int ret;

    ARG_2_BE(ap, fd, long, off, off_t);

    ret = fs_ftruncate(fd, off);

    return ret;
}
int be_fcntl(int fd, int cmd, void *arg)
{
    return fs_fcntl(fd, cmd, arg);
}
int be_access(const char *filename, int amode)
{
    return -ENOSYS;
}
