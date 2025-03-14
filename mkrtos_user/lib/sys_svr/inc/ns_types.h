#pragma once
#include "u_types.h"
#include "u_rpc_svr.h"
#include "u_slist.h"

#define NS_NODE_NAME_LEN 32

typedef enum node_type
{
    NODE_TYPE_DUMMY, //!< 虚拟节点，可以有子节点
    NODE_TYPE_SVR,   //!< 服务节点，不能有子节点
    NODE_TYPE_ROOT,  //!< ROOT节点
} node_type_t;

typedef struct ns_node
{
    char name[NS_NODE_NAME_LEN]; //!< 节点名字
    node_type_t type;            //!< 节点类型
    struct ns_node *parent;      //!< 父节点
    struct ns_node *next;        //!< 下一个
    // union
    // {
        struct ns_node *sub;  //!< 子树
        obj_handler_t svr_hd; //!< 服务节点
    // };
    int ref; //!< 引用计数
} ns_node_t;
typedef struct ns
{
    rpc_svr_obj_t svr;
    ns_node_t root_node;
} ns_t;