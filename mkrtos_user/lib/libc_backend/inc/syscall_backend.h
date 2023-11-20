#pragma once

#include "fs_backend.h"
#include "u_types.h"
#include <sys/uio.h>
#include <time.h>
// #include <unistd.h>

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

long syscall_backend(long sys_inx, ...);

void fs_backend_init(void);

long be_read(long fd, char *buf, long size);
long be_write(long fd, char *buf, long size);
long be_writev(long fd, const struct iovec *iov, long iovcnt);
long be_ioctl(long fd, long req, void *args);
long be_set_tid_address(int *val);
long be_set_thread_area(void *p);
umword_t be_munmap(void *start, size_t len);
umword_t be_mmap2(void *start,
                  size_t len,
                  long prot,
                  long flags,
                  long fd,
                  long _offset);

umword_t sys_mmap2(va_list ap);
long sys_set_tid_address(va_list ap);
long sys_set_thread_area(va_list ap);
void sys_exit(va_list ap);
umword_t sys_munmap(va_list ap);
int be_futex(uint32_t *uaddr, int futex_op, uint32_t val,
             const struct timespec *timeout, uint32_t uaddr2, uint32_t val3);

void pthread_cnt_inc(void);
int pthread_cnt_dec(void);
int pthread_get(void);
