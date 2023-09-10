#pragma once

#include "u_types.h"

typedef struct sys_info
{
    umword_t sys_tick;
} sys_info_t;

void sys_read_info(obj_handler_t obj, sys_info_t *info);
