
#include "u_hd_man.h"
#include "u_ipc.h"
#include "u_prot.h"
#include "u_task.h"
#include "u_thread.h"
static obj_handler_t buf_hd = HANDLER_INVALID;
obj_handler_t rpc_hd_get(int inx)
{
    /*TODO:*/
    return buf_hd;
}
int rpc_hd_alloc(void)
{
    ipc_msg_t *msg;
    msg_tag_t tag;
    obj_handler_t hd;
    bool_t alloc_new = TRUE;

    if (handler_is_used(buf_hd))
    {
        if (task_obj_valid(TASK_THIS, buf_hd).prot == 0)
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
    }
    else
    {
        hd = buf_hd;
    }
    thread_msg_buf_get(-1, (umword_t *)(&msg), NULL);
    msg->map_buf[0] = vpage_create_raw3(0, 0, hd).raw;
    buf_hd = hd;
    return 0;
}
