
#include "ns.h"
#include "u_types.h"
// #include "ns_types.h"

// 其他进程可以注册节点进来，并且可以注册子节点进来，子节点可以注册更多的子节点进来。
// 可以注册两种节点DUMMY和SVR节点
// 只有DUMMY节点里面可以注册SVR节点
// 查找节点时采取最大服务匹配的原则，即匹配到最后一个SVR节点时，返回最后一个SVR节点的HD，以及截断的位置。

#define NS_NODE_NAME_LEN 32

typedef enum node_type
{
    NODE_TYPE_DUMMY,
    NODE_TYPE_SVR,
} node_type_t;

typedef struct ns_node
{
    char name[NS_NODE_NAME_LEN];
    node_type_t type;
    struct ns_node *next;
    struct ns_node *sub;
    int ref;
} ns_node_t;


