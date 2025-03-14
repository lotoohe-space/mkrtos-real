#pragma once
#include "u_thread.h"

obj_handler_t rpc_hd_get(int inx);
int rpc_hd_alloc_raw(ipc_msg_t *msg);
int rpc_hd_alloc(void);
