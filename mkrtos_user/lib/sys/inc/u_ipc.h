#pragma once

#include "u_prot.h"

#define MSG_BUG_LEN 128
#define MSG_BUF_RECV_R_FLAGS 0x02U //!< 接收上次发送数据的接收者
#define MSG_BUF_REPLY_FLAGS 0x04U  //!<

#define IPC_MSG_SIZE 96  //!< IPC消息大小
#define MAP_BUF_SIZE 16  //!< 映射消息大小
#define IPC_USER_SIZE 12 //!< 用户态消息大小

typedef struct ipc_msg
{
    union
    {
        struct
        {
            umword_t msg_buf[IPC_MSG_SIZE / WORD_BYTES];
            umword_t map_buf[MAP_BUF_SIZE / WORD_BYTES];
            umword_t user[IPC_USER_SIZE / WORD_BYTES];
        };
        uint8_t data[MSG_BUG_LEN];
    };
} ipc_msg_t;

msg_tag_t ipc_bind(obj_handler_t obj, obj_handler_t tag_th, umword_t user_obj);
msg_tag_t ipc_wait(obj_handler_t obj, umword_t *user_obj);
msg_tag_t ipc_reply(obj_handler_t obj, msg_tag_t tag);
msg_tag_t ipc_call(obj_handler_t obj, msg_tag_t tag);
