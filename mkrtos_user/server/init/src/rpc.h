#pragma once

#include "u_types.h"

int rpc_creaite_bind_ipc(obj_handler_t th, void *obj, obj_handler_t *ipc_hd);
void rpc_loop(obj_handler_t ipc_hd, msg_tag_t (*rpc_dispatch)(ipc_msg_t *msg));
