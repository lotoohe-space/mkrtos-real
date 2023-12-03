#pragma once

#include "u_types.h"
#include "ns_types.h"

int ns_register(const char *path, obj_handler_t svr_hd, enum node_type type);
int ns_query(const char *path, obj_handler_t *svr_hd);
