#pragma once
#include "u_types.h"
#include "u_rpc_svr.h"
#include "u_slist.h"

int net_drv_cli_write(obj_handler_t dm9000_obj, obj_handler_t shm_obj, int len);
int net_drv_cli_read(obj_handler_t dm9000_obj, obj_handler_t shm_obj);
