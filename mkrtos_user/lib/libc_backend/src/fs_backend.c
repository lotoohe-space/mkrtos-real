
#include "fs_backend.h"
#include "syscall_backend.h"
#include <stdarg.h>
#include <errno.h>
#include <u_prot.h>
#include <u_log.h>
#include <u_env.h>
#include <sys/uio.h>
long be_read(long fd, char *buf, long size)
{
    return -ENOSYS;
}
long be_write(long fd, char *buf, long size)
{
    switch (fd)
    {
    case 0:
    case 1:
    case 2:
        ulog_write_bytes(u_get_global_env()->log_hd, buf, size);
        return size;
    default:
        return -ENOSYS;
    }
    return 0;
}
long be_writev(long fd, const struct iovec *iov, long iovcnt)
{
    long wlen = 0;
    for (int i = 0; i < iovcnt; i++)
    {
        ulog_write_bytes(u_get_global_env()->log_hd, iov[i].iov_base, iov[i].iov_len);
        wlen += iov[i].iov_len;
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
