#pragma once
#include "u_types.h"
#include "u_rpc_svr.h"
#include "u_slist.h"
#include "blk_drv_types.h"
int blk_drv_cli_write(obj_handler_t dm9000_obj, obj_handler_t shm_obj, int len, int inx);
int blk_drv_cli_read(obj_handler_t dm9000_obj, obj_handler_t shm_obj, int len, int inx);
int blk_drv_cli_map(obj_handler_t svr_boj, obj_handler_t *sem_obj);
