#pragma once

#include "u_prot.h"

#define MSG_BUG_LEN 128
#define MSG_BUF_CALL_FLAGS 0x02U     //!< 是CALL

msg_tag_t ipc_recv(obj_handler_t obj);
msg_tag_t ipc_send(obj_handler_t obj, umword_t len, umword_t flags);