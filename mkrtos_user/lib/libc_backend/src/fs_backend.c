
#include "fs_backend.h"
#include "syscall_backend.h"
#include "fs_cli.h"
#include <stdarg.h>
#include <errno.h>
#include <u_prot.h>
#include <u_log.h>
#include <u_env.h>
#include <u_task.h>
#include <cons_cli.h>
#include <sys/uio.h>
#include <assert.h>
#include <u_util.h>
#include "fd_map.h"
#include "u_sleep.h"
AUTO_CALL(101)
void fs_backend_init(void)
{
    assert(fd_map_alloc(0, 0, FD_TTY) >= 0);
    assert(fd_map_alloc(0, 1, FD_TTY) >= 0);
    assert(fd_map_alloc(0, 2, FD_TTY) >= 0);
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
            u_sleep_ms(10);
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
        return fs_read(fd, buf, size);
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
        return fs_write(fd, buf, size);
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
                    u_sleep_ms(10); // TODO:改成信号量
                    goto again_read;
                }
            }
            wlen += read_cn;
        }
        break;
        case FD_FS:
        {
            int rsize = fs_read(fd, iov[i].iov_base, iov[i].iov_len);

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
            int wsize = fs_write(fd, iov[i].iov_base, iov[i].iov_len);

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
    /*TODO:*/
    return 0;
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
