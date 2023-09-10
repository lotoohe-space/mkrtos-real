#pragma once

#include "fs_backend.h"
#include "u_types.h"

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

umword_t be_mmap2(va_list ap);
