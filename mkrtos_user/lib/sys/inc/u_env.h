#pragma once

#include "u_types.h"

//!< 暂时只能是16字节
typedef struct uenv
{
    obj_handler_t log_hd;
    obj_handler_t rev0;
    obj_handler_t rev1;
    obj_handler_t rev2;
} uenv_t;
