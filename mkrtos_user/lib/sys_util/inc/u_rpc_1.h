#include "u_types.h"
#include "u_thread.h"
#include "u_err.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_task.h"
#include <stddef.h>
#include <sys/types.h>
#include <errno.h>
/**
 * @brief 该宏用于生成一个客户端的调用函数（传递一个参数）
 *
 */
#define RPC_GENERATION_CALL1(struct_type, prot, op, func_name, cli_type0, svr_type0, dir0, rpc_type0, name0)                       \
    msg_tag_t struct_type##_##func_name##_call(obj_handler_t hd, cli_type0 *var0)                                                  \
    {                                                                                                                              \
        void *buf;                                                                                                                 \
        ipc_msg_t *msg_ipc;                                                                                                        \
                                                                                                                                   \
        thread_msg_buf_get(-1, (umword_t *)(&buf), NULL);                                                                 \
        msg_ipc = (ipc_msg_t *)buf;                                                                                                \
                                                                                                                                   \
        int off = 0;                                                                                                               \
        int off_map = 0;                                                                                                           \
        int ret = -1;                                                                                                              \
        umword_t op_val = op;                                                                                                           \
        /*拷贝op*/                                                                                                               \
        rpc_memcpy(msg_ipc->msg_buf, &op_val, sizeof(op_val));                                                                     \
        off += rpc_align(sizeof(op_val), __alignof(op));                                                                           \
                                                                                                                                   \
        RPC_CLI_MSG_TO_BUF_IN(rpc_type0, cli_type0, var0, dir0, (uint8_t *)msg_ipc->msg_buf, off);                                 \
        PRC_CLI_FILL_MAP_BUF(rpc_type0, cli_type0, var0, dir0, (uint8_t *)msg_ipc->map_buf, off_map);                              \
        /*msg_tag_t tag = dispatch_test(msg_tag_init4(0, ROUND_UP(off, WORD_BYTES), ROUND_UP(off_map, WORD_BYTES), 0), msg_ipc);*/ \
        msg_tag_t tag = thread_ipc_call(msg_tag_init4(0, ROUND_UP(off, WORD_BYTES), ROUND_UP(off_map, WORD_BYTES), prot), hd,      \
                                        ipc_timeout_create2(0, 0));                                                                \
                                                                                                                                   \
        if (msg_tag_get_val(tag) < 0)                                                                                              \
        {                                                                                                                          \
            return tag;                                                                                                            \
        } /*拷贝返回的数据*/                                                                                                \
        off = 0;                                                                                                                   \
        RPC_CLI_BUF_TO_MSG_OUT(rpc_type0, cli_type0, var0, dir0, (uint8_t *)msg_ipc->msg_buf, off, tag.msg_buf_len *WORD_BYTES);   \
        return tag;                                                                                                                \
    }
/**
 * @brief 该宏用于生成服务端的分发函数（一个参数）
 *
 */
#define RPC_GENERATION_DISPATCH1(struct_type, prot, op, func_name,                                         \
                                 cli_type0, svr_type0, dir0, rpc_type0, name0)                             \
    msg_tag_t struct_type##_##func_name##_dispatch(struct_type *obj, msg_tag_t tag, ipc_msg_t *ipc_msg)    \
    {                                                                                                      \
        svr_type0 var0;                                                                                    \
        size_t op_val;                                                                                     \
        uint8_t *value = (uint8_t *)(ipc_msg->msg_buf);                                                    \
        int off = 0;                                                                                       \
                                                                                                           \
        RPC_TYPE_INIT_FUNC_CALL(svr_type0, &var0);                                                         \
                                                                                                           \
        /*取得op*/                                                                                       \
        op_val = *((typeof(op) *)value);                                                                   \
        off += sizeof(typeof(op));                                                                         \
        off = rpc_align(off, __alignof(typeof(op)));                                                       \
                                                                                                           \
        RPC_SVR_BUF_TO_MSG_IN(rpc_type0, svr_type0, &var0, dir0, value, off, tag.msg_buf_len *WORD_BYTES); \
                                                                                                           \
        short ret_val = struct_type##_##func_name##_op(obj, &var0);                                        \
                                                                                                           \
        if (ret_val < 0)                                                                                   \
        {                                                                                                  \
            return msg_tag_init4(0, 0, 0, ret_val);                                                        \
        }                                                                                                  \
        off = 0;                                                                                           \
        int off_map = 0;                                                                                   \
        RPC_SVR_MSG_TO_BUF_OUT(rpc_type0, svr_type0, &var0, dir0, value, off);                             \
        PRC_SVR_FILL_MAP_BUF(rpc_type0, svr_type0, &var0, dir0, value, off_map);                           \
        return msg_tag_init4(0, ROUND_UP(off, WORD_BYTES), ROUND_UP(off_map, WORD_BYTES), ret_val);        \
    }
/**
 * @brief 该宏用于生成一个服务端的实现（一个参数）
 *
 */
#define RPC_GENERATION_OP1(struct_type, prot, op, func_name, cli_type0, svr_type0, dir0, rpc_type0, name0) \
    short struct_type##_##func_name##_op(struct_type *obj, svr_type0 *name0)
