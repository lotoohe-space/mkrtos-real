
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "ns.h"
#ifdef MKRTOS
#include <u_hd_man.h>
#include <u_malloc.h>
#include <u_task.h>
#endif
// 其他进程可以注册节点进来，并且可以注册子节点进来，子节点可以注册更多的子节点进来。
// 可以注册两种节点DUMMY和SVR节点
// 只有DUMMY节点里面可以注册SVR节点
// 查找节点时采取最大服务匹配的原则，即匹配到最后一个SVR节点时，返回最后一个SVR节点的HD，以及截断的位置。

static ns_node_t root_node = {
    .type = NODE_TYPE_DUMMY,
};

void ns_root_node_init(obj_handler_t hd)
{
    root_node.svr_hd = hd;
}
/**
 * 从路径中copy名字
 */
static int ns_node_strcpy(char *dst, const char *src, int n)
{
    int j;

    for (j = 0; src[j] && src[j] != '/' && j < n; j++)
    {
        dst[j] = src[j];
    }
    if (j < n)
    {
        dst[j] = 0;
    }
    return j;
}
/**
 * 找到节点中的子节点
 */
static ns_node_t *ns_node_find_sub(ns_node_t *tree, const char *name)
{
    assert(name);
    if (tree == NULL)
    {
        return NULL;
    }
    ns_node_t *cur = tree->sub;

    while (cur)
    {
        if (strcmp(cur->name, name) == 0)
        {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}
int ns_nodes_count(ns_node_t *tree)
{
    int nr = 0;
    assert(tree);
    assert(tree->type == NODE_TYPE_DUMMY);

    ns_node_t *cur = tree->next;

    while (cur)
    {
        nr++;
        cur = cur->next;
    }
    return nr;
}
static int ns_node_del(ns_node_t *tree, ns_node_t *del_node)
{
    ns_node_t *tmp = tree->sub;
    ns_node_t *pre = NULL;

    while (tmp)
    {
        if (tmp == del_node)
        {
            if (pre == NULL)
            {
                // 删除第一个节点
                tree->sub = tmp->next;
                return 0;
            }
            else
            {
                pre->next = tmp->next;
                return 0;
            }
        }
        pre = tmp;
        tmp = tmp->next;
    }
    return -1;
}
static ns_node_t *node_create(const char *name, node_type_t type)
{
    ns_node_t *tmp;

#ifdef MKRTOS
    tmp = u_calloc(sizeof(ns_node_t), 1);
#else
    tmp = calloc(sizeof(ns_node_t), 1);
#endif
    if (!tmp)
    {
        return NULL;
    }
    strncpy(tmp->name, name, NS_NODE_NAME_LEN);
    tmp->name[NS_NODE_NAME_LEN - 1] = 0;
    tmp->type = type;
    return tmp;
}
ns_node_t *ns_node_get_inx(ns_node_t *tree, int inx)
{
    assert(tree);
    assert(tree->type == NODE_TYPE_DUMMY);
    int nr = 0;
    ns_node_t *cur = tree->sub;

    while (cur)
    {
        if (nr == inx)
        {
            return cur;
        }
        nr++;
        cur = cur->next;
    }
    return cur;
}
/**
 * 查找节点，如果是dummy节点，则一直循环查找。
 * 如果是svr节点，则立刻终止查找，然后返回当前节点，以及截断的位置
 */
ns_node_t *ns_node_find(ns_node_t **pnode, const char *path, int *ret, int *svr_inx, int *p_inx)
{
    assert(ret);
    assert(path);

    ns_node_t *cur_node = NULL;
    char name[NS_NODE_NAME_LEN];
    int find_node_cut_inx = 0;

    if (pnode)
    {
        *pnode = NULL;
    }
    if (path[0] != '/')
    {
        printf("[%s] path is error.\n", path);
        *ret = -EINVAL;
        return cur_node;
    }
    cur_node = &root_node;
    int i = 0;

    while (path[i])
    {
        ns_node_t *t_node;
        int len;

        // 获得节点的名字
        len = ns_node_strcpy(name, path + i + 1, NS_NODE_NAME_LEN);
        name[MIN(NS_NODE_NAME_LEN - 1, len)] = 0;
        if (len == 0)
        {
            // find_node_cut_inx++;
            goto end;
        }
        // 查找子节点
        t_node = ns_node_find_sub(cur_node, name);
        if (pnode)
        {
            *pnode = cur_node;
        }

        if (t_node)
        {
            if (t_node->type == NODE_TYPE_DUMMY)
            {
                cur_node = t_node;
            }
            else if (t_node->type == NODE_TYPE_SVR)
            {
                find_node_cut_inx = i + len + 1;
                if (path[find_node_cut_inx] == '/')
                {
                    find_node_cut_inx++;
                }
                // 找到服务节点，那直接返回，服务节点不会有子节点。
                cur_node = t_node;
                goto end;
            }
        }
        else
        {
            if (cur_node == NULL)
            {
                goto end;
            }
            cur_node = NULL;
        }
        if (p_inx)
        {
            *p_inx = i;
        }
        if (path[i] == '/')
        {
            i++;
        }
        i += len;
    }
end:
    *svr_inx = find_node_cut_inx;
    *ret = 0;
    return cur_node;
}

/**
 * 删除节点时，必须保证全路径匹配
 */
int ns_delnode(const char *path)
{
    ns_node_t *cur_node;
    int ret;
    int cur_inx;

    cur_node = ns_node_find(NULL, path, &ret, &cur_inx, NULL);
    if (cur_node == NULL)
    {
        printf("ns node not find.\n");
        return -ENOENT;
    }
    if (cur_node == NULL)
    {
        printf("ns path is error.\n");
        // 未找到，节点不是dummy节点，路径不是结尾处 则直接退出
        return -ENOENT;
    }
    if (cur_node->type == NODE_TYPE_DUMMY && cur_node->ref != 0)
    {
        printf("ns dir is not empty.\n");
        return -ENOTEMPTY;
    }
    ret = ns_node_del(cur_node->parent, cur_node);
    if (cur_node->type == NODE_TYPE_SVR)
    {
#ifdef MKRTOS
        handler_free_umap(cur_node->svr_hd);
#else
        printf("ns del node:0x%lx.\n", cur_node->svr_hd);
#endif
    }
    cur_node->parent->ref--;
#ifdef MKRTOS
    u_free(cur_node);
#else
    free(cur_node);
#endif
    return ret;
}
/**
 * 查找到完整的路径目录
 */
ns_node_t *ns_node_find_full_dir(const char *path, int *ret, int *cur_inx)
{
    assert(path);
    assert(ret);
    assert(cur_inx);
    ns_node_t *dir_node;

    dir_node = ns_node_find(NULL, path, ret, cur_inx, NULL);
    if (dir_node == NULL || dir_node->type != NODE_TYPE_DUMMY || *cur_inx != 0)
    {
#if 0
        printf("ns node not find or path error.\n");
#endif
        // 未找到，节点不是dummy节点，路径不是结尾处 则直接退出
        *ret = -ENOENT;
        return NULL;
    }

    return dir_node;
}
/**
 * 查找到完整的路径文件
 */
ns_node_t *ns_node_find_full_file(const char *path, int *ret, int *cur_inx)
{
    assert(path);
    assert(ret);
    assert(cur_inx);
    ns_node_t *dir_node;

    dir_node = ns_node_find(NULL, path, ret, cur_inx, NULL);
    if (dir_node == NULL || dir_node->type != NODE_TYPE_SVR || path[*cur_inx] != '\0')
    {
#if 0
        printf("ns node not find or path error.\n");
#endif
        // 未找到，节点不是 svr 节点，路径不是结尾处 则直接退出
        *ret = -ENOENT;
        return NULL;
    }

    return dir_node;
}
static inline int is_root_node(ns_node_t *node)
{
    return node == &root_node;
}
ns_node_t *ns_node_find_svr_file(const char *path, int *ret, int *cur_inx)
{
    assert(path);
    assert(ret);
    assert(cur_inx);
    ns_node_t *dir_node;
    ns_node_t *parent_node;

    dir_node = ns_node_find(&parent_node, path, ret, cur_inx, NULL);
    if (is_root_node(dir_node))
    {
        (*cur_inx)++;
        return dir_node;
    }
    if (dir_node == NULL || dir_node->type != NODE_TYPE_SVR)
    {
#if 0
        printf("ns node not find or path error.\n");
#endif
        if (is_root_node(parent_node))
        {
            return parent_node;
        }
        if (!parent_node)
        {
            *ret = -ENOENT;
            return NULL;
        }
        if ((parent_node->type == NODE_TYPE_DUMMY && (dir_node == NULL || dir_node->type == NODE_TYPE_DUMMY)))
        {
            return &root_node;
        }
        // 未找到，节点不是 svr 节点，路径不是结尾处 则直接退出
        *ret = -ENOENT;
        return NULL;
    }

    return dir_node;
}
int ns_find_svr_obj(const char *path, obj_handler_t *svr_hd)
{
    assert(path);
    assert(svr_hd);
    ns_node_t *svr_node;
    int ret;
    int cur_inx;

    svr_node = ns_node_find_svr_file(path, &ret, &cur_inx);
    if (svr_node == NULL || ret < 0)
    {
        return -ENOENT;
    }
#ifdef MKRTOS
    msg_tag_t tag;

    tag = task_obj_valid(TASK_THIS, svr_node->svr_hd, NULL);
    if (msg_tag_get_val(tag) < 0 || msg_tag_get_val(tag) == 0)
    {
        // 节点不存在，直接退出，删除节点
        ret = ns_delnode(path);
        return ret;
    }
#endif
    *svr_hd = svr_node->svr_hd;
    return cur_inx;
}
/**
 * 创建一个节点
 * 只能在前置节点全部为dummy的节点里面创建节点
 */
int ns_mknode(const char *path, obj_handler_t svr_hd, node_type_t type)
{
    ns_node_t *dir_node;
    ns_node_t *new_node;
    ns_node_t *cur_node;
    int ret;
    int cur_inx;
    int p_inx;
    char name[NS_NODE_NAME_LEN];

    cur_node = ns_node_find(&dir_node, path, &ret, &cur_inx, &p_inx);
    if (dir_node == NULL || dir_node->type != NODE_TYPE_DUMMY || (cur_node != NULL && cur_node->type == NODE_TYPE_SVR))
    {
        if (cur_node != NULL && cur_node->type == NODE_TYPE_SVR)
        {
            return -EEXIST;
        }
        return -ENOENT;
    }
    // 获得节点的名字
    int name_len = ns_node_strcpy(name, path + p_inx + 1, NS_NODE_NAME_LEN);

    new_node = node_create(name, NODE_TYPE_DUMMY);
    if (new_node == NULL)
    {
        return -ENOMEM;
    }
    // 加到目录的链表中
    new_node->next = dir_node->sub;
    dir_node->sub = new_node;
    new_node->type = type;
    new_node->parent = dir_node;
    if (type == NODE_TYPE_SVR)
    {
        new_node->svr_hd = svr_hd;
    }
    else
    {
        new_node->sub = NULL;
    }
    // 增加引用计数
    dir_node->ref++;
    return 0;
}
