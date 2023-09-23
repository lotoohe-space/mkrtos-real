#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "ns_types.h"
#include "rpc_prot.h"
#include "u_env.h"
#include "u_prot.h"
#include "u_hd_man.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define NS_CLI_CACHE_NR 8 //!< 一个客户端最多可以请求8个服务
typedef struct ns_cli_entry
{
    char path[NAMESPACE_PATH_LEN];
    obj_handler_t hd;
} ns_cli_entry_t;

typedef struct ns_cli_cache
{
    ns_cli_entry_t cache[NS_CLI_CACHE_NR];
} ns_cli_cache_t;

static ns_cli_cache_t ns_cli_cache;

static obj_handler_t find_hd(const char *path)
{
    int i = 0;
    int empty = -1;
    for (i = 0; i < NS_CLI_CACHE_NR; i++)
    {
        if (ns_cli_cache.cache[i].path[0] != 0)
        {
            if (strcmp(ns_cli_cache.cache[i].path, path) == 0)
            {
                return ns_cli_cache.cache[i].hd;
            }
        }
    }
    return HANDLER_INVALID;
}
static bool_t reg_hd(const char *path, obj_handler_t hd)
{
    int i = 0;
    int empty = -1;
    for (i = 0; i < NS_CLI_CACHE_NR; i++)
    {
        if (ns_cli_cache.cache[i].path[0] == 0)
        {
            strcpy(ns_cli_cache.cache[i].path, path);
            ns_cli_cache.cache[i].hd = hd;
            return TRUE;
        }
    }
    return FALSE;
}

RPC_GENERATION_CALL2(ns_t, NS_REGISTER_OP, register,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, svr_hd)

// RPC_GENERATION_CALL2(ns_t, NS_QUERY_OP, query,
//                      rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
//                      rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_INOUT, RPC_TYPE_BUF, cli_hd)
msg_tag_t ns_t_query_call(obj_handler_t hd, rpc_ref_array_uint32_t_uint8_t_32_t *var0, rpc_obj_handler_t_t *var1)
{
    void *buf;
    ipc_msg_t *msg_ipc;
    thread_msg_buf_get(2, (umword_t *)(&buf), ((void *)0));
    msg_ipc = (ipc_msg_t *)buf;
    int off = 0;
    int off_buf = 0;
    int ret = -1;
    size_t op_val = ((uint16_t)1);
    rpc_memcpy(msg_ipc->msg_buf, &op_val, sizeof(op_val));
    off += rpc_align(sizeof(op_val), __alignof(((uint16_t)1)));
    do
    {
        if (1 == 1)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_ref_array_uint32_t_uint8_t_32_t(var0, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 2)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_ref_array_uint32_t_uint8_t_32_t(var0, (uint8_t *)((uint8_t *)msg_ipc->map_buf), off_buf);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_buf = ret;
            }
        }
    } while (0);
    do
    {
        if (2 == 1)
        {
            if (4 == 1 || 4 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_obj_handler_t_t(var1, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (2 == 2)
        {
            if (4 == 1 || 4 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_obj_handler_t_t(var1, (uint8_t *)((uint8_t *)msg_ipc->map_buf), off_buf);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_buf = ret;
            }
        }
    } while (0);
    msg_tag_t tag = ipc_call(hd, ((msg_tag_t){.flags = (0), .msg_buf_len = ((((off) / ((sizeof(void *)))) + (((off) % ((sizeof(void *)))) ? 1 : 0))), .map_buf_len = (0), .prot = (0)}), ipc_timeout_create2(0, 0));
    if (((int16_t)((tag).prot)) < 0)
    {
        return tag;
    }
    off = 0;
    do
    {
        if (1 == 1)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_cli_buf_to_msg_rpc_ref_array_uint32_t_uint8_t_32_t(var0, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (2 == 1)
        {
            if (4 == 2 || 4 == 4)
            {
                int ret = rpc_cli_buf_to_msg_rpc_obj_handler_t_t(var1, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    return tag;
}
int ns_register(const char *path, obj_handler_t svr_hd)
{
    assert(path);

    rpc_ref_array_uint32_t_uint8_t_32_t rpc_path = {
        .data = path,
        .len = strlen(path) + 1,
    };
    rpc_obj_handler_t_t rpc_svr_hd = {
        .data = svr_hd,
    };

    msg_tag_t tag = ns_t_register_call(u_get_global_env()->ns_hd, &rpc_path, &rpc_svr_hd);

    return msg_tag_get_val(tag);
}
int ns_query(const char *path, obj_handler_t *svr_hd)
{
    assert(path);
    assert(svr_hd);

    obj_handler_t newfd;

    newfd = find_hd(path);
    if (newfd != HANDLER_INVALID)
    {
        return newfd;
    }

    newfd = handler_alloc();

    if (newfd == HANDLER_INVALID)
    {
        return -ENOENT;
    }

    rpc_ref_array_uint32_t_uint8_t_32_t rpc_path = {
        .data = path,
        .len = strlen(path) + 1,
    };
    rpc_obj_handler_t_t rpc_svr_hd = {
        .data = newfd,
    };

    msg_tag_t tag = ns_t_query_call(u_get_global_env()->ns_hd, &rpc_path, &rpc_svr_hd);

    if (msg_tag_get_val(tag) < 0)
    {
        handler_free(newfd);
        return msg_tag_get_val(tag);
    }
    if (reg_hd(path, newfd) == FALSE)
    {
        printf("客户端服务缓存区已满.\n");
        handler_free_umap(newfd);
        return -ENOMEM;
    }
    *svr_hd = newfd;
    return msg_tag_get_val(tag);
}