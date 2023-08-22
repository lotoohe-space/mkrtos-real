#pragma once


#include "fs_backend.h"


#define ARG_1_BE(ap, arg0, type0)\
do {\
    arg0 = (type0)va_arg(ap, long);\
} while(0)

#define ARG_2_BE(ap, arg0, type0, arg1, type1)\
do {\
    arg0 = (type0)va_arg(ap, long);\
    arg1 = (type1)va_arg(ap, long);\
} while(0)

#define ARG_3_BE(ap, arg0, type0, arg1, type1, arg2, type2)\
do {\
    arg0 = (type0)va_arg(ap, long);\
    arg1 = (type1)va_arg(ap, long);\
    arg2 = (type2)va_arg(ap, long);\
} while(0)


long syscall_backend(long sys_inx, ...);
