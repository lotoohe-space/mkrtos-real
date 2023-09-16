#pragma once

#include "u_types.h"

typedef struct uenv
{
    obj_handler_t log_hd;
    obj_handler_t ns_hd;
    obj_handler_t rev1;
    obj_handler_t rev2;
} uenv_t;

uenv_t *u_get_global_env(void);
void u_env_init(void *in_env);
void u_env_default_init(void);
