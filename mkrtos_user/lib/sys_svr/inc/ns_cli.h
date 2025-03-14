#pragma once

#include "u_types.h"
#include "ns_types.h"

int ns_register(const char *path, obj_handler_t svr_hd, int flags);
int ns_query(const char *path, obj_handler_t *svr_hd, int flags);
int ns_query_svr(const char *path, obj_handler_t *svr_hd, int flags);
