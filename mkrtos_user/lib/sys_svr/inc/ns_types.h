#pragma once
#include "u_types.h"
#include "u_rpc_svr.h"
#include "u_slist.h"
#define NAMESPACE_PATH_LEN 32
#define NAMESAPCE_NR 8

enum node_type
{
    MOUNT_NODE, //!< 挂载节点
    FILE_NODE,  //!< 文件节点
    DIR_NODE,   //!< 存在子目录
};
typedef struct ns_node
{
    struct ns_node *parent;
    char node_name[NAMESPACE_PATH_LEN];
    enum node_type type;
    union
    {
        struct
        {
            obj_handler_t node_hd;
            pid_t pid;
        };
        slist_head_t sub_dir;
    };
    slist_head_t node;
    int ref;
} ns_node_t;

typedef struct ns
{
    rpc_svr_obj_t svr;
    ns_node_t root_node;
} ns_t;