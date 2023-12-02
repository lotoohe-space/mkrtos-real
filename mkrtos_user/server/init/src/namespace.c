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
#include "namespace.h"
#include "u_rpc_svr.h"
static ns_t ns;
void namespace_init(void)
{
    ns_init(&ns);
    meta_reg_svr_obj(&ns.svr, NS_PROT);
    printf("ns svr init...\n");
}

static int namespace_alloc(const char *path, obj_handler_t hd)
{
    for (int i = 0; i < NAMESAPCE_NR; i++)
    {
        if (ns.ne_list[i].hd == HANDLER_INVALID)
        {
            ns.ne_list[i].hd = hd;
            strncpy(ns.ne_list[i].path, path, NAMESPACE_PATH_LEN);
            ns.ne_list[i].path[NAMESPACE_PATH_LEN - 1] = 0;
            return i;
        }
    }
    return -1;
}

enum ns_op
{
    OP_REGISTER,
    OP_QUERY,
};

/**
 * @brief 注册一个obj
 *
 * @param path 注册的路径
 * @param hd 注册的hd
 * @return int
 */
int namespace_register(const char *path, obj_handler_t hd)
{
    if (namespace_alloc(path, hd) < 0)
    {
        return -1;
    }
    printf("register svr, name is %s, hd is %d\n", path, hd);
    namespace_pre_alloc_map_fd();
    return 0;
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
    for (int i = 0; i < NAMESAPCE_NR; i++)
    {
        if (ns.ne_list[i].hd != HANDLER_INVALID)
        {
            char *split_str = strstr(path, ns.ne_list[i].path);
            if (split_str && (split_str == path))
            {
                msg_tag_t tag = task_obj_valid(TASK_THIS, ns.ne_list[i].hd);
                if (msg_tag_get_val(tag) != 1)
                {
                    // 对象变为无效，删除该条记录
                    ns.ne_list[i].hd = HANDLER_INVALID;
                }
                *hd = ns.ne_list[i].hd;
                return (int)(strlen(ns.ne_list[i].path));
            }
        }
    }
    return -1;
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
