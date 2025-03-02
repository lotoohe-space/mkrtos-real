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
#include "namespace.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <u_types.h>

#include "fs_svr.h"
#include "ns_svr.h"
#include "ns_types.h"
#include "u_hd_man.h"
#include "u_ipc.h"
#include "u_rpc_svr.h"
#include "u_slist.h"
#include "nsfs.h"
static ns_t ns;
static fs_t ns_fs;

static obj_handler_t ns_hd;

static int fs_svr_open(const char *path, int flags, int mode)
{
    return fs_ns_open(path, flags, mode);
}

static int fs_svr_readdir(int fd, dirent_t *dir)
{
    return fs_ns_readdir(fd, dir);
}
static void fs_svr_close(int fd)
{
    fs_ns_close(fd);
}
static int fs_svr_unlink(const char *path)
{
    return -ENOSYS;
}
static int fs_svr_mkdir(char *path)
{
    return fs_ns_mkdir(path);
}
static int fs_svr_stat(const char *path, void *_buf)
{
    struct kstat *buf = (struct kstat *)_buf;
    int ret;

    ret = fs_ns_stat(path, buf);
    return ret;
}
/**
 * @brief 创建一个软链接节点
 *
 * @param src
 * @param dst
 * @return int
 */
static int fs_svr_symlink(const char *src, const char *dst)
{
    return -ENOSYS;
}

/**
 * @brief 注册一个obj
 * 如果节点，但是服务是无效的，则删除它
 *
 * @param path 注册的路径
 * @param hd 注册的hd
 * @return int
 */
int namespace_register(const char *path, obj_handler_t hd, int type)
{
    int ret;

again:
    ret = ns_mknode(path, hd, NODE_TYPE_SVR);
    if (ret == -EEXIST)
    {
        //  如果已经存在，则检查是否有效，否则删除它
        obj_handler_t old_hd;
        msg_tag_t tag;
        int obj_type;

        ret = ns_find_svr_obj(path, &old_hd); // 这个函数里面会自动删除
        if (ret < 0)
        {
            return ret;
        }
    }
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
    int ret;

    ret = ns_find_svr_obj(path, hd);
    return ret;
}
/**
 * 文件操作
 */
static const fs_operations_t ops =
    {
        .fs_svr_mkdir = fs_svr_mkdir,
        .fs_svr_symlink = fs_svr_symlink,
        .fs_svr_unlink = fs_svr_unlink,
        .fs_svr_close = fs_svr_close,
        .fs_svr_readdir = fs_svr_readdir,
        .fs_svr_open = fs_svr_open,
        .fs_svr_stat = fs_svr_stat,
};
/**
 * 初始化namespace
 */
void namespace_init(obj_handler_t ipc_hd)
{
    ns_init(&ns);
    fs_init(&ns_fs, &ops);
    meta_reg_svr_obj(&ns.svr, NS_PROT);
    meta_reg_svr_obj(&ns_fs.svr, FS_PROT);
    thread_set_src_pid(0);
    ns_hd = ipc_hd;
    ns_root_node_init(ipc_hd);
    // printf("ns svr init...\n");
}
