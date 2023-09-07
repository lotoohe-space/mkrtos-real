#pragma once

#include "u_prot.h"

#define MSG_BUG_LEN 128
#define MSG_BUF_RECV_R_FLAGS 0x02U //!< 接收上次发送数据的接收者
#define MSG_BUF_REPLY_FLAGS 0x04U  //!<

msg_tag_t ipc_bind(obj_handler_t obj, obj_handler_t tag_th, umword_t user_obj);
msg_tag_t ipc_wait(obj_handler_t obj, umword_t *user_obj);
msg_tag_t ipc_reply(obj_handler_t obj, umword_t len);
msg_tag_t ipc_call(obj_handler_t obj, umword_t len);