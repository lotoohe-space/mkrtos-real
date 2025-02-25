#pragma once

#include <assert.h>
#include <stdint.h>

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define NS_NODE_NAME_LEN 32

typedef unsigned long obj_handler_t;

#define HANDLER_INVALID ((obj_handler_t)(-1))

typedef enum node_type
{
    NODE_TYPE_DUMMY, //!< 虚拟节点，可以有子节点
    NODE_TYPE_SVR,   //!< 服务节点，不能有子节点
} node_type_t;

typedef struct ns_node
{
    char name[NS_NODE_NAME_LEN]; //!< 节点名字
    node_type_t type;            //!< 节点类型
    struct ns_node *parent;      //!< 父节点
    struct ns_node *next;        //!< 下一个
    union
    {
        struct ns_node *sub;  //!< 子树
        obj_handler_t svr_hd; //!< 服务节点
    };
    int ref; //!< 引用计数
} ns_node_t;

ns_node_t *ns_node_find(ns_node_t **pnode, const char *path, int *ret, int *svr_inx, int *p_inx);
ns_node_t *ns_node_find_full_dir(const char *path, int *ret, int *cur_inx);
ns_node_t *ns_node_find_full_file(const char *path, int *ret, int *cur_inx);
ns_node_t *ns_node_find_svr_file(const char *path, int *ret, int *cur_inx);
int ns_nodes_count(ns_node_t *tree);
ns_node_t *ns_node_get_inx(ns_node_t *tree, int inx);
int ns_delnode(const char *path);
int ns_mknode(const char *path, obj_handler_t svr_hd, node_type_t type);
static inline ns_node_t *ns_node_get_next(ns_node_t *tree_node, ns_node_t *cur_node)
{
    assert(cur_node);
    return cur_node->next;
}
static inline ns_node_t *ns_node_get_first(ns_node_t *tree_node)
{
    assert(tree_node);
    return tree_node->sub;
}
