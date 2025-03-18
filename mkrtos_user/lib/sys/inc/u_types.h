
#pragma once

#include <stdint.h>
#include <stddef.h>

#include "u_arch.h"
#include "u_util.h"
typedef unsigned long umword_t;
typedef long mword_t;
typedef unsigned short uhmword_t;
typedef umword_t ptr_t;
typedef char bool_t;
typedef unsigned long obj_addr_t;
typedef long obj_handler_t;
typedef umword_t addr_t;
typedef umword_t paddr_t;
typedef umword_t mk_pid_t;

/**
 * @brief 资源描述符定义
 *
 */
typedef umword_t sd_t; // 16bit(hd)|16bit(fd)
typedef union mk_sd
{
    umword_t raw;
    struct
    {
        uhmword_t hd;
        uhmword_t fd;//只能用15bits，最高位为符号位
    };
} mk_sd_t;
#define mk_sd_init_raw(val) ((mk_sd_t){.raw = val})
#define mk_sd_init2(h, f) ((mk_sd_t){\
    .hd = h, \
    .fd = (f) & (~((1 << (sizeof(uhmword_t) * 8 - 1))))\
}\
)
#define mk_sd_get_hd(sd) ((obj_handler_t)((sd).hd))
#define mk_sd_get_fd(sd) ((int)((sd).fd))

// #ifndef NULL
// #define NULL ((void *)0)
// #endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!(FALSE))
#endif

#define HANDLER_INVALID ((umword_t)(-1))
