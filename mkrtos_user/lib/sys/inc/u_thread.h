#pragma once

#include "u_types.h"
#include "u_prot.h"

#define MSG_BUG_LEN CONFIG_THREAD_MSG_BUG_LEN
#define MSG_BUF_RECV_R_FLAGS 0x02U //!< 接收上次发送数据的接收者
#define MSG_BUF_REPLY_FLAGS 0x04U  //!<

#define IPC_MSG_SIZE CONFIG_THREAD_IPC_MSG_LEN  //!< IPC消息大小
#define MAP_BUF_SIZE CONFIG_THREAD_MAP_BUF_LEN  //!< 映射消息大小
#define IPC_USER_SIZE 12 //!< 用户态消息大小

typedef struct ipc_msg
{
    union
    {
        struct
        {
            umword_t msg_buf[IPC_MSG_SIZE / WORD_BYTES];
            umword_t map_buf[MAP_BUF_SIZE / WORD_BYTES];
            umword_t user[IPC_USER_SIZE / WORD_BYTES]; // 0 pthread使用 1驱动使用 2 ipc通信时存储目标的pid
        };
        uint8_t data[MSG_BUG_LEN];
    };
} ipc_msg_t;

typedef union ipc_timeout
{
    umword_t raw;
    struct
    {
        uhmword_t send_timeout;
        uhmword_t recv_timeout;
    };
} ipc_timeout_t;
static inline ipc_timeout_t ipc_timeout_create(umword_t raw)
{
    return (ipc_timeout_t){
        .raw = raw,
    };
}
static inline ipc_timeout_t ipc_timeout_create2(uhmword_t send_timeout, uhmword_t recv_timeout)
{
    return (ipc_timeout_t){
        .send_timeout = send_timeout,
        .recv_timeout = recv_timeout,
    };
}

#define EWTIMEDOUT 131 /* Connection timed out */
#define ERTIMEDOUT 132 /* Connection timed out */

msg_tag_t thread_yield(obj_handler_t obj);
msg_tag_t thread_msg_buf_set(obj_handler_t obj, void *msg);
msg_tag_t thread_msg_buf_get(obj_handler_t obj, umword_t *msg, umword_t *len);
msg_tag_t thread_exec_regs(obj_handler_t obj, umword_t pc, umword_t sp, umword_t ram, umword_t cp_stack);
msg_tag_t thread_run(obj_handler_t obj, uint8_t prio);
msg_tag_t thread_bind_task(obj_handler_t obj, obj_handler_t tk_obj);

msg_tag_t thread_ipc_wait(ipc_timeout_t timeout, umword_t *obj, obj_handler_t ipc_obj);
msg_tag_t thread_ipc_reply(msg_tag_t in_tag, ipc_timeout_t timeout);
msg_tag_t thread_ipc_send(msg_tag_t in_tag, obj_handler_t target_th_obj, ipc_timeout_t timeout);
 __attribute__((optimize(0)))  msg_tag_t thread_ipc_call(msg_tag_t in_tag, obj_handler_t target_th_obj, ipc_timeout_t timeout);

static inline ipc_msg_t *thread_get_cur_ipc_msg(void)
{
    umword_t buf;
    thread_msg_buf_get(-1, &buf, NULL);
    return (ipc_msg_t *)buf;
}

#define thread_get_src_pid() thread_get_cur_ipc_msg()->user[2]
#define thread_set_src_pid(pid) thread_get_cur_ipc_msg()->user[2] = pid
