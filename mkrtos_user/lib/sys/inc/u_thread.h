#pragma once

#include "u_types.h"
#include "u_prot.h"

#define MSG_BUG_LEN CONFIG_THREAD_MSG_BUG_LEN
#define MSG_BUF_RECV_R_FLAGS 0x02U //!< 接收上次发送数据的接收者
#define MSG_BUF_REPLY_FLAGS 0x04U  //!<

#define IPC_MSG_SIZE (CONFIG_THREAD_IPC_MSG_LEN * sizeof(void *))
#define MAP_BUF_SIZE (CONFIG_THREAD_MAP_BUF_LEN * sizeof(void *))
#define IPC_USER_SIZE (CONFIG_THREAD_USER_BUF_LEN * sizeof(void *))

#if IS_ENABLED(CONFIG_VCPU)
#define IPC_VPUC_MSG_OFFSET (3 * 1024) //!< vcpu 传递消息的偏移量
#endif

typedef struct ipc_msg
{
    union
    {
        struct
        {
            umword_t msg_buf[CONFIG_THREAD_IPC_MSG_LEN];
            umword_t map_buf[CONFIG_THREAD_MAP_BUF_LEN];
            umword_t user[CONFIG_THREAD_USER_BUF_LEN]; // 0 pthread使用 1驱动使用 2 ipc通信时存储目标的pid
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
msg_tag_t thread_run_cpu(obj_handler_t obj, uint8_t prio, umword_t cpu);
static inline msg_tag_t thread_run(obj_handler_t obj, uint8_t prio)
{
    return thread_run_cpu(obj, prio, -1);
}
msg_tag_t thread_bind_task(obj_handler_t obj, obj_handler_t tk_obj);
msg_tag_t thread_set_exec(obj_handler_t obj, obj_handler_t exec_th);

msg_tag_t thread_ipc_wait(ipc_timeout_t timeout, umword_t *obj, obj_handler_t ipc_obj);
msg_tag_t thread_ipc_reply(msg_tag_t in_tag, ipc_timeout_t timeout);
msg_tag_t thread_ipc_send(msg_tag_t in_tag, obj_handler_t target_th_obj, ipc_timeout_t timeout);
msg_tag_t thread_ipc_call(msg_tag_t in_tag, obj_handler_t target_th_obj, ipc_timeout_t timeout);
msg_tag_t thread_ipc_fast_call(msg_tag_t in_tag, obj_handler_t target_obj, umword_t arg0, umword_t arg1, umword_t arg2);
msg_tag_t thread_ipc_fast_replay(msg_tag_t in_tag, obj_handler_t target_obj, int unlock_bitmap);

static inline ipc_msg_t *thread_get_cur_ipc_msg(void)
{
    umword_t buf;
    thread_msg_buf_get(-1, &buf, NULL);
    return (ipc_msg_t *)buf;
}

#define thread_get_src_pid() thread_get_cur_ipc_msg()->user[2]
#define thread_set_src_pid(pid) thread_get_cur_ipc_msg()->user[2] = pid
