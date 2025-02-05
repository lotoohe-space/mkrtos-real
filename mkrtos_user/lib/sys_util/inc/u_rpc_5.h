#include "u_types.h"
#include "u_thread.h"
#include "u_err.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_task.h"
#include <stddef.h>
#include <sys/types.h>
#include <errno.h>

#define RPC_GENERATION_CALL5(is_fast, struct_type, prot, op, func_name,                                                                               \
                             cli_type0, svr_type0, dir0, rpc_type0, name0,                                                                            \
                             cli_type1, svr_type1, dir1, rpc_type1, name1,                                                                            \
                             cli_type2, svr_type2, dir2, rpc_type2, name2,                                                                            \
                             cli_type3, svr_type3, dir3, rpc_type3, name3,                                                                            \
                             cli_type4, svr_type4, dir4, rpc_type4, name4)                                                                            \
    msg_tag_t struct_type##_##func_name##_call(obj_handler_t hd, cli_type0 *var0, cli_type1 *var1, cli_type2 *var2, cli_type3 *var3, cli_type4 *var4) \
    {                                                                                                                                                 \
        void *buf;                                                                                                                                    \
        ipc_msg_t *msg_ipc;                                                                                                                           \
                                                                                                                                                      \
        thread_msg_buf_get(-1, (umword_t *)(&buf), NULL);                                                                                             \
        msg_ipc = (ipc_msg_t *)buf;                                                                                                                   \
                                                                                                                                                      \
        int off = 0;                                                                                                                                  \
        int off_buf = 0;                                                                                                                              \
        int ret = -1;                                                                                                                                 \
        umword_t op_val = op;                                                                                                                         \
        /*拷贝op*/                                                                                                                                    \
        rpc_memcpy(msg_ipc->msg_buf, &op_val, __alignof(op_val));                                                                                     \
        off += rpc_align(sizeof(op_val), __alignof(op));                                                                                              \
                                                                                                                                                      \
        RPC_CLI_MSG_TO_BUF_IN(rpc_type0, cli_type0, var0, dir0, (uint8_t *)msg_ipc->msg_buf, off);                                                    \
        PRC_CLI_FILL_MAP_BUF(rpc_type0, cli_type0, var0, dir0, (uint8_t *)msg_ipc->map_buf, off_buf);                                                 \
        RPC_CLI_MSG_TO_BUF_IN(rpc_type1, cli_type1, var1, dir1, (uint8_t *)msg_ipc->msg_buf, off);                                                    \
        PRC_CLI_FILL_MAP_BUF(rpc_type1, cli_type1, var1, dir1, (uint8_t *)msg_ipc->map_buf, off_buf);                                                 \
        RPC_CLI_MSG_TO_BUF_IN(rpc_type2, cli_type2, var2, dir2, (uint8_t *)msg_ipc->msg_buf, off);                                                    \
        PRC_CLI_FILL_MAP_BUF(rpc_type2, cli_type2, var2, dir2, (uint8_t *)msg_ipc->map_buf, off_buf);                                                 \
        RPC_CLI_MSG_TO_BUF_IN(rpc_type3, cli_type3, var3, dir3, (uint8_t *)msg_ipc->msg_buf, off);                                                    \
        PRC_CLI_FILL_MAP_BUF(rpc_type3, cli_type3, var3, dir3, (uint8_t *)msg_ipc->map_buf, off_buf);                                                 \
        RPC_CLI_MSG_TO_BUF_IN(rpc_type4, cli_type4, var4, dir4, (uint8_t *)msg_ipc->msg_buf, off);                                                    \
        PRC_CLI_FILL_MAP_BUF(rpc_type4, cli_type4, var4, dir4, (uint8_t *)msg_ipc->map_buf, off_buf);                                                 \
        /*msg_tag_t tag = dispatch_test(msg_tag_init4(0, ROUND_UP(off, WORD_BYTES), ROUND_UP(off_buf, WORD_BYTES), 0), msg_ipc); */                   \
        msg_tag_t tag;                                                                                                                                \
        if (is_fast)                                                                                                                                  \
        {                                                                                                                                             \
            tag = thread_ipc_fast_call(msg_tag_init4(0, ROUND_UP(off, WORD_BYTES), ROUND_UP(off_buf, WORD_BYTES), prot),                              \
                                       hd, 1111, 2222, 3333);                                                                                         \
        }                                                                                                                                             \
        else                                                                                                                                          \
        {                                                                                                                                             \
            tag = thread_ipc_call(msg_tag_init4(0, ROUND_UP(off, WORD_BYTES), ROUND_UP(off_buf, WORD_BYTES), prot), hd,                               \
                                  ipc_timeout_create2(0, 0));                                                                                         \
        }                                                                                                                                             \
        if (msg_tag_get_val(tag) < 0)                                                                                                                 \
        {                                                                                                                                             \
            return tag;                                                                                                                               \
        } /*拷贝返回的数据*/                                                                                                                          \
        off = 0;                                                                                                                                      \
        RPC_CLI_BUF_TO_MSG_OUT(rpc_type0, cli_type0, var0, dir0, (uint8_t *)msg_ipc->msg_buf, off, tag.msg_buf_len *WORD_BYTES);                      \
        RPC_CLI_BUF_TO_MSG_OUT(rpc_type1, cli_type1, var1, dir1, (uint8_t *)msg_ipc->msg_buf, off, tag.msg_buf_len *WORD_BYTES);                      \
        RPC_CLI_BUF_TO_MSG_OUT(rpc_type2, cli_type2, var2, dir2, (uint8_t *)msg_ipc->msg_buf, off, tag.msg_buf_len *WORD_BYTES);                      \
        RPC_CLI_BUF_TO_MSG_OUT(rpc_type3, cli_type3, var3, dir3, (uint8_t *)msg_ipc->msg_buf, off, tag.msg_buf_len *WORD_BYTES);                      \
        RPC_CLI_BUF_TO_MSG_OUT(rpc_type4, cli_type4, var4, dir4, (uint8_t *)msg_ipc->msg_buf, off, tag.msg_buf_len *WORD_BYTES);                      \
        return tag;                                                                                                                                   \
    }

#define RPC_GENERATION_DISPATCH5(struct_type, prot, op, func_name,                                         \
                                 cli_type0, svr_type0, dir0, rpc_type0, name0,                             \
                                 cli_type1, svr_type1, dir1, rpc_type1, name1,                             \
                                 cli_type2, svr_type2, dir2, rpc_type2, name2,                             \
                                 cli_type3, svr_type3, dir3, rpc_type3, name3,                             \
                                 cli_type4, svr_type4, dir4, rpc_type4, name4)                             \
    msg_tag_t struct_type##_##func_name##_dispatch(struct_type *obj, msg_tag_t tag, ipc_msg_t *ipc_msg)    \
    {                                                                                                      \
        svr_type0 var0;                                                                                    \
        svr_type1 var1;                                                                                    \
        svr_type2 var2;                                                                                    \
        svr_type3 var3;                                                                                    \
        svr_type4 var4;                                                                                    \
        size_t op_val;                                                                                     \
        uint8_t *value = (uint8_t *)(ipc_msg->msg_buf);                                                    \
        int off = 0;                                                                                       \
                                                                                                           \
        RPC_TYPE_INIT_FUNC_CALL(svr_type0, &var0);                                                         \
        RPC_TYPE_INIT_FUNC_CALL(svr_type1, &var1);                                                         \
        RPC_TYPE_INIT_FUNC_CALL(svr_type2, &var2);                                                         \
        RPC_TYPE_INIT_FUNC_CALL(svr_type3, &var3);                                                         \
        RPC_TYPE_INIT_FUNC_CALL(svr_type4, &var4);                                                         \
                                                                                                           \
        /*取得op*/                                                                                         \
        op_val = *((typeof(op) *)value);                                                                   \
        if (op_val != op)                                                                                  \
        {                                                                                                  \
            return msg_tag_init4(0, 0, 0, -EPROTO);                                                        \
        }                                                                                                  \
        off += sizeof(typeof(op));                                                                         \
        off = rpc_align(off, __alignof(typeof(op)));                                                       \
                                                                                                           \
        RPC_SVR_BUF_TO_MSG_IN(rpc_type0, svr_type0, &var0, dir0, value, off, tag.msg_buf_len *WORD_BYTES); \
        RPC_SVR_BUF_TO_MSG_IN(rpc_type1, svr_type1, &var1, dir1, value, off, tag.msg_buf_len *WORD_BYTES); \
        RPC_SVR_BUF_TO_MSG_IN(rpc_type2, svr_type2, &var2, dir2, value, off, tag.msg_buf_len *WORD_BYTES); \
        RPC_SVR_BUF_TO_MSG_IN(rpc_type3, svr_type3, &var3, dir3, value, off, tag.msg_buf_len *WORD_BYTES); \
        RPC_SVR_BUF_TO_MSG_IN(rpc_type4, svr_type4, &var4, dir4, value, off, tag.msg_buf_len *WORD_BYTES); \
                                                                                                           \
        short ret_val = struct_type##_##func_name##_op(obj, &var0, &var1, &var2, &var3, &var4);            \
                                                                                                           \
        if (ret_val < 0)                                                                                   \
        {                                                                                                  \
            return msg_tag_init4(0, 0, 0, ret_val);                                                        \
        }                                                                                                  \
        off = 0;                                                                                           \
        int off_map = 0;                                                                                   \
        RPC_SVR_MSG_TO_BUF_OUT(rpc_type0, svr_type0, &var0, dir0, value, off);                             \
        RPC_SVR_MSG_TO_BUF_OUT(rpc_type1, svr_type1, &var1, dir1, value, off);                             \
        RPC_SVR_MSG_TO_BUF_OUT(rpc_type2, svr_type2, &var2, dir2, value, off);                             \
        RPC_SVR_MSG_TO_BUF_OUT(rpc_type3, svr_type3, &var3, dir3, value, off);                             \
        RPC_SVR_MSG_TO_BUF_OUT(rpc_type4, svr_type4, &var4, dir4, value, off);                             \
        PRC_SVR_FILL_MAP_BUF(rpc_type0, svr_type0, &var0, dir0, ipc_msg->map_buf, off_map);                \
        PRC_SVR_FILL_MAP_BUF(rpc_type1, svr_type1, &var1, dir1, ipc_msg->map_buf, off_map);                \
        PRC_SVR_FILL_MAP_BUF(rpc_type2, svr_type2, &var2, dir2, ipc_msg->map_buf, off_map);                \
        PRC_SVR_FILL_MAP_BUF(rpc_type3, svr_type3, &var3, dir3, ipc_msg->map_buf, off_map);                \
        PRC_SVR_FILL_MAP_BUF(rpc_type4, svr_type4, &var4, dir4, ipc_msg->map_buf, off_map);                \
        return msg_tag_init4(0, ROUND_UP(off, WORD_BYTES), ROUND_UP(off_map, WORD_BYTES), ret_val);        \
    }

#define RPC_GENERATION_OP5(struct_type, prot, op, func_name,             \
                           cli_type0, svr_type0, dir0, rpc_type0, name0, \
                           cli_type1, svr_type1, dir1, rpc_type1, name1, \
                           cli_type2, svr_type2, dir2, rpc_type2, name2, \
                           cli_type3, svr_type3, dir3, rpc_type3, name3, \
                           cli_type4, svr_type4, dir4, rpc_type4, name4) \
    short struct_type##_##func_name##_op(struct_type *obj, svr_type0 *name0, svr_type1 *name1, svr_type2 *name2, svr_type3 *name3, svr_type4 *name4)

#define RPC_GENERATION_OP_DISPATCH5(struct_type, prot, op, func_name,                                                                                 \
                                    cli_type0, svr_type0, dir0, rpc_type0, name0,                                                                     \
                                    cli_type1, svr_type1, dir1, rpc_type1, name1,                                                                     \
                                    cli_type2, svr_type2, dir2, rpc_type2, name2,                                                                     \
                                    cli_type3, svr_type3, dir3, rpc_type3, name3,                                                                     \
                                    cli_type4, svr_type4, dir4, rpc_type4, name4)                                                                     \
    short struct_type##_##func_name##_op(struct_type *obj, svr_type0 *name0, svr_type1 *name1, svr_type2 *name2, svr_type3 *name3, svr_type4 *name4); \
    RPC_GENERATION_DISPATCH5(struct_type, prot, op, func_name,                                                                                        \
                             cli_type0, svr_type0, dir0, rpc_type0, name0,                                                                            \
                             cli_type1, svr_type1, dir1, rpc_type1, name1,                                                                            \
                             cli_type2, svr_type2, dir2, rpc_type2, name2,                                                                            \
                             cli_type3, svr_type3, dir3, rpc_type3, name3,                                                                            \
                             cli_type4, svr_type4, dir4, rpc_type4, name4)                                                                            \
    short struct_type##_##func_name##_op(struct_type *obj, svr_type0 *name0, svr_type1 *name1, svr_type2 *name2, svr_type3 *name3, svr_type4 *name4)
