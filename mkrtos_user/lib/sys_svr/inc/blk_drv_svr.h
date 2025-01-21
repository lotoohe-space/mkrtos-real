#pragma once
#include "u_types.h"
#include "u_rpc_svr.h"
#include "u_slist.h"
#include "blk_drv_types.h"
int blk_drv_write(obj_handler_t obj, int len, int inx);
int blk_drv_read(obj_handler_t obj, int len, int inx);
int blk_drv_map(obj_handler_t *hd);
void blk_drv_init(blk_drv_t *ns);
