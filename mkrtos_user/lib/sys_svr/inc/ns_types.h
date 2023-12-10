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
    SYM_NODE,   //!< 一个软链接节点
};
typedef struct ns_node
{
    struct ns_node *parent;             //!< 父节点
    char node_name[NAMESPACE_PATH_LEN]; //!< 节点的类型
    enum node_type type;                //!< 节点的类型
    union
    {
        struct
        {
            obj_handler_t node_hd;         //!< 存储设备ipc的hd号码
            pid_t pid;                     //!< 哪一个task注册的？
        };                                 //!< FILE_NODE &MOUNT_NODE使用该结构
        slist_head_t sub_dir;              //!< 只有DIR_NODE才有子目录
        char sym_path[NAMESPACE_PATH_LEN]; //!< SYM_NODE的软链接路径
    };
    slist_head_t node; //!< 当前节点作为子节点使用
    int ref;           //!< 引用计数
} ns_node_t;

typedef struct ns
{
    rpc_svr_obj_t svr;
    ns_node_t root_node;
} ns_t;