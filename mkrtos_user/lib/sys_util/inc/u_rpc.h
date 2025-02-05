/**
 * @file u_rpc.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "u_types.h"
#include "u_thread.h"
#include "u_err.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_task.h"
#include <stddef.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statfs.h>

typedef struct stat stat_t;
// typedef struct kstat kstat_t;
typedef struct statfs statfs_t;
typedef struct dirent dirent_t;

extern msg_tag_t dispatch_test(msg_tag_t tag, ipc_msg_t *msg);

/*rpc变量定义宏*/
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
        _dst++;
        _src++;
    }
}

/*定义变量初始化宏函数*/
#define RPC_TYPE_INIT(data_type)                                \
    static inline void rpc_var_##data_type##_init(data_type *d) \
    {                                                           \
        uint8_t *tmp = (uint8_t *)(&(d->data));                 \
        for (int i = 0; i < sizeof(d->data); i++)               \
        {                                                       \
            tmp[i] = 0;                                         \
        }                                                       \
    }
/*RPC的初始化函数定义*/
#define RPC_TYPE_INIT_WITHOUT_IMPL(data_type) \
    static inline void rpc_var_##data_type##_init(data_type *d)
/*定义一个call函数*/
#define RPC_TYPE_INIT_FUNC_CALL(data_type, d) \
    rpc_var_##data_type##_init(d)
/*end*/

/*普通数据定义*/
/**
 * @brief 客户端消息填充到buf
 *
 */
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
/**
 * @brief 服务端buf填充到消息
 *
 */
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
/**
 * @brief 服务端buf填充到消息定义
 *
 */
#define RPC_SVR_BUF_TO_MSG_WITHOUT_IMPL(data_type, len_type) \
    static inline int rpc_svr_buf_to_msg_##data_type(data_type *d, uint8_t *buf, len_type len, len_type max)

/**
 * @brief 客户端buf填充到消息
 *
 */
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

/**
 * @brief 客户端buf填充到消息定义
 *
 */
#define RPC_CLI_BUF_TO_MSG_WITHOUT_IMPL(data_type, len_type) \
    static inline int rpc_cli_buf_to_msg_##data_type(data_type *d, uint8_t *buf, len_type len, len_type max)

/**
 * @brief 服务端消息填充到buf
 *
 */
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

/**
 * @brief 服务端消息填充到buf定义
 *
 */
#define RPC_SVR_MSG_TO_BUF_WITHOUT_IMPL(data_type, len_type) \
    static inline int rpc_svr_msg_to_buf_##data_type(data_type *d, uint8_t *buf, len_type len)

/**
 * @brief 客户端rpc类型转换为服务端类型
 *
 */
#define RPC_CLI_TYPE_TRAN_TO_SVR_TYPE(cli_type, svr_type)                                        \
    static inline void rpc_cli_##cli_type##_tran_to_svr_##svr_type(cli_type *cli, svr_type *svr) \
    {                                                                                            \
        cli->data = svr->data;                                                                   \
    }

/*end*/

/**
 * @brief 该宏用于定义一个普通的rpc类型
 *
 */
#define RPC_TYPE_DEF_ALL(type)              \
    RPC_TYPE_DEF(type);                     \
    RPC_CLI_MSG_TO_BUF(rpc_##type##_t, int) \
    RPC_CLI_BUF_TO_MSG(rpc_##type##_t, int) \
    RPC_SVR_BUF_TO_MSG(rpc_##type##_t, int) \
    RPC_SVR_MSG_TO_BUF(rpc_##type##_t, int) \
    RPC_TYPE_INIT(rpc_##type##_t)           \
    RPC_CLI_TYPE_TRAN_TO_SVR_TYPE(rpc_##type##_t, rpc_##type##_t)

RPC_TYPE_DEF_ALL(int)      //!< 定义所有的
RPC_TYPE_DEF_ALL(uint32_t) //!< 定义所有的
RPC_TYPE_DEF_ALL(int64_t)  //!< 定义所有的
RPC_TYPE_DEF_ALL(uint64_t) //!< 定义所有的
RPC_TYPE_DEF_ALL(size_t)   //!< 定义所有的
RPC_TYPE_DEF_ALL(umword_t) //!< 定义所有的
RPC_TYPE_DEF_ALL(mword_t)  //!< 定义所有的
RPC_TYPE_DEF_ALL(dirent_t) //!< 目录类型
RPC_TYPE_DEF_ALL(stat_t)
// RPC_TYPE_DEF_ALL(kstat_t)
RPC_TYPE_DEF_ALL(statfs_t)

/**
 * @brief 数组类型的rpc类型定义
 *
 */
#define RPC_ARRAY_TYPE_DEF(len_type, data_type, length)          \
    typedef struct rpc_array_##len_type##_##data_type##_##length \
    {                                                            \
        len_type len;                                            \
        data_type data[length];                                  \
    } rpc_array_##len_type##_##data_type##_##length##_t

/**
 * @brief 数组类型的RPC定义，用于定义任意长度
 *
 */
#define RPC_ARRAY_DEF(len_type, data_type, length)                                              \
    RPC_ARRAY_TYPE_DEF(len_type, data_type, length);                                            \
    RPC_SVR_MSG_TO_BUF_WITHOUT_IMPL(rpc_array_##len_type##_##data_type##_##length##_t, int)     \
    {                                                                                           \
        if (rpc_align(len, __alignof(d->len)) >                                                 \
            IPC_MSG_SIZE)                                                                       \
        {                                                                                       \
            return -ETOLONG;                                                                    \
        }                                                                                       \
        len = rpc_align(len, __alignof(d->len));                                                \
        *((typeof(d->len) *)(&buf[len])) = d->len;                                              \
        if (rpc_align(len, __alignof(d->data[0]) + d->len * sizeof(d->data[0])) > IPC_MSG_SIZE) \
        {                                                                                       \
            return -ETOLONG;                                                                    \
        }                                                                                       \
        len += sizeof(d->len);                                                                  \
        len = rpc_align(len, __alignof(d->data[0]));                                            \
        for (int i = 0; i < d->len * sizeof(d->data[0]); i++)                                   \
        {                                                                                       \
            buf[i + len] = ((uint8_t *)(d->data))[i];                                           \
        }                                                                                       \
        len += d->len * sizeof(d->data[0]);                                                     \
        return len;                                                                             \
    }                                                                                           \
    RPC_SVR_BUF_TO_MSG_WITHOUT_IMPL(rpc_array_##len_type##_##data_type##_##length##_t, int)     \
    {                                                                                           \
        if (rpc_align(len, __alignof(d->len)) >                                                 \
            max)                                                                                \
        {                                                                                       \
            return -ETOLONG;                                                                    \
        }                                                                                       \
        len = rpc_align(len, __alignof(d->len));                                                \
        d->len = *((typeof(d->len) *)(&buf[len]));                                              \
        if (rpc_align(len, __alignof(d->data[0]) + d->len * sizeof(d->data[0])) > max)          \
        {                                                                                       \
            return -ETOLONG;                                                                    \
        }                                                                                       \
        len += sizeof(d->len);                                                                  \
        len = rpc_align(len, __alignof(d->data[0]));                                            \
        for (int i = 0; i < d->len * sizeof(d->data[0]); i++)                                   \
        {                                                                                       \
            ((uint8_t *)(d->data))[i] = buf[i + len];                                           \
        }                                                                                       \
        len += d->len * sizeof(d->data[0]);                                                     \
        return len;                                                                             \
    }                                                                                           \
    RPC_CLI_MSG_TO_BUF_WITHOUT_IMPL(rpc_array_##len_type##_##data_type##_##length##_t, int)     \
    {                                                                                           \
        if (rpc_align(len, __alignof(d->len)) >                                                 \
            IPC_MSG_SIZE)                                                                       \
        {                                                                                       \
            return -ETOLONG;                                                                    \
        }                                                                                       \
        len = rpc_align(len, __alignof(d->len));                                                \
        *((typeof(d->len) *)(&buf[len])) = d->len;                                              \
        if (rpc_align(len, __alignof(d->data[0]) + d->len * sizeof(d->data[0])) > IPC_MSG_SIZE) \
        {                                                                                       \
            return -ETOLONG;                                                                    \
        }                                                                                       \
        len += sizeof(d->len);                                                                  \
        len = rpc_align(len, __alignof(d->data[0]));                                            \
        for (int i = 0; i < d->len * sizeof(d->data[0]); i++)                                   \
        {                                                                                       \
            buf[i + len] = ((uint8_t *)(d->data))[i];                                           \
        }                                                                                       \
        len += d->len * sizeof(d->data[0]);                                                     \
        return len;                                                                             \
    }                                                                                           \
    RPC_CLI_BUF_TO_MSG_WITHOUT_IMPL(rpc_array_##len_type##_##data_type##_##length##_t, int)     \
    {                                                                                           \
        if (rpc_align(len, __alignof(d->len)) > max)                                            \
        {                                                                                       \
            return -ETOLONG;                                                                    \
        }                                                                                       \
        len = rpc_align(len, __alignof(d->len));                                                \
        d->len = *((typeof(d->len) *)(&buf[len]));                                              \
        if (rpc_align(len, __alignof(d->data[0]) + d->len * sizeof(d->data[0])) > max)          \
        {                                                                                       \
            return -ETOLONG;                                                                    \
        }                                                                                       \
        len += sizeof(d->len);                                                                  \
        len = rpc_align(len, __alignof(d->data[0]));                                            \
        for (int i = 0; i < d->len * sizeof(d->data[0]); i++)                                   \
        {                                                                                       \
            ((uint8_t *)(d->data))[i] = buf[i + len];                                           \
        }                                                                                       \
        len += d->len * sizeof(d->data[0]);                                                     \
        return len;                                                                             \
    }                                                                                           \
    RPC_TYPE_INIT_WITHOUT_IMPL(rpc_array_##len_type##_##data_type##_##length##_t)               \
    {                                                                                           \
    }
//!< end
/**
 * @brief 定义一个32字节长度的数组
 *
 */
RPC_ARRAY_DEF(uint32_t, uint8_t, 32)
RPC_ARRAY_DEF(uint32_t, uint8_t, 64)
RPC_ARRAY_DEF(uint32_t, uint8_t, 96)
RPC_ARRAY_DEF(uint32_t, uint8_t, 128)
RPC_ARRAY_DEF(uint32_t, uint8_t, 256)
RPC_ARRAY_DEF(uint32_t, uint8_t, 400)
RPC_ARRAY_DEF(uint32_t, uint8_t, 512)

/**
 * @brief 引用类型的数组定义，数组的数据来自其它地方
 *
 */
#define RPC_REF_ARRAY_TYPE_DEF(len_type, data_type, length)          \
    typedef struct rpc_ref_array_##len_type##_##data_type##_##length \
    {                                                                \
        len_type len;                                                \
        data_type *data;                                             \
    } rpc_ref_array_##len_type##_##data_type##_##length##_t

/**
 * @brief 引用类型的数组定义
 *
 */
#define RPC_REF_ARRAY_DEF(len_type, data_type, length)                                          \
    RPC_REF_ARRAY_TYPE_DEF(len_type, data_type, length);                                        \
    RPC_CLI_MSG_TO_BUF_WITHOUT_IMPL(rpc_ref_array_##len_type##_##data_type##_##length##_t, int) \
    {                                                                                           \
        if (rpc_align(len, __alignof(d->len)) > IPC_MSG_SIZE)                                   \
        {                                                                                       \
            return -ETOLONG;                                                                    \
        }                                                                                       \
        len = rpc_align(len, __alignof(d->len));                                                \
        *((typeof(d->len) *)(&buf[len])) = d->len;                                              \
        if (rpc_align(len, __alignof(d->data[0]) + d->len * sizeof(d->data[0])) > IPC_MSG_SIZE) \
        {                                                                                       \
            return -ETOLONG;                                                                    \
        }                                                                                       \
        len += sizeof(d->len);                                                                  \
        len = rpc_align(len, __alignof(d->data[0]));                                            \
        for (int i = 0; i < d->len * sizeof(d->data[0]); i++)                                   \
        {                                                                                       \
            buf[i + len] = ((uint8_t *)(d->data))[i];                                           \
        }                                                                                       \
        len += d->len * sizeof(d->data[0]);                                                     \
        return len;                                                                             \
    }                                                                                           \
    RPC_CLI_BUF_TO_MSG_WITHOUT_IMPL(rpc_ref_array_##len_type##_##data_type##_##length##_t, int) \
    {                                                                                           \
        if (rpc_align(len, __alignof(d->len)) >                                                 \
            max)                                                                                \
        {                                                                                       \
            return -ETOLONG;                                                                    \
        }                                                                                       \
        len = rpc_align(len, __alignof(d->len));                                                \
        d->len = *((typeof(d->len) *)(&buf[len]));                                              \
        if (rpc_align(len, __alignof(d->data[0]) + d->len * sizeof(d->data[0])) > max)          \
        {                                                                                       \
            return -ETOLONG;                                                                    \
        }                                                                                       \
        len += sizeof(d->len);                                                                  \
        len = rpc_align(len, __alignof(d->data[0]));                                            \
        for (int i = 0; i < d->len * sizeof(d->data[0]); i++)                                   \
        {                                                                                       \
            ((uint8_t *)(d->data))[i] = buf[i + len];                                           \
        }                                                                                       \
        len += d->len * sizeof(d->data[0]);                                                     \
        return len;                                                                             \
    }
//!< end

/**
 * @brief 定义一个32自己长度的引用数组
 *
 */
RPC_REF_ARRAY_DEF(uint32_t, uint8_t, 32)
RPC_REF_ARRAY_DEF(uint32_t, uint8_t, 48)
RPC_REF_ARRAY_DEF(uint32_t, uint8_t, 64)
RPC_REF_ARRAY_DEF(uint32_t, uint8_t, 96)
RPC_REF_ARRAY_DEF(uint32_t, uint8_t, 128)
RPC_REF_ARRAY_DEF(uint32_t, uint8_t, 160)
RPC_REF_ARRAY_DEF(uint32_t, uint8_t, 256)
RPC_REF_ARRAY_DEF(uint32_t, uint8_t, 400)
RPC_REF_ARRAY_DEF(uint32_t, uint8_t, 512)

/**
 * @brief Construct a new rpc type def object
 *
 */
// RPC_TYPE_DEF(obj_handler_t);
#define RPC_OBJ_HANDLER_TYPE_DEF(type) \
    typedef struct rpc_##type          \
    {                                  \
        type data;                     \
        uint8_t del_map_flags;         \
    } rpc_##type##_t

RPC_OBJ_HANDLER_TYPE_DEF(obj_handler_t);

/**
 * @brief Construct a new rpc cli msg to buf without impl object
 *
 */
RPC_CLI_MSG_TO_BUF_WITHOUT_IMPL(rpc_obj_handler_t_t, int)
{
    if (sizeof(d->data) + rpc_align(len, __alignof(d->data)) > IPC_MSG_SIZE)
    {
        return -ETOLONG;
    }
    len = rpc_align(len, __alignof(d->data));
    *((umword_t *)(buf + len)) = vpage_create_raw3(KOBJ_ALL_RIGHTS, 0xff & (~(d->del_map_flags)), d->data).raw;
    return sizeof(d->data) + rpc_align(len, __alignof(d->data));
}
/**
 * @brief Construct a new rpc cli buf to msg without impl object
 *
 */
RPC_CLI_BUF_TO_MSG_WITHOUT_IMPL(rpc_obj_handler_t_t, int)
{
    return len;
}
/**
 * @brief Construct a new rpc svr msg to buf without impl object
 *
 */
RPC_SVR_MSG_TO_BUF_WITHOUT_IMPL(rpc_obj_handler_t_t, int)
{
    if (sizeof(d->data) + rpc_align(len, __alignof(d->data)) > IPC_MSG_SIZE)
    {
        return -ETOLONG;
    }
    len = rpc_align(len, __alignof(d->data));
    *((umword_t *)(buf + len)) = vpage_create_raw3(KOBJ_ALL_RIGHTS, 0xff & (~(d->del_map_flags)), d->data).raw;
    return sizeof(d->data) + rpc_align(len, __alignof(d->data));
}
/**
 * @brief Construct a new rpc svr buf to msg without impl object
 *
 */
RPC_SVR_BUF_TO_MSG_WITHOUT_IMPL(rpc_obj_handler_t_t, int)
{
    if (sizeof(d->data) + rpc_align(len, __alignof(d->data)) > max)
    {
        return -ETOLONG;
    }
    len = rpc_align(len, __alignof(d->data));
    d->data = *((typeof(d->data) *)(buf + len));
    return sizeof(d->data) + rpc_align(len, __alignof(d->data));
}
/**
 * @brief Construct a new rpc type init without impl object
 *
 */
RPC_TYPE_INIT_WITHOUT_IMPL(rpc_obj_handler_t_t)
{
    d->data = 0;
    d->del_map_flags = 0;
}
//!< end

#define RPC_DIR_IN 1    //!< 输入类型
#define RPC_DIR_OUT 2   //!< 输出类型
#define RPC_DIR_INOUT 4 //!< 输入输出类型

#define RPC_TYPE_DATA 1 //!< 数据类型
#define RPC_TYPE_BUF 2  //!< 映射类型

/**
 * @brief 客户端吧数据放到buf里面去
 *
 */
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

/**
 * @brief 客户端往映射缓冲区里面填充数据
 *
 */
#define PRC_CLI_FILL_MAP_BUF(rpc_type, var_type, var, dir, buf, off)                 \
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

/**
 * @brief 服务端吧数据放到buf里面去
 *
 */
#define RPC_SVR_MSG_TO_BUF_OUT(rpc_type, var_type, var, dir, buf, off)               \
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

/**
 * @brief 服务端从buf里面取出数据
 *
 */
#define RPC_SVR_BUF_TO_MSG_IN(rpc_type, var_type, var, dir, buf, off, max)                \
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
/**
 * @brief 服务端从map里面取出数据
 *
 */
#define RPC_SVR_MAP_TO_MSG_IN(rpc_type, var_type, var, dir, buf, off, max)                \
    do                                                                                    \
    {                                                                                     \
        if (rpc_type == RPC_TYPE_BUF)                                                     \
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
/**
 * @brief 填充映射数据
 *
 */
#define PRC_SVR_FILL_MAP_BUF(rpc_type, var_type, var, dir, buf, off)                 \
    do                                                                               \
    {                                                                                \
        if (rpc_type == RPC_TYPE_BUF)                                                \
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

#define RPC_DISPATCH1(struct_type, prot, op_type, func0_op, func0_name)                                   \
    msg_tag_t rpc_##struct_type##_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg) \
    {                                                                                                     \
        msg_tag_t tag = msg_tag_init4(0, 0, 0, -EPROTO);                                                  \
        size_t op_val;                                                                                    \
                                                                                                          \
        op_val = *((op_type *)(ipc_msg->msg_buf));                                                        \
        switch (op_val)                                                                                   \
        {                                                                                                 \
        case func0_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func0_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        }                                                                                                 \
        return tag;                                                                                       \
    }

#define RPC_DISPATCH2(struct_type, prot, op_type, func0_op, func0_name, func1_op, func1_name)             \
    msg_tag_t rpc_##struct_type##_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg) \
    {                                                                                                     \
        msg_tag_t tag = msg_tag_init4(0, 0, 0, -EPROTO);                                                  \
        size_t op_val;                                                                                    \
                                                                                                          \
        op_val = *((op_type *)(ipc_msg->msg_buf));                                                        \
        switch (op_val)                                                                                   \
        {                                                                                                 \
        case func0_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func0_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func1_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func1_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        }                                                                                                 \
        return tag;                                                                                       \
    }

#define RPC_DISPATCH3(struct_type, prot, op_type, func0_op, func0_name, func1_op, func1_name, func2_op, func2_name) \
    msg_tag_t rpc_##struct_type##_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg)           \
    {                                                                                                               \
        msg_tag_t tag = msg_tag_init4(0, 0, 0, -EPROTO);                                                            \
        size_t op_val;                                                                                              \
                                                                                                                    \
        op_val = *((op_type *)(ipc_msg->msg_buf));                                                                  \
        switch (op_val)                                                                                             \
        {                                                                                                           \
        case func0_op:                                                                                              \
        {                                                                                                           \
            tag = struct_type##_##func0_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);                       \
        }                                                                                                           \
        break;                                                                                                      \
        case func1_op:                                                                                              \
        {                                                                                                           \
            tag = struct_type##_##func1_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);                       \
        }                                                                                                           \
        break;                                                                                                      \
        case func2_op:                                                                                              \
        {                                                                                                           \
            tag = struct_type##_##func2_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);                       \
        }                                                                                                           \
        break;                                                                                                      \
        }                                                                                                           \
        return tag;                                                                                                 \
    }

#define RPC_DISPATCH4(struct_type, prot, op_type, func0_op, func0_name, func1_op, func1_name, func2_op, func2_name, func3_op, func3_name) \
    msg_tag_t rpc_##struct_type##_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg)                                 \
    {                                                                                                                                     \
        msg_tag_t tag = msg_tag_init4(0, 0, 0, -EPROTO);                                                                                  \
        size_t op_val;                                                                                                                    \
                                                                                                                                          \
        op_val = *((op_type *)(ipc_msg->msg_buf));                                                                                        \
        switch (op_val)                                                                                                                   \
        {                                                                                                                                 \
        case func0_op:                                                                                                                    \
        {                                                                                                                                 \
            tag = struct_type##_##func0_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);                                             \
        }                                                                                                                                 \
        break;                                                                                                                            \
        case func1_op:                                                                                                                    \
        {                                                                                                                                 \
            tag = struct_type##_##func1_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);                                             \
        }                                                                                                                                 \
        break;                                                                                                                            \
        case func2_op:                                                                                                                    \
        {                                                                                                                                 \
            tag = struct_type##_##func2_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);                                             \
        }                                                                                                                                 \
        break;                                                                                                                            \
        case func3_op:                                                                                                                    \
        {                                                                                                                                 \
            tag = struct_type##_##func3_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);                                             \
        }                                                                                                                                 \
        break;                                                                                                                            \
        }                                                                                                                                 \
        return tag;                                                                                                                       \
    }

#define RPC_DISPATCH5(struct_type, prot, op_type, func0_op, func0_name, func1_op, func1_name,             \
                      func2_op, func2_name, func3_op, func3_name,                                         \
                      func4_op, func4_name)                                                               \
    msg_tag_t rpc_##struct_type##_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg) \
    {                                                                                                     \
        msg_tag_t tag = msg_tag_init4(0, 0, 0, -EPROTO);                                                  \
        size_t op_val;                                                                                    \
                                                                                                          \
        op_val = *((op_type *)(ipc_msg->msg_buf));                                                        \
        switch (op_val)                                                                                   \
        {                                                                                                 \
        case func0_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func0_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func1_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func1_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func2_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func2_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func3_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func3_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func4_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func4_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        }                                                                                                 \
        return tag;                                                                                       \
    }
#define RPC_DISPATCH12(struct_type, prot, op_type, func0_op, func0_name, func1_op, func1_name,            \
                       func2_op, func2_name, func3_op, func3_name,                                        \
                       func4_op, func4_name, func5_op, func5_name, func6_op, func6_name,                  \
                       func7_op, func7_name, func8_op, func8_name, func9_op, func9_name,                  \
                       func10_op, func10_name, func11_op, func11_name)                                    \
    msg_tag_t rpc_##struct_type##_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg) \
    {                                                                                                     \
        msg_tag_t tag = msg_tag_init4(0, 0, 0, -EPROTO);                                                  \
        size_t op_val;                                                                                    \
                                                                                                          \
        op_val = *((op_type *)(ipc_msg->msg_buf));                                                        \
        switch (op_val)                                                                                   \
        {                                                                                                 \
        case func0_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func0_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func1_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func1_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func2_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func2_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func3_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func3_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func4_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func4_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func5_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func5_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func6_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func6_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func7_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func7_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func8_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func8_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func9_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func9_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func10_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func10_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func11_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func11_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        default:                                                                                          \
            break;                                                                                        \
        }                                                                                                 \
        return tag;                                                                                       \
    }
#define RPC_DISPATCH13(struct_type, prot, op_type, func0_op, func0_name, func1_op, func1_name,            \
                       func2_op, func2_name, func3_op, func3_name,                                        \
                       func4_op, func4_name, func5_op, func5_name, func6_op, func6_name,                  \
                       func7_op, func7_name, func8_op, func8_name, func9_op, func9_name,                  \
                       func10_op, func10_name, func11_op, func11_name, func12_op, func12_name)            \
    msg_tag_t rpc_##struct_type##_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg) \
    {                                                                                                     \
        msg_tag_t tag = msg_tag_init4(0, 0, 0, -EPROTO);                                                  \
        size_t op_val;                                                                                    \
                                                                                                          \
        op_val = *((op_type *)(ipc_msg->msg_buf));                                                        \
        switch (op_val)                                                                                   \
        {                                                                                                 \
        case func0_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func0_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func1_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func1_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func2_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func2_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func3_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func3_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func4_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func4_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func5_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func5_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func6_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func6_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func7_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func7_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func8_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func8_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func9_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func9_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func10_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func10_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func11_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func11_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func12_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func12_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        default:                                                                                          \
            break;                                                                                        \
        }                                                                                                 \
        return tag;                                                                                       \
    }
#define RPC_DISPATCH14(struct_type, prot, op_type, func0_op, func0_name, func1_op, func1_name,            \
                       func2_op, func2_name, func3_op, func3_name,                                        \
                       func4_op, func4_name, func5_op, func5_name, func6_op, func6_name,                  \
                       func7_op, func7_name, func8_op, func8_name, func9_op, func9_name,                  \
                       func10_op, func10_name, func11_op, func11_name, func12_op, func12_name,            \
                       func13_op, func13_name)                                    \
    msg_tag_t rpc_##struct_type##_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg) \
    {                                                                                                     \
        msg_tag_t tag = msg_tag_init4(0, 0, 0, -EPROTO);                                                  \
        size_t op_val;                                                                                    \
                                                                                                          \
        op_val = *((op_type *)(ipc_msg->msg_buf));                                                        \
        switch (op_val)                                                                                   \
        {                                                                                                 \
        case func0_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func0_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func1_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func1_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func2_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func2_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func3_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func3_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func4_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func4_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func5_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func5_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func6_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func6_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func7_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func7_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func8_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func8_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func9_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func9_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func10_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func10_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func11_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func11_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func12_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func12_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func13_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func13_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        default:                                                                                          \
            break;                                                                                        \
        }                                                                                                 \
        return tag;                                                                                       \
    }
#define RPC_DISPATCH15(struct_type, prot, op_type, func0_op, func0_name, func1_op, func1_name,            \
                       func2_op, func2_name, func3_op, func3_name,                                        \
                       func4_op, func4_name, func5_op, func5_name, func6_op, func6_name,                  \
                       func7_op, func7_name, func8_op, func8_name, func9_op, func9_name,                  \
                       func10_op, func10_name, func11_op, func11_name, func12_op, func12_name,            \
                       func13_op, func13_name, func14_op, func14_name)                                    \
    msg_tag_t rpc_##struct_type##_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg) \
    {                                                                                                     \
        msg_tag_t tag = msg_tag_init4(0, 0, 0, -EPROTO);                                                  \
        size_t op_val;                                                                                    \
                                                                                                          \
        op_val = *((op_type *)(ipc_msg->msg_buf));                                                        \
        switch (op_val)                                                                                   \
        {                                                                                                 \
        case func0_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func0_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func1_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func1_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func2_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func2_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func3_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func3_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func4_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func4_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func5_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func5_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func6_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func6_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func7_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func7_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func8_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func8_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func9_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func9_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func10_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func10_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func11_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func11_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func12_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func12_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func13_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func13_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func14_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func14_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        default:                                                                                          \
            break;                                                                                        \
        }                                                                                                 \
        return tag;                                                                                       \
    }
#define RPC_DISPATCH18(struct_type, prot, op_type, func0_op, func0_name, func1_op, func1_name,            \
                       func2_op, func2_name, func3_op, func3_name,                                        \
                       func4_op, func4_name, func5_op, func5_name, func6_op, func6_name,                  \
                       func7_op, func7_name, func8_op, func8_name, func9_op, func9_name,                  \
                       func10_op, func10_name, func11_op, func11_name, func12_op, func12_name,            \
                       func13_op, func13_name, func14_op, func14_name, func15_op, func15_name,            \
                       func16_op, func16_name, func17_op, func17_name)                                    \
    msg_tag_t rpc_##struct_type##_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg) \
    {                                                                                                     \
        msg_tag_t tag = msg_tag_init4(0, 0, 0, -EPROTO);                                                  \
        size_t op_val;                                                                                    \
                                                                                                          \
        op_val = *((op_type *)(ipc_msg->msg_buf));                                                        \
        switch (op_val)                                                                                   \
        {                                                                                                 \
        case func0_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func0_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func1_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func1_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func2_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func2_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func3_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func3_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func4_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func4_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func5_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func5_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func6_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func6_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func7_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func7_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func8_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func8_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func9_op:                                                                                    \
        {                                                                                                 \
            tag = struct_type##_##func9_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);             \
        }                                                                                                 \
        break;                                                                                            \
        case func10_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func10_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func11_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func11_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func12_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func12_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func13_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func13_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func14_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func14_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func15_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func15_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func16_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func16_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        case func17_op:                                                                                   \
        {                                                                                                 \
            tag = struct_type##_##func17_name##_dispatch((struct_type *)obj, in_tag, ipc_msg);            \
        }                                                                                                 \
        break;                                                                                            \
        default:                                                                                          \
            break;                                                                                        \
        }                                                                                                 \
        return tag;                                                                                       \
    }
#include "u_rpc_1.h"
#include "u_rpc_2.h"
#include "u_rpc_3.h"
#include "u_rpc_4.h"
#include "u_rpc_5.h"
#include "u_rpc_6.h"