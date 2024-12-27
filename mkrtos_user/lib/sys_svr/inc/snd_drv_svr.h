#pragma once
#include "u_types.h"
#include "u_rpc_svr.h"
#include "u_slist.h"
#include "snd_drv_types.h"
int snd_drv_write(obj_handler_t obj, int len);
int snd_drv_read(obj_handler_t obj, int len);
int snd_drv_map(obj_handler_t *hd);
void snd_drv_init(snd_drv_t *ns);
