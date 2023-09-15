
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_util.h"
#include <assert.h>
#include <stdio.h>
#include <slist.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

typedef enum tree_node_type
{
    TREE_NODE_ENTITY = 0,
    TREE_NODE_TREE = 1,
} tree_node_type_t;

typedef struct tree_node
{
    char *name; //!< 节点的名字
    union
    {
        slist_head_t list; //!< 双向链表
        struct
        {
            struct tree_node *next_tree; //!< 下一级树
            struct tree_node *parent;    //!< 父节点
        };
    };
    tree_node_type_t type; //!< 节点类型
    umword_t hlink;        //!< 硬链接数量
    obj_handler_t obj;     //!< 目标对象索引号
} tree_node_t;

bool_t tree_node_init(tree_node_t *node, const char *name, tree_node_type_t type)
{
    assert(node);
    assert(name);
    char *new_name = strdup(name);
    if (!new_name)
    {
        return FALSE;
    }
    node->name = new_name;
    switch (type)
    {
    case TREE_NODE_ENTITY:
        slist_init(&node->list);
        break;
    case TREE_NODE_TREE:
        node->next_tree = NULL;
        node->parent = NULL;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}
void tree_node_add_raw(tree_node_t *head, tree_node_t *node, tree_node_type_t type)
{
    assert(head);
    assert(node);
    switch (type)
    {
    case TREE_NODE_ENTITY:
        slist_add(head, &node->list);
        break;
    case TREE_NODE_TREE:
        head->next_tree = node;
        head->parent = head;
        break;
    }
}

/**
 * @brief 比较两个字符串，b2只比较指定长度
 *
 * @param b1
 * @param b2
 * @param b2len
 * @return int
 */
int strcmp_l(const char *b1, const char *b2, size_t b2len)
{
    int i;

    for (i = 0; b1[i] && b2[i] && i < b2len; i++)
    {
        if (b1[i] - b2[i] != 0)
        {
            return b1[i] - b2[i];
        }
    }
    if (b2len == i)
    {
        return 0;
    }
    return -1;
}
/**
 * @brief 在树中找到需要的节点
 *
 * @param head
 * @param name
 * @param len
 * @param node
 * @return tree_node_t*
 */
tree_node_t *tree_node_find(tree_node_t *head, const char *name, int len, tree_node_type_t node)
{
    tree_node_t *pos;
    assert(head->type == TREE_NODE_TREE);

    slist_foreach(pos, &head->list, list)
    {
        if (pos->type == node &&
            strcmp_l(pos->name, name, len) == 0)
        {
            return pos;
        }
    }
    return NULL;
}

typedef struct mk_file
{
    tree_node_t *node; //!< 打开的node
    int flags;         //!< 操作的标记
    int mode;          //!< 操作的模式
    int offset;        //!< 操作偏移
    int used;          //!< 是否在使用
} mk_file_t;
#define MK_FILES_NR 8 //!< 最多支持打开多少文件
static mk_file_t mk_files[MK_FILES_NR];
mk_file_t *mk_file_alloc(int *inx)
{
    //!< TODO:多线程情况下需要加锁
    for (int i = 0; i < MK_FILES_NR; i++)
    {
        if (mk_files[i].used == 0)
        {
            mk_files[i].used = 1;
            *inx = i;
            return &mk_files[i];
        }
    }
    return NULL;
}
void mk_file_free(int inx)
{
    mk_files[inx].used = 0;
}

char *pwd = "/";                           //! 当前节点
static tree_node_t root_node;              //!< 根节点
static tree_node_t *pwd_node = &root_node; //!< 当前节点

int path_lookup(tree_node_t *dir, const char *name, int len, tree_node_t **result)
{
    if (len == 2 && name[0] == '.' && name[1] == '.')
    {
        if (dir == &root_node) //!< 根目录的上一个就是自己
        {
            *result = dir;
            return 0;
        }
        else if (0)
        {
            /*TODO:如果是一个挂载点*/
        }
    }
    if (dir->type != TREE_NODE_TREE)
    {
        return -ENOTDIR;
    }
    *result = tree_node_find(dir, name, len, TREE_NODE_TREE);
    if (*result == NULL)
    {
        return -ENOENT;
    }
    return 0;
}
tree_node_t *path_open_dir_namei(
    tree_node_t *dir,
    const char *name,
    char **res_name,
    int *res_len,
    int *err)
{
    tree_node_t *cur_node = dir;
    int inx = 0;
    int find_len = 0;

    if (cur_node == NULL)
    {
        dir = pwd_node;
        //!< TODO:引用计数+1
    }

    if (name[inx] == '/' || name[inx] == '\\')
    {
        cur_node = &root_node;
        //!< TODO:引用计数+1
        inx++;
    }
    else
    {
        cur_node = pwd_node;
        //!< TODO:引用计数+1
    }
    while (name[inx])
    {
        find_len = 0;
        if (name[inx] != 0 && name[inx] != '/' && name[inx] != '\\')
        {
            find_len++;
            inx++;
        }
        *res_len = find_len;
        *res_name = &name[inx];
        int ret = path_lookup(cur_node, &name[inx], find_len, &cur_node);
        if (ret < 0)
        {
            if (ret == -ENOTDIR)
            {
                return cur_node;
            }
            else
            {
                *err = ret;
                return NULL;
            }
        }
    }
    *err = 0;
    return cur_node;
}
int path_open_nami(const char *path, int flags, int mode, tree_node_t **res_node)
{
    int err;
    char *name;
    int len;
    int res;
    tree_node_t *file_node;
    tree_node_t *node = path_open_dir_namei(NULL, path, &name, &len, &err);

    if (err < 0)
    {
        return err;
    }
    if (node->type != TREE_NODE_TREE)
    {
        return -ENOTDIR;
    }
    if (len == 0)
    {
        if ((flags & O_WRONLY) || (flags & O_RDWR))
        {
            // TOOD:puti(node)
            return -EISDIR;
        }
        *res_node = node;
        return 0;
    }
    if (flags & O_CREAT)
    {
        res = path_lookup(node, name, len, &file_node);
        if (res >= 0)
        {
            if (flags & O_EXCL)
            {
                // puti(file_node);
                return -EEXIST;
            }
        }
        else
        {
            // TODO: 创建新的节点
            return -ENOSYS;
        }
    }
    else
    {
        res = path_lookup(node, name, len, &file_node);
        if (res < 0)
        {
            // TODO: puti(dir);
            return -ENOENT;
        }
    }
    if (flags & O_TRUNC)
    {
        return -ENOSYS;
    }
    *res_node = file_node;
    return res;
}
int path_open(const char *path, int flags, mode_t mode)
{
    int inx = 0;
    mk_file_t *file = mk_file_alloc(&inx);
    if (!file)
    {
        return -EMFILE;
    }
    tree_node_t *node;

    int res = path_open_nami(path, flags, mode, &node);
    if (res < 0)
    {
        mk_file_free(inx);
        return res;
    }
    /*TODO:调用打开接口*/
    if (msg_tag_get_val(task_obj_valid(TASK_THIS, node->obj)) <= 0)
    {
        /*TODO:obj已经无效，删除这个文件节点*/
        mk_file_free(inx);
        return -ECANCELED;
    }
    else
    {
        // 映射给open的操作者
    }
    file->flags = flags;
    file->mode = mode;
    file->offset = 0;
    file->node = node;
    return inx;
}
// 路径管理器大纲
// 路径管理器，所有的非固化存储的资源
// 支持mount其它文件系统，其它文件系统也是一个服务对象
// 支持mount单个服务对象。
// 打开一个文件时，首先在路径管理器中找到文件系统服务的对象，
// 然后调用文件系统服务对象的open接口打开文件，该文件系统返回一个句柄。
// 之后的操作都直接对文件系统服务对象进行操作，不在经过路径管理器。

// 路径管理器也可以映射其它任意对象，但这些对象可能不具备读写等操作，具体操作由使用者负责。

int main(int argc, char *args[])
{
    // printf("shell>\n");
    ulog_write_str(LOG_PROT, "MKRTOS:\n");
    while (1)
        ;
    return 0;
}