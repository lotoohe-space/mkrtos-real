#pragma once
#include "u_types.h"
#include "u_rpc_svr.h"
#define NAMESPACE_PATH_LEN 32
#define NAMESAPCE_NR 32

typedef struct namespace_entry
{
    char path[NAMESPACE_PATH_LEN]; //!< 服务的路径名
    obj_handler_t hd;              //!< 服务的fd
} namespace_entry_t;

typedef struct ns
{
    rpc_svr_obj_t svr;
    namespace_entry_t ne_list[NAMESAPCE_NR]; //!< 服务列表
    obj_handler_t hd;
    obj_handler_t ipc_hd;          //!< 服务的ipc hd
} ns_t;