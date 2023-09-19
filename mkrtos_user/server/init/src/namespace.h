#pragma once
#include "u_types.h"
#include "u_prot.h"

void ns_init(void);
int ns_register(const char *path, obj_handler_t hd);
int ns_query(const char *path, obj_handler_t *hd);
msg_tag_t ns_dispatch(ipc_msg_t *msg);
int ns_pre_alloc_map_fd(ipc_msg_t *msg);
