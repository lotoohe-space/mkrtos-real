
#include <u_types.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include "u_hd_man.h"
#include "u_ipc.h"
#define NAMESPACE_PATH_LEN 32
#define NAMESAPCE_NR 32

typedef struct namespace_entry
{
    char path[NAMESPACE_PATH_LEN];
    obj_handler_t hd;
} namespace_entry_t;

typedef struct namespace
{
    namespace_entry_t ne_list[NAMESAPCE_NR];
}
namespace_t;

static namespace_t ns;

static int ns_alloc(const char *path, obj_handler_t hd)
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
int ns_register(const char *path, obj_handler_t hd)
{
    if (ns_alloc(path, hd) < 0)
    {
        return -1;
    }
    printf("register svr, name is %d, hd is %d\n", path, hd);
    return 0;
}
/**
 * @brief 申请一个obj
 *
 * @param path
 * @param hd
 * @return int
 */
int ns_query(const char *path, obj_handler_t *hd)
{
    for (int i = 0; i < NAMESAPCE_NR; i++)
    {
        if (ns.ne_list[i].hd == HANDLER_INVALID)
        {
            if (strncmp(ns.ne_list[i].path, path, NAMESPACE_PATH_LEN) == 0)
            {
                *hd = ns.ne_list[i].hd;
                return 0;
            }
        }
    }
    return -1;
}
obj_handler_t pre_alloc_hd = HANDLER_INVALID;
int ns_pre_alloc_map_fd(ipc_msg_t *msg)
{
    obj_handler_t hd = handler_alloc();

    if (hd == HANDLER_INVALID)
    {
        return -1;
    }
    msg->map_buf[0] = vpage_create_raw3(0, 0, hd).raw;
    pre_alloc_hd = hd;
    return 0;
}
msg_tag_t ns_dispatch(ipc_msg_t *msg)
{
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -ENOSYS);
    assert(msg);

    switch (msg->msg_buf[0])
    {
    case OP_REGISTER:
    {
        size_t len = msg->msg_buf[1];
        if (len > NAMESPACE_PATH_LEN)
        {
            tag = msg_tag_init4(0, 0, 0, -EINVAL);
            break;
        }
        ((char *)&(msg->msg_buf[2]))[len] = 0;
        ns_register((char*)msg->msg_buf[2], pre_alloc_hd);
        ns_pre_alloc_map_fd(msg);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case OP_QUERY:
        tag = msg_tag_init4(0, 0, 0, 0);
        break;
    default:
        tag = msg_tag_init4(0, 0, 0, -ENOSYS);
        break;
    }
    return tag;
}
