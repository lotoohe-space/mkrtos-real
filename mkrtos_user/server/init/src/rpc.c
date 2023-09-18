
#include "u_ipc.h"
#include "u_factory.h"
#include "u_hd_man.h"
#include "u_thread.h"
#include <errno.h>
#include <assert.h>
#include <stdio.h>

int rpc_creaite_bind_ipc(obj_handler_t th, void *obj, obj_handler_t *ret_ipc_hd)
{
    obj_handler_t ipc_hd;
    msg_tag_t tag;

    assert(ipc_hd);

    ipc_hd = handler_alloc();
    if (ipc_hd == HANDLER_INVALID)
    {
        return -ENOENT;
    }
    tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(0, 0, ipc_hd));
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    tag = ipc_bind(ipc_hd, th, (umword_t)obj);
    if (msg_tag_get_val(tag) < 0)
    {
        handler_free_umap(ipc_hd);
        return msg_tag_get_val(tag);
    }
    *ret_ipc_hd = ipc_hd;
    return 0;
}
void rpc_loop(obj_handler_t ipc_hd, msg_tag_t (*rpc_dispatch)(ipc_msg_t *msg))
{
    umword_t obj;
    msg_tag_t tag;
    umword_t buf;
    ipc_msg_t *msg;

    thread_msg_buf_get(-1, &buf, NULL);
    msg = (ipc_msg_t *)buf;
    while (1)
    {
        tag = ipc_wait(ipc_hd, &obj);
        if (msg_tag_get_val(tag) < 0)
        {
            continue;
        }
        if (rpc_dispatch) {
            tag = rpc_dispatch(msg);
        }
        ipc_reply(ipc_hd, tag);
    }
}
