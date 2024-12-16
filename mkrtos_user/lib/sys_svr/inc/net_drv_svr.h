#pragma once
#include "u_types.h"
#include "u_rpc_svr.h"
#include "u_slist.h"
#include "net_drv_types.h"
int net_drv_write(obj_handler_t obj, int len);
int net_drv_read(obj_handler_t obj, int len);
int net_drv_map(obj_handler_t *hd);
void net_drv_init(net_drv_t *ns);
