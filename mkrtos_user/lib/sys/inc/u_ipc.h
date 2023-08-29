#pragma once

#include "u_prot.h"
msg_tag_t ipc_recv(obj_handler_t obj);
msg_tag_t ipc_send(obj_handler_t obj, umword_t len);