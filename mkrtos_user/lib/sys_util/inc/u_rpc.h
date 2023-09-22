#pragma once

#include "u_types.h"
#include "u_thread.h"
#include "u_err.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_task.h"
#include <stddef.h>
#include <sys/types.h>

#define RPC_TYPE_DEF(type)    \
    typedef struct rpc_##type \
    {                         \
        type data;            \
    } rpc_##type##_t

static inline size_t rpc_align(size_t size, size_t align)
{
    return ALIGN(size, align);
}
static inline void rpc_memcpy(void *dst, void *src, size_t size)
{
    char *_dst = dst;
    char *_src = src;

    while (size--)
    {
        *_dst = *_src;
    }
}

/*定义变量初始化宏*/
#define RPC_TYPE_INIT(data_type)                                \
    static inline void rpc_var_##data_type##_init(data_type *d) \
    {                                                           \
        uint8_t *tmp = (uint8_t *)(&(d->data));                 \
        for (int i = 0; i < sizeof(d->data); i++)               \
        {                                                       \
            tmp[i] = 0;                                         \
        }                                                       \
    }
#define RPC_TYPE_INIT_WITHOUT_IMPL(data_type) \
    static inline void rpc_var_##data_type##_init(data_type *d)
#define RPC_TYPE_INIT_FUNC_CALL(data_type, d) \
    rpc_var_##data_type##_init(d)
/*end*/

/*普通数据定义*/
#define RPC_CLI_MSG_TO_BUF(data_type, len_type)                                                \
    static inline int rpc_cli_msg_to_buf_##data_type(data_type *d, uint8_t *buf, len_type len) \
    {                                                                                          \
        if (sizeof(d->data) + rpc_align(len, __alignof(d->data)) > IPC_MSG_SIZE)               \
        {                                                                                      \
            return -ETOLONG;                                                                   \
        }                                                                                      \
        len = rpc_align(len, __alignof(d->data));                                              \
        *((typeof(d->data) *)(buf + len)) = d->data;                                           \
        return sizeof(d->data) + rpc_align(len, __alignof(d->data));                           \
    }
#define RPC_CLI_MSG_TO_BUF_WITHOUT_IMPL(data_type, len_type) \
    static inline int rpc_cli_msg_to_buf_##data_type(data_type *d, uint8_t *buf, len_type len)

#define RPC_SVR_BUF_TO_MSG(data_type, len_type)                                                              \
    static inline int rpc_svr_buf_to_msg_##data_type(data_type *d, uint8_t *buf, len_type len, len_type max) \
    {                                                                                                        \
        if (sizeof(d->data) + rpc_align(len, __alignof(d->data)) > max)                                      \
        {                                                                                                    \
            return -ETOLONG;                                                                                 \
        }                                                                                                    \
        len = rpc_align(len, __alignof(d->data));                                                            \
        d->data = *((typeof(d->data) *)(buf + len));                                                         \
        return sizeof(d->data) + rpc_align(len, __alignof(d->data));                                         \
    }

#define RPC_SVR_BUF_TO_MSG_WITHOUT_IMPL(data_type, len_type) \
    static inline int rpc_svr_buf_to_msg_##data_type(data_type *d, uint8_t *buf, len_type len, len_type max)

#define RPC_CLI_BUF_TO_MSG(data_type, len_type)                                                              \
    static inline int rpc_cli_buf_to_msg_##data_type(data_type *d, uint8_t *buf, len_type len, len_type max) \
    {                                                                                                        \
        if (sizeof(d->data) + rpc_align(len, __alignof(d->data)) > max)                                      \
        {                                                                                                    \
            return -ETOLONG;                                                                                 \
        }                                                                                                    \
        len = rpc_align(len, __alignof(d->data));                                                            \
        d->data = *((typeof(d->data) *)(buf + len));                                                         \
        return sizeof(d->data) + rpc_align(len, __alignof(d->data));                                         \
    }
#define RPC_CLI_BUF_TO_MSG_WITHOUT_IMPL(data_type, len_type) \
    static inline int rpc_cli_buf_to_msg_##data_type(data_type *d, uint8_t *buf, len_type len, len_type max)

#define RPC_SVR_MSG_TO_BUF(data_type, len_type)                                                \
    static inline int rpc_svr_msg_to_buf_##data_type(data_type *d, uint8_t *buf, len_type len) \
    {                                                                                          \
        if (sizeof(d->data) + rpc_align(len, __alignof(d->data)) > IPC_MSG_SIZE)               \
        {                                                                                      \
            return -ETOLONG;                                                                   \
        }                                                                                      \
        len = rpc_align(len, __alignof(d->data));                                              \
        *((typeof(d->data) *)(buf + len)) = d->data;                                           \
        return sizeof(d->data) + rpc_align(len, __alignof(d->data));                           \
    }
#define RPC_SVR_MSG_TO_BUF_WITHOUT_IMPL(data_type, len_type) \
    static inline int rpc_svr_msg_to_buf_##data_type(data_type *d, uint8_t *buf, len_type len)

/*end*/

#define RPC_TYPE_DEF_ALL(type)              \
    RPC_TYPE_DEF(type);                     \
    RPC_CLI_MSG_TO_BUF(rpc_##type##_t, int) \
    RPC_CLI_BUF_TO_MSG(rpc_##type##_t, int) \
    RPC_SVR_BUF_TO_MSG(rpc_##type##_t, int) \
    RPC_SVR_MSG_TO_BUF(rpc_##type##_t, int) \
    RPC_TYPE_INIT(rpc_##type##_t)

RPC_TYPE_DEF_ALL(int) //!< 定义所有的

//!< 数组定义
#define RPC_ARRAY_TYPE_DEF(len_type, data_type, length)          \
    typedef struct rpc_array_##len_type##_##data_type##_##length \
    {                                                            \
        len_type len;                                            \
        data_type data[length];                                  \
    } rpc_array_##len_type##_##data_type##_##length##_t

RPC_ARRAY_TYPE_DEF(uint32_t, uint8_t, 32);
RPC_CLI_MSG_TO_BUF_WITHOUT_IMPL(rpc_array_uint32_t_uint8_t_32_t, int)
{
    if (rpc_align(d->len, __alignof(d->len) + sizeof(d->len)) >= IPC_MSG_SIZE)
    {
        return -ETOLONG;
    }
    len = rpc_align(len, __alignof(d->len));
    *((typeof(d->len) *)(&buf[len])) = d->len;
    if (rpc_align(d->len, __alignof(d->data[0]) + d->len * sizeof(d->data[0])) >= IPC_MSG_SIZE)
    {
        return -ETOLONG;
    }
    len += sizeof(d->len);
    len = rpc_align(len, __alignof(d->data[0]));
    for (int i = 0; i < d->len * sizeof(d->data[0]); i++)
    {
        buf[i + len] = ((uint8_t *)(d->data))[i];
    }
    len += d->len * sizeof(d->data[0]);
    return len;
}
RPC_CLI_BUF_TO_MSG_WITHOUT_IMPL(rpc_array_uint32_t_uint8_t_32_t, int)
{
    if (rpc_align(d->len, __alignof(d->len) + sizeof(d->len)) >= max)
    {
        return -ETOLONG;
    }
    len = rpc_align(len, __alignof(d->len));
    d->len = *((typeof(d->len) *)(&buf[len]));
    if (rpc_align(d->len, __alignof(d->data[0]) + d->len * sizeof(d->data[0])) >= max)
    {
        return -ETOLONG;
    }
    len += sizeof(d->len);
    len = rpc_align(len, __alignof(d->data[0]));
    for (int i = 0; i < d->len * sizeof(d->data[0]); i++)
    {
        ((uint8_t *)(d->data))[i] = buf[i + len];
    }
    len += d->len * sizeof(d->data[0]);
    return len;
}
//!< end

//!< ref数组定义
#define RPC_REF_ARRAY_TYPE_DEF(len_type, data_type, length)          \
    typedef struct rpc_ref_array_##len_type##_##data_type##_##length \
    {                                                                \
        len_type len;                                                \
        data_type *data;                                             \
    } rpc_ref_array_##len_type##_##data_type##_##length##_t

RPC_REF_ARRAY_TYPE_DEF(uint32_t, uint8_t, 32);
RPC_CLI_MSG_TO_BUF_WITHOUT_IMPL(rpc_ref_array_uint32_t_uint8_t_32_t, int)
{
    if (rpc_align(d->len, __alignof(d->len) + sizeof(d->len)) >= IPC_MSG_SIZE)
    {
        return -ETOLONG;
    }
    len = rpc_align(len, __alignof(d->len));
    *((typeof(d->len) *)(&buf[len])) = d->len;
    if (rpc_align(d->len, __alignof(d->data[0]) + d->len * sizeof(d->data[0])) >= IPC_MSG_SIZE)
    {
        return -ETOLONG;
    }
    len += sizeof(d->len);
    len = rpc_align(len, __alignof(d->data[0]));
    for (int i = 0; i < d->len * sizeof(d->data[0]); i++)
    {
        buf[i + len] = ((uint8_t *)(d->data))[i];
    }
    len += d->len * sizeof(d->data[0]);
    return len;
}
RPC_CLI_BUF_TO_MSG_WITHOUT_IMPL(rpc_ref_array_uint32_t_uint8_t_32_t, int)
{
    if (rpc_align(d->len, __alignof(d->len) + sizeof(d->len)) >= max)
    {
        return -ETOLONG;
    }
    len = rpc_align(len, __alignof(d->len));
    d->len = *((typeof(d->len) *)(&buf[len]));
    if (rpc_align(d->len, __alignof(d->data[0]) + d->len * sizeof(d->data[0])) >= max)
    {
        return -ETOLONG;
    }
    len += sizeof(d->len);
    len = rpc_align(len, __alignof(d->data[0]));
    for (int i = 0; i < d->len * sizeof(d->data[0]); i++)
    {
        ((uint8_t *)(d->data))[i] = buf[i + len];
    }
    len += d->len * sizeof(d->data[0]);
    return len;
}
//!< end

//!< 映射类型定义
RPC_TYPE_DEF(obj_handler_t);
RPC_CLI_MSG_TO_BUF_WITHOUT_IMPL(rpc_obj_handler_t_t, int)
{
    if (sizeof(d->data) + rpc_align(len, __alignof(d->data)) >= IPC_MSG_SIZE)
    {
        return -ETOLONG;
    }
    len = rpc_align(len, __alignof(d->data));
    *((umword_t *)(buf + len)) = vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, d->data).raw;
    return sizeof(d->data) + rpc_align(len, __alignof(d->data));
}
RPC_CLI_BUF_TO_MSG_WITHOUT_IMPL(rpc_obj_handler_t_t, int)
{
    return len;
}
RPC_SVR_MSG_TO_BUF_WITHOUT_IMPL(rpc_obj_handler_t_t, int)
{
    return len;
}
RPC_SVR_BUF_TO_MSG_WITHOUT_IMPL(rpc_obj_handler_t_t, int)
{
    return len;
}
//!< end

#define RPC_DIR_IN 1
#define RPC_DIR_OUT 2
#define RPC_DIR_INOUT 4

#define RPC_TYPE_DATA 1
#define RPC_TYPE_BUF 2

/*客户端吧数据放到buf里面去*/
#define RPC_CLI_MSG_TO_BUF_IN(rpc_type, var_type, var, dir, buf, off)                \
    do                                                                               \
    {                                                                                \
        if (rpc_type == RPC_TYPE_DATA)                                               \
        {                                                                            \
            if (dir == RPC_DIR_IN || dir == RPC_DIR_INOUT)                           \
            {                                                                        \
                int ret = rpc_cli_msg_to_buf_##var_type(var, (uint8_t *)(buf), off); \
                if (ret < 0)                                                         \
                {                                                                    \
                    return msg_tag_init4(0, 0, 0, ret);                              \
                }                                                                    \
                off = ret;                                                           \
            }                                                                        \
        }                                                                            \
    } while (0)

/*客户端从buf里面取出数据*/
#define RPC_CLI_BUF_TO_MSG_OUT(rpc_type, var_type, var, dir, buf, off, max)               \
    do                                                                                    \
    {                                                                                     \
        if (rpc_type == RPC_TYPE_DATA)                                                    \
        {                                                                                 \
            if (dir == RPC_DIR_OUT || dir == RPC_DIR_INOUT)                               \
            {                                                                             \
                int ret = rpc_cli_buf_to_msg_##var_type(var, (uint8_t *)(buf), off, max); \
                if (ret < 0)                                                              \
                {                                                                         \
                    return msg_tag_init4(0, 0, 0, ret);                                   \
                }                                                                         \
                off = ret;                                                                \
            }                                                                             \
        }                                                                                 \
    } while (0)

/*客户端往映射缓冲区里面填充数据*/
#define PRC_CLI_FILL_MAP_BUF(rpc_type, var_type, var, dir, buf, off)                     \
    do                                                                               \
    {                                                                                \
        if (rpc_type == RPC_TYPE_BUF)                                                \
        {                                                                            \
            if (dir == RPC_DIR_IN || dir == RPC_DIR_INOUT)                           \
            {                                                                        \
                int ret = rpc_cli_msg_to_buf_##var_type(var, (uint8_t *)(buf), off); \
                if (ret < 0)                                                         \
                {                                                                    \
                    return msg_tag_init4(0, 0, 0, ret);                              \
                }                                                                    \
                off = ret;                                                           \
            }                                                                        \
        }                                                                            \
    } while (0)

/*服务端吧数据放到buf里面去*/
#define RPC_SVR_MSG_TO_BUF_IN(rpc_type, var_type, var, dir, buf, off)                \
    do                                                                               \
    {                                                                                \
        if (rpc_type == RPC_TYPE_DATA)                                               \
        {                                                                            \
            if (dir == RPC_DIR_OUT || dir == RPC_DIR_INOUT)                          \
            {                                                                        \
                int ret = rpc_svr_msg_to_buf_##var_type(var, (uint8_t *)(buf), off); \
                if (ret < 0)                                                         \
                {                                                                    \
                    return msg_tag_init4(0, 0, 0, ret);                              \
                }                                                                    \
                off = ret;                                                           \
            }                                                                        \
        }                                                                            \
    } while (0)

/*服务端从buf里面取出数据*/
#define RPC_SVR_BUF_TO_MSG_OUT(rpc_type, var_type, var, dir, buf, off, max)               \
    do                                                                                    \
    {                                                                                     \
        if (rpc_type == RPC_TYPE_DATA)                                                    \
        {                                                                                 \
            if (dir == RPC_DIR_IN || dir == RPC_DIR_INOUT)                                \
            {                                                                             \
                int ret = rpc_svr_buf_to_msg_##var_type(var, (uint8_t *)(buf), off, max); \
                if (ret < 0)                                                              \
                {                                                                         \
                    return msg_tag_init4(0, 0, 0, ret);                                   \
                }                                                                         \
                off = ret;                                                                \
            }                                                                             \
        }                                                                                 \
    } while (0)

#define RPC_GENERATION_CALL1(struct_type, op, func_name, type0, dir0, rpc_type0, name0)                                      \
    msg_tag_t struct_type##_##func_name##_call(obj_handler_t hd, type0 *var0)                                                \
    {                                                                                                                        \
        void *buf;                                                                                                           \
        ipc_msg_t *msg_ipc;                                                                                                  \
                                                                                                                             \
        thread_msg_buf_get(THREAD_MAIN, (umword_t *)(&buf), NULL);                                                           \
        msg_ipc = (ipc_msg_t *)buf;                                                                                          \
                                                                                                                             \
        int off = 0;                                                                                                         \
        int ret = -1;                                                                                                        \
        int op_val = op;                                                                                                     \
        /*拷贝op*/                                                                                                         \
        rpc_memcpy(msg_ipc->msg_buf, &op_val, sizeof(op_val));                                                               \
        off += rpc_align(sizeof(op_val), __alignof(op));                                                                     \
                                                                                                                             \
        RPC_CLI_MSG_TO_BUF_IN(rpc_type0, type0, var0, dir0, (uint8_t *)msg_ipc->msg_buf, off);                               \
        PRC_CLI_FILL_MAP_BUF(rpc_type0, type0, var0, dir0, (uint8_t *)msg_ipc->msg_buf, off);                                    \
        msg_tag_t tag = ipc_call(hd, msg_tag_init4(0, ROUND_UP(off, WORD_BYTES), 0, 0),                                      \
                                 ipc_timeout_create2(0, 0));                                                                 \
                                                                                                                             \
        if (msg_tag_get_val(tag) < 0)                                                                                        \
        {                                                                                                                    \
            return tag;                                                                                                      \
        } /*拷贝返回的数据*/                                                                                          \
        off = 0;                                                                                                             \
        RPC_CLI_BUF_TO_MSG_OUT(rpc_type0, type0, var0, dir0, (uint8_t *)msg_ipc->msg_buf, off, tag.msg_buf_len *WORD_BYTES); \
        return tag;                                                                                                          \
    }
#define RPC_GENERATION_DISPATCH1(struct_type, op, func_name,                                            \
                                 type0, dir0, rpc_type0, name0)                                         \
    msg_tag_t struct_type##_##func_name##_dispatch(struct_type *obj, msg_tag_t tag, ipc_msg_t *ipc_msg) \
    {                                                                                                   \
        type0 var0;                                                                                     \
        size_t op_val;                                                                                  \
        uint8_t *value = (uint8_t *)(ipc_msg->msg_buf);                                                 \
        int off = 0;                                                                                    \
                                                                                                        \
        RPC_TYPE_INIT_FUNC_CALL(type0, &var0);                                                          \
                                                                                                        \
        /*取得op*/                                                                                    \
        op_val = *((typeof(op) *)value);                                                                \
        off += sizeof(typeof(op));                                                                      \
        off = rpc_align(off, __alignof(typeof(op)));                                                    \
                                                                                                        \
        RPC_SVR_BUF_TO_MSG_OUT(rpc_type0, type0, &var0, dir0, value, off, tag.msg_buf_len *WORD_BYTES); \
                                                                                                        \
        short ret_val = struct_type##_##func_name##_op(obj, &var0);                                     \
                                                                                                        \
        if (ret_val < 0)                                                                                \
        {                                                                                               \
            return msg_tag_init4(0, 0, 0, ret_val);                                                     \
        }                                                                                               \
        off = 0;                                                                                        \
        RPC_SVR_MSG_TO_BUF_IN(rpc_type0, type0, &var0, dir0, value, off);                               \
        return msg_tag_init4(0, ROUND_UP(off, WORD_BYTES), 0, ret_val);                                 \
    }

#define RPC_GENERATION_OP1(struct_type, op, func_name, type0, dir0, rpc_type0, name0) \
    short struct_type##_##func_name##_op(struct_type *obj, type0 *name0)

#define RPC_GENERATION_CALL2(struct_type, op, func_name,                                                                     \
                             type0, dir0, rpc_type0, name0,                                                                  \
                             type1, dir1, rpc_type1, name1)                                                                  \
    msg_tag_t struct_type##_##func_name##_call(obj_handler_t hd, type0 *var0, type1 *var1)                                   \
    {                                                                                                                        \
        void *buf;                                                                                                           \
        ipc_msg_t *msg_ipc;                                                                                                  \
                                                                                                                             \
        thread_msg_buf_get(THREAD_MAIN, (umword_t *)(&buf), NULL);                                                           \
        msg_ipc = (ipc_msg_t *)buf;                                                                                          \
                                                                                                                             \
        int off = 0;                                                                                                         \
        int off_buf = 0;                                                                                                     \
        int ret = -1;                                                                                                        \
        int op_val = op;                                                                                                     \
        /*拷贝op*/                                                                                                         \
        rpc_memcpy(msg_ipc->msg_buf, &op_val, sizeof(op_val));                                                               \
        off += rpc_align(sizeof(op_val), __alignof(op));                                                                     \
                                                                                                                             \
        RPC_CLI_MSG_TO_BUF_IN(rpc_type0, type0, var0, dir0, (uint8_t *)msg_ipc->msg_buf, off);                               \
        PRC_CLI_FILL_MAP_BUF(rpc_type0, type0, var0, dir0, (uint8_t *)msg_ipc->msg_buf, off_buf);                                \
        RPC_CLI_MSG_TO_BUF_IN(rpc_type1, type1, var1, dir1, (uint8_t *)msg_ipc->msg_buf, off);                               \
        PRC_CLI_FILL_MAP_BUF(rpc_type1, type1, var1, dir1, (uint8_t *)msg_ipc->msg_buf, off_buf);                                \
        msg_tag_t tag = ipc_call(hd, msg_tag_init4(0, ROUND_UP(off, WORD_BYTES), 0, 0),                                      \
                                 ipc_timeout_create2(0, 0));                                                                 \
                                                                                                                             \
        if (msg_tag_get_val(tag) < 0)                                                                                        \
        {                                                                                                                    \
            return tag;                                                                                                      \
        } /*拷贝返回的数据*/                                                                                          \
        off = 0;                                                                                                             \
        RPC_CLI_BUF_TO_MSG_OUT(rpc_type0, type0, var0, dir0, (uint8_t *)msg_ipc->msg_buf, off, tag.msg_buf_len *WORD_BYTES); \
        RPC_CLI_BUF_TO_MSG_OUT(rpc_type1, type1, var1, dir1, (uint8_t *)msg_ipc->msg_buf, off, tag.msg_buf_len *WORD_BYTES); \
        return tag;                                                                                                          \
    }
