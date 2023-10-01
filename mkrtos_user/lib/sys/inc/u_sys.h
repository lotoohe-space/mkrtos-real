#pragma once

#include "u_types.h"
#include "u_prot.h"
typedef struct sys_info
{
    umword_t sys_tick;
    umword_t bootfs_start_addr;
} sys_info_t;

msg_tag_t sys_read_info(obj_handler_t obj, sys_info_t *info);

#define sys_read_tick() ({          \
    sys_info_t info;                \
                                    \
    sys_read_info(SYS_PROT, &info); \
    info.sys_tick;                  \
})
