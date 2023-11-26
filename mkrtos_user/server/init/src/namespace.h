#pragma once
#include "u_types.h"
#include "u_prot.h"

obj_handler_t namespace_init(void);
int namespace_register(const char *path, obj_handler_t hd);
int namespace_query(const char *path, obj_handler_t *hd);
// msg_tag_t ns_dispatch(ipc_msg_t *msg);
int namespace_pre_alloc_map_fd(void);
void namespace_loop(void);
