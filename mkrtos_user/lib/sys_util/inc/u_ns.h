#pragma once

#include "u_types.h"

int cli_ns_register(const char *name, obj_handler_t hd);
int cli_ns_query(const char *name, obj_handler_t *ret_hd);
