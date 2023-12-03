#pragma once
#include "u_types.h"
#include "u_rpc_svr.h"
#include "u_slist.h"
#define NAMESPACE_PATH_LEN 32
#define NAMESAPCE_NR 8

enum node_type
{
    MOUNT_NODE,
    FILE_NODE,
    DIR_NODE,
};
typedef struct ns_node
{
    struct ns_node *parent;
    char node_name[NAMESPACE_PATH_LEN];
    enum node_type type;
    union
    {
        obj_handler_t node_hd;
        slist_head_t sub_dir;
    };
    slist_head_t node;
    int ref;
} ns_node_t;

typedef struct ns
{
    rpc_svr_obj_t svr;
    ns_node_t root_node;
    obj_handler_t hd;                        //!< 存储临时用于映射的hd
} ns_t;