#pragma once

#include "u_types.h"

typedef struct sys_info
{
    umword_t sys_tick;
    umword_t bootfs_start_addr;
} sys_info_t;

msg_tag_t sys_read_info(obj_handler_t obj, sys_info_t *info);
