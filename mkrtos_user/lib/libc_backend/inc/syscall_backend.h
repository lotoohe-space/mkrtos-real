#pragma once

#include "fs_backend.h"
#include "u_types.h"
#include <sys/uio.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/socket.h>
#define ARG_1_BE(ap, arg0, type0)       \
    do                                  \
    {                                   \
        arg0 = (type0)va_arg(ap, long); \
    } while (0)

#define ARG_2_BE(ap, arg0, type0, arg1, type1) \
    do                                         \
    {                                          \
        arg0 = (type0)va_arg(ap, long);        \
        arg1 = (type1)va_arg(ap, long);        \
    } while (0)

#define ARG_3_BE(ap, arg0, type0, arg1, type1, arg2, type2) \
    do                                                      \
    {                                                       \
        arg0 = (type0)va_arg(ap, long);                     \
        arg1 = (type1)va_arg(ap, long);                     \
        arg2 = (type2)va_arg(ap, long);                     \
    } while (0)

#define ARG_4_BE(ap, arg0, type0, arg1, type1, arg2, type2, arg3, type3) \
    do                                                                   \
    {                                                                    \
        arg0 = (type0)va_arg(ap, long);                                  \
        arg1 = (type1)va_arg(ap, long);                                  \
        arg2 = (type2)va_arg(ap, long);                                  \
        arg3 = (type3)va_arg(ap, long);                                  \
    } while (0)

#define ARG_5_BE(ap, arg0, type0, arg1, type1, arg2, type2, arg3, type3, arg4, type4) \
    do                                                                                \
    {                                                                                 \
        arg0 = (type0)va_arg(ap, long);                                               \
        arg1 = (type1)va_arg(ap, long);                                               \
        arg2 = (type2)va_arg(ap, long);                                               \
        arg3 = (type3)va_arg(ap, long);                                               \
        arg4 = (type4)va_arg(ap, long);                                               \
    } while (0)

#define ARG_6_BE(ap, arg0, type0, arg1, type1, arg2, type2, arg3, type3, arg4, type4, arg5, type5) \
    do                                                                                             \
    {                                                                                              \
        arg0 = (type0)va_arg(ap, long);                                                            \
        arg1 = (type1)va_arg(ap, long);                                                            \
        arg2 = (type2)va_arg(ap, long);                                                            \
        arg3 = (type3)va_arg(ap, long);                                                            \
        arg4 = (type4)va_arg(ap, long);                                                            \
        arg5 = (type5)va_arg(ap, long);                                                            \
    } while (0)

#define ARG_7_BE(ap, arg0, type0, arg1, type1, arg2, type2,          \
                 arg3, type3, arg4, type4, arg5, type5, arg6, type6) \
    do                                                               \
    {                                                                \
        arg0 = (type0)va_arg(ap, long);                              \
        arg1 = (type1)va_arg(ap, long);                              \
        arg2 = (type2)va_arg(ap, long);                              \
        arg3 = (type3)va_arg(ap, long);                              \
        arg4 = (type4)va_arg(ap, long);                              \
        arg5 = (type5)va_arg(ap, long);                              \
        arg6 = (type6)va_arg(ap, long);                              \
    } while (0)

long syscall_backend(long sys_inx, ...);

void fs_backend_init(void);
long be_lseek(long fd, long offset, long whence);
int be_open(const char *path, int flags, mode_t mode);
int be_close(int fd);
void be_exit(long exit_code);
umword_t be_mmap2(void *start,
                  size_t len,
                  long prot,
                  long flags,
                  long fd,
                  long _offset);
long be_read(long fd, char *buf, long size);
long be_write(long fd, char *buf, long size);
long be_readv(long fd, const struct iovec *iov, long iovcnt);
long be_writev(long fd, const struct iovec *iov, long iovcnt);
long be_ioctl(long fd, long req, void *args);
long be_set_tid_address(int *val);
long be_set_thread_area(void *p);
long be_getdents(long fd, char *buf, size_t size);
long be_mkdir(const char *path, mode_t mode);
long be_symlink(const char *src, const char *dst);
long be_stat(const char *path, void *buf);
long be_unlink(const char *path);
int be_clone(int (*func)(void *), void *stack, int flags, void *args, pid_t *ptid, void *tls, pid_t *ctid);
long be_munmap(void *start, size_t len);
umword_t be_mmap(void *start,
                 size_t len,
                 long prot,
                 long flags,
                 long fd,
                 long _offset);
int be_fcntl(int fd, int cmd, void *arg);
long be_chdir(const char *path);
int be_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
// net api
int be_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int be_bind(int s, const struct sockaddr *name, socklen_t namelen);
int be_shutdown(int s, int how);
int be_getpeername(int s, struct sockaddr *name, socklen_t *namelen);
int be_getsockname(int s, struct sockaddr *name, socklen_t *namelen);
int be_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
int be_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
int be_connect(int s, const struct sockaddr *name, socklen_t namelen);
int be_listen(int s, int backlog);
ssize_t be_recv(int s, void *mem, size_t len, int flags);
ssize_t be_recvfrom(int s, void *mem, size_t len, int flags,
                    struct sockaddr *from, socklen_t *fromlen);
ssize_t be_send(int s, const void *dataptr, size_t size, int flags);
ssize_t be_sendto(int s, const void *dataptr, size_t size, int flags,
                  const struct sockaddr *to, socklen_t tolen);
int be_socket(int domain, int type, int protocol);
// end net api
long be_clock_gettime(clockid_t clk_id, struct timespec *tp);

long sys_mmap(va_list ap);
long sys_set_tid_address(va_list ap);
long sys_set_thread_area(va_list ap);
long sys_exit(va_list ap);
long sys_munmap(va_list ap);
long sys__clone(va_list ap);
long sys_futex(va_list ap);
long sys_clock_nanosleep(va_list ap);
long sys_nanosleep(va_list ap);
int be_futex(uint32_t *uaddr, int futex_op, uint32_t val,
             const struct timespec *timeout, uint32_t uaddr2, uint32_t val3);
void pthread_cnt_inc(void);
int pthread_cnt_dec(void);
int pthread_get(void);
