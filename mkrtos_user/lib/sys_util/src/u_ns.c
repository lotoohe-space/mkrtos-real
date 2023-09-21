

#include "u_types.h"
#include "u_ipc.h"
#include "u_thread.h"
#include "u_prot.h"
#include "u_env.h"
#include "u_arch.h"
#include "u_util.h"
#include "u_hd_man.h"
#include <string.h>
#include <assert.h>
#include <errno.h>
enum ns_op
{
    OP_REGISTER,
    OP_QUERY,
};
/**
 * @brief 暂时手动填充数据，后期改为RPC。TODO:
 *
 * @param buf
 * @param len
 * @return int
 */
int cli_ns_register(const char *name, obj_handler_t hd)
{
    ipc_msg_t *ipc_msg;
    msg_tag_t tag;
    void *buf;

    int len = strlen(name);

    len = MIN(len, IPC_MSG_SIZE - WORD_BYTES * 2 - 1);
    // 获取发送数据的buf
    thread_msg_buf_get(THREAD_MAIN, (umword_t *)(&buf), NULL);
    ipc_msg = (ipc_msg_t *)buf;
    // 发送的正常数据
    ipc_msg->msg_buf[0] = OP_REGISTER;
    ipc_msg->msg_buf[1] = len + 1;
    // 映射数据
    ipc_msg->map_buf[0] = vpage_create_raw3(0, 0, hd).raw;

    strncpy((char *)(&ipc_msg->msg_buf[2]), name, IPC_MSG_SIZE - WORD_BYTES * 2);
    ((char *)(&ipc_msg->msg_buf[2]))[IPC_MSG_SIZE - WORD_BYTES * 2 - 1] = 0;

    // 发送ipc
    tag = ipc_call(u_get_global_env()->ns_hd,
                   msg_tag_init4(0, 2 + ROUND_UP(len, WORD_BYTES), 1, 0),
                   ipc_timeout_create2(0, 0));
    return msg_tag_get_val(tag);
}
int cli_ns_query(const char *name, obj_handler_t *ret_hd)
{
    ipc_msg_t *ipc_msg;
    msg_tag_t tag;
    void *buf;
    obj_handler_t new_fd;
    assert(ret_hd);

    new_fd = handler_alloc();

    if (new_fd == HANDLER_INVALID)
    {
        return -ENOMEM;
    }
    int len = strlen(name);

    len = MIN(len, IPC_MSG_SIZE - WORD_BYTES * 2 - 1);
    // 获取发送数据的buf
    thread_msg_buf_get(THREAD_MAIN, (umword_t *)(&buf), NULL);
    ipc_msg = (ipc_msg_t *)buf;
    // 发送的正常数据
    ipc_msg->msg_buf[0] = OP_QUERY;
    ipc_msg->msg_buf[1] = len + 1;
    // 映射数据
    ipc_msg->map_buf[0] = vpage_create_raw3(0, 0, new_fd).raw;

    strncpy((char *)(&ipc_msg->msg_buf[2]), name, IPC_MSG_SIZE - WORD_BYTES * 2);
    ((char *)(&ipc_msg->msg_buf[2]))[IPC_MSG_SIZE - WORD_BYTES * 2 - 1] = 0;

    // 发送ipc
    tag = ipc_call(u_get_global_env()->ns_hd,
                   msg_tag_init4(0, 2 + ROUND_UP(len, WORD_BYTES), 0, 0),
                   ipc_timeout_create2(0, 0));

    if (msg_tag_get_val(tag) < 0)
    {
        handler_free_umap(new_fd);
    }

    return msg_tag_get_val(tag);
}
