/**
 * @file namespace.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-11-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <u_types.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include "u_hd_man.h"
#include "u_ipc.h"
#include "ns_types.h"
#include "ns_svr.h"
#include "fs_svr.h"
#include "namespace.h"
#include "u_rpc_svr.h"
#include "file_desc.h"
#include <malloc.h>
#include <fcntl.h>
static ns_t ns;
static fs_t ns_fs;
int ns_reg(const char *path, obj_handler_t hd, enum node_type type);

static ns_node_t *node_init(ns_node_t *new_node, ns_node_t *parent, const char *name, obj_handler_t hd, enum node_type type)
{
    strncpy(new_node->node_name, name, sizeof(new_node->node_name));
    new_node->node_name[sizeof(new_node->node_name) - 1] = 0;
    new_node->ref = 1;
    new_node->type = type;
    new_node->parent = parent;
    if (parent)
    {
        parent->ref++; //! 父目录的引用计数+1
    }
    slist_init(&new_node->node);
    if (type == DIR_NODE)
    {
        slist_init(&new_node->sub_dir);
    }
    else
    {
        new_node->node_hd = hd;
    }
    return new_node;
}
/**
 * @brief Create a node object
 *
 * @param dir
 * @param name
 * @param hd
 * @param typef
 * @return ns_node_t*
 */
static ns_node_t *create_node(ns_node_t *parent, const char *name, obj_handler_t hd, enum node_type type)
{
    ns_node_t *new_node = malloc(sizeof(*new_node));

    if (new_node == NULL)
    {
        return NULL;
    }
    node_init(new_node, parent, name, hd, type);
    return new_node;
}
/**
 * @brief 路径分割
 *
 * @param name
 * @return int
 */
static int path_split(const char *name)
{
    int i = -1;

    for (i = 0; name[i]; i++)
    {
        if (name[i] == '/')
        {
            break;
        }
    }
    return i;
}
/**
 * @brief 找到某个节点
 *
 * @param dir
 * @param name
 * @param ret_inx
 * @return ns_node_t*
 */
static ns_node_t *node_lookup(ns_node_t *dir, const char *name, size_t *ret_inx)
{
    int inx = -1;
    int find_inx = 0;
    int r_inx = 0;
    ns_node_t *node = NULL;
    bool_t find = FALSE;

    if (dir->type != DIR_NODE)
    {
        return NULL;
    }
    if (name[0] == 0)
    {
        return dir;
    }
    if (name[0] == '/')
    {
        node = dir;
        find_inx++;
        r_inx++;
        find = TRUE;
    }

    while (1)
    {
        ns_node_t *pos;

        inx = path_split(name + find_inx);
        if (inx <= 0)
        {
            break;
        }
        slist_foreach(pos, &(node->sub_dir), node)
        {
            if (strncmp(name + find_inx, pos->node_name, inx) != 0)
            {
                continue;
            }
            switch (pos->type)
            {
            case DIR_NODE:
            {
                // dir
                node = pos;
                r_inx += inx;
                find = TRUE;
            }
            break;
            case FILE_NODE:
            case MOUNT_NODE:
            {
                find = TRUE;
                r_inx += inx;
                node = pos;
                goto end;
            }
            break;
            }
        }
        find_inx += inx;
    }
end:
    if (!find)
    {
        return NULL;
    }
    if (ret_inx)
    {
        *ret_inx = r_inx;
    }
    return node;
}
/**
 * @brief 释放一个节点
 *
 * @param node
 */
int ns_node_free(ns_node_t *node)
{
    if (!node)
    {
        return 0;
    }
    if (node->ref == 1)
    {
        //!< 父级目录的引用计数-1
        ns_node_free(node->parent);
    }
    node->ref--;
    if (node->ref <= 0)
    {
        switch (node->type)
        {
        case DIR_NODE:
            assert(slist_is_empty(&node->sub_dir));
            break;
        case FILE_NODE:
        case MOUNT_NODE:
            handler_free_umap(node->node_hd);
            break;
        default:
            break;
        }
        if (slist_in_list(&node->node))
        {
            slist_del(&node->node);
        }
        free(node);
    }
    return node->ref;
}
int fs_svr_open(const char *path, int flags, int mode)
{
    ns_node_t *node;
    size_t ret_inx;
    int len;

again:
    node = node_lookup(&ns.root_node, path, &ret_inx);
    if (!node)
    {
        return -ENOENT;
    }
    len = strlen(path);
    if (len != ret_inx)
    {
        if (flags & O_CREAT)
        {
            int ret = ns_reg(path, 0, DIR_NODE);

            if (ret < 0)
            {
                return ret;
            }
            goto again;
        }
        return -ENOENT;
    }
    file_desc_t *fd_p = fd_alloc(node);

    if (!fd_p)
    {
        return -ENOMEM;
    }
    node->ref++;
    return 0;
}
int fs_svr_readdir(int fd, dirent_t *dir)
{
    bool_t is_dec_ref = 0;
    file_desc_t *fdp = fd_get(fd);

    if (!fdp)
    {
        return -ENOENT;
    }
    if (fdp->node_iter == (void *)((umword_t)(-1)))
    {
        return -ENOENT;
    }
    if (fdp->node_iter == NULL)
    {
        // 首次迭代
        if (slist_is_empty(&fdp->node->sub_dir))
        {
            return -ENOENT;
        }
        // 获得第一个节点
        fdp->node_iter = container_of(slist_first(&fdp->node->sub_dir), ns_node_t, node);
        fdp->node_iter->ref++;
    }

    // 拷贝数据
    dir->d_type = fdp->node_iter->type == FILE_NODE ? DT_CHR : DT_DIR;
    dir->d_reclen = sizeof(*dir);
    strncpy(dir->d_name, fdp->node_iter->node_name, sizeof(dir->d_name));
    dir->d_name[sizeof(dir->d_name) - 1] = 0;
    dir->d_ino = 0;
    slist_head_t *next = fdp->node_iter->node.next;

    ns_node_free(fdp->node_iter);
    if (next == &fdp->node->sub_dir)
    {
        // 到达结尾
        fdp->node_iter = (void *)((umword_t)(-1));
        return sizeof(*dir);
    }
    else
    {
        ns_node_t *next_n = container_of(next, ns_node_t, node);

        fdp->node_iter = next_n;
        next_n->ref++;
    }
    return sizeof(*dir);
}
void fs_svr_close(int fd)
{
    file_desc_t *fdp = fd_get(fd);

    if (!fdp)
    {
        return;
    }
    if (fdp->node_iter && fdp->node_iter != (void *)(-1))
    {
        // 最后一个迭代的节点需要删除
        ns_node_free(fdp->node_iter);
    }
    ns_node_free(fdp->node);
    fd_free(fd);
}
int fs_svr_unlink(char *path)
{
    ns_node_t *node;
    int ret_inx;

    node = node_lookup(&ns.root_node, path, &ret_inx);
    if (node && ret_inx == strlen(path))
    {
        if (node->ref == 1)
        {
            ns_node_free(node);
        }
    }
    else
    {
        return -ENOENT;
    }

    return 0;
}

static int find_path(const char *name)
{
    int len = strlen(name);

    for (int i = len; i >= 0; i--)
    {
        if (name[i] == '/')
        {
            return i + 1;
        }
    }
    return -1;
}
/**
 * @brief 注册一个节点
 *
 * @param path
 * @param hd
 * @return int
 */
int ns_reg(const char *path, obj_handler_t hd, enum node_type type)
{
    size_t ret_inx;
    ns_node_t *new_node;
    ns_node_t *node;

    node = node_lookup(&ns.root_node, path, &ret_inx);
    if (node && ret_inx == strlen(path))
    {
        handler_free_umap(hd);
        return -EEXIST;
    }
    int inx = find_path(path);

    if (ret_inx != inx)
    {
        handler_free_umap(hd);
        return -ENOENT;
    }
    new_node = create_node(node, path + inx, hd, type);
    if (!new_node)
    {
        handler_free_umap(hd);
        return -ENOMEM;
    }

    slist_add_append(&node->sub_dir, &new_node->node);

    return 0;
}
static obj_handler_t ns_hd;
void namespace_init(obj_handler_t ipc_hd)
{
    ns_init(&ns);
    fs_init(&ns_fs);
    meta_reg_svr_obj(&ns.svr, NS_PROT);
    meta_reg_svr_obj(&ns_fs.svr, FS_PROT);
    node_init(&ns.root_node, NULL, "", 0, DIR_NODE);
    ns_hd = ipc_hd;
    printf("ns svr init...\n");
}

/**
 * @brief 注册一个obj
 *
 * @param path 注册的路径
 * @param hd 注册的hd
 * @return int
 */
int namespace_register(const char *path, obj_handler_t hd, int type)
{
    if (type == DIR_NODE)
    {
        handler_free_umap(hd);
        namespace_pre_alloc_map_fd();
        return -ECANCELED;
    }
    int ret = ns_reg(path, hd, type); // TODO:增加类型支持
    printf("register svr, name is %s, hd is %d\n", path, hd);
    namespace_pre_alloc_map_fd();
    return ret;
}
/**
 * @brief 申请一个obj
 *
 * @param path
 * @param hd
 * @return int
 */
int namespace_query(const char *path, obj_handler_t *hd)
{
    assert(hd);
    size_t ret_inx;
    ns_node_t *new_node;
    ns_node_t *node;

    if (path[0] == '/' && path[1] == 0)
    {
        *hd = ns_hd;
        return 0;
    }

    node = node_lookup(&ns.root_node, path, &ret_inx);
    if (node && ret_inx == strlen(path))
    {
        return -EEXIST;
    }
    if (node->type == DIR_NODE)
    {
        return -ENOENT;
    }
    *hd = node->node_hd;
    return ret_inx;
}
int namespace_pre_alloc_map_fd(void)
{
    ipc_msg_t *msg;
    obj_handler_t hd = handler_alloc();

    if (hd == HANDLER_INVALID)
    {
        return -1;
    }
    thread_msg_buf_get(-1, (umword_t *)(&msg), NULL);
    msg->map_buf[0] = vpage_create_raw3(0, 0, hd).raw;
    ns.hd = hd;
    return 0;
}

void namespace_loop(void)
{
    rpc_loop();
}


int fs_svr_read(int fd, void *buf, size_t len)
{
    return -ENOSYS;
}
int fs_svr_write(int fd, void *buf, size_t len)
{
    return -ENOSYS;
}
int fs_svr_lseek(int fd, int offs, int whence)
{
    return -ENOSYS;
}
int fs_svr_ftruncate(int fd, off_t off)
{
    return -ENOSYS;
}
void fs_svr_sync(int fd)
{
}
int fs_svr_mkdir(char *path)
{
    return -ENOSYS;
}
int fs_svr_renmae(char *oldname, char *newname)
{
    return -ENOSYS;
}
int fs_svr_fstat(int fd, stat_t *stat)
{
    return -ENOSYS;
}
