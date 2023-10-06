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

/**
 * @brief TODO:加锁
 *
 * @param path
 * @param split_pos
 * @return obj_handler_t
 */
static obj_handler_t find_hd(const char *path, int *split_pos)
{
    int i = 0;
    int empty = -1;
    for (i = 0; i < NS_CLI_CACHE_NR; i++)
    {
        if (ns_cli_cache.cache[i].path[0] != 0)
        {
            char *new_str = strstr(path, ns_cli_cache.cache[i].path);
            if (new_str && (new_str == path))
            {
                if (split_pos)
                {
                    *split_pos = (int)(strlen(ns_cli_cache.cache[i].path));
                }
                return ns_cli_cache.cache[i].hd;
            }
        }
    }
    return HANDLER_INVALID;
}
static bool_t reg_hd(const char *path, obj_handler_t hd, int split_inx)
{
    int i = 0;
    int empty = -1;
    for (i = 0; i < NS_CLI_CACHE_NR; i++)
    {
        if (ns_cli_cache.cache[i].path[0] == 0)
        {
            strncpy(ns_cli_cache.cache[i].path, path, split_inx);
            ns_cli_cache.cache[i].path[split_inx] = 0;
            ns_cli_cache.cache[i].hd = hd;
            return TRUE;
        }
    }
    return FALSE;
}

RPC_GENERATION_CALL2(ns_t, NS_REGISTER_OP, register,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, svr_hd)

RPC_GENERATION_CALL2(ns_t, NS_QUERY_OP, query,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_INOUT, RPC_TYPE_BUF, cli_hd)

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
    int inx = 0;
    assert(path);
    assert(svr_hd);

    obj_handler_t newfd;

    newfd = find_hd(path, &inx);
    if (newfd != HANDLER_INVALID)
    {
        *svr_hd = newfd;
        return inx;
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
        .del_map_flags = VPAGE_FLAGS_MAP,
    };

    msg_tag_t tag = ns_t_query_call(u_get_global_env()->ns_hd, &rpc_path, &rpc_svr_hd);

    if (msg_tag_get_val(tag) < 0)
    {
        handler_free(newfd);
        return msg_tag_get_val(tag);
    }
    if (reg_hd(path, newfd, msg_tag_get_val(tag)) == FALSE)
    {
        printf("The client service cache is full.\n");
        handler_free_umap(newfd);
        return -ENOMEM;
    }
    *svr_hd = newfd;
    return msg_tag_get_val(tag);
}