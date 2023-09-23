#pragma once

#include "u_types.h"

int ns_register(const char *path, obj_handler_t svr_hd);
int ns_query(const char *path, obj_handler_t *svr_hd);
