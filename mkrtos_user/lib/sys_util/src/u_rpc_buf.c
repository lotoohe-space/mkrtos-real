
#include "u_hd_man.h"
#include "u_ipc.h"
#include "u_prot.h"
#include "u_task.h"
#include "u_thread.h"
#include "u_util.h"
#include <assert.h>

// FIXME: 每个线程应该有一个buf_hd
#define RPC_SVR_MAP_OBJ_NR (MAP_BUF_SIZE / sizeof(umword_t))
static obj_handler_t buf_hd[RPC_SVR_MAP_OBJ_NR];

AUTO_CALL(101)
void rpc_hd_init(void)
{
    for (int i = 0; i < RPC_SVR_MAP_OBJ_NR; i++)
    {
        buf_hd[i] = HANDLER_INVALID;
    }
}
obj_handler_t rpc_hd_get(int inx)
{
    assert(inx < RPC_SVR_MAP_OBJ_NR && inx >= 0);
    return buf_hd[inx];
}
int rpc_hd_alloc_raw(ipc_msg_t *msg)
{
    msg_tag_t tag;
    obj_handler_t hd;
    bool_t alloc_new = TRUE;

    for (int i = 0; i < RPC_SVR_MAP_OBJ_NR; i++)
    {
        obj_handler_t tmp_hd = buf_hd[i];
        if (handler_is_used(tmp_hd))
        {
            if (task_obj_valid(TASK_THIS, tmp_hd, 0).prot == 0)
            {
                alloc_new = FALSE;
            }
        }
        if (alloc_new)
        {
            hd = handler_alloc();
            if (hd == HANDLER_INVALID)
            {
                return -1;
            }
            // 如果新申请的和后面的一样，则后面的需要清空
            for (int j = i + 1; j < RPC_SVR_MAP_OBJ_NR; j++)
            {
                if (hd == buf_hd[j])
                {
                    buf_hd[j] = 0;
                    break;
                }
            }
        }
        else
        {
            hd = tmp_hd;
        }
        msg->map_buf[i] = vpage_create_raw3(0, 0, hd).raw;
        buf_hd[i] = hd;
    }
    return 0;
}
int rpc_hd_alloc(void)
{
    ipc_msg_t *msg;
    msg_tag_t tag;
    tag = thread_msg_buf_get(-1, (void *)(&msg), NULL);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    return rpc_hd_alloc_raw(msg);
}
