
#include "u_ipc.h"
#include "u_factory.h"
#include "u_hd_man.h"
#include "u_thread.h"
#include "u_rpc_svr.h"
#include "rpc_prot.h"
#include "u_err.h"
#include <errno.h>
#include <assert.h>
#include <stdio.h>

static meta_t meta_obj;
static msg_tag_t rpc_meta_t_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg);

static rpc_svr_obj_t *meta_svr_find(meta_t *meta, umword_t prot)
{
    pthread_spin_lock(&meta->lock);
    for (int i = 0; i < META_PROT_NR; i++)
    {
        if (meta->svr_list[i])
        {
            if (meta->svr_list_prot[i] == prot)
            {
                pthread_spin_unlock(&meta->lock);
                return meta->svr_list[i];
            }
        }
    }
    pthread_spin_unlock(&meta->lock);
}
void meta_unreg_svr_obj_raw(meta_t *meta, umword_t prot)
{
    pthread_spin_lock(&meta->lock);
    for (int i = 0; i < META_PROT_NR; i++)
    {
        if (meta->svr_list[i] && meta->svr_list_prot[i] == prot)
        {
            meta->svr_list[i] = NULL;
            meta->svr_list_prot[i] = 0;
            break;
        }
    }
    pthread_spin_unlock(&meta->lock);
}
void meta_unreg_svr_obj(umword_t prot)
{
    meta_unreg_svr_obj_raw(&meta_obj, prot);
}
int meta_reg_svr_obj_raw(meta_t *meta, rpc_svr_obj_t *svr_obj, umword_t prot)
{
    int i = 0;
    pthread_spin_lock(&meta->lock);
    for (i = 0; i < META_PROT_NR; i++)
    {
        if (meta->svr_list[i] == NULL)
        {
            meta->svr_list_prot[i] = prot;
            meta->svr_list[i] = svr_obj;
            break;
        }
    }
    pthread_spin_unlock(&meta->lock);
    return i < META_PROT_NR;
}
int meta_reg_svr_obj(rpc_svr_obj_t *svr_obj, umword_t prot)
{
    return meta_reg_svr_obj_raw(&meta_obj, svr_obj, prot);
}
AUTO_CALL(101)
static void meta_obj_init(void)
{
    rpc_svr_obj_init(&meta_obj.svr, rpc_meta_t_dispatch, META_PROT);
}
static msg_tag_t rpc_meta_t_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg)
{
    meta_t *meta = (meta_t *)obj;

    if (msg_tag_get_prot(in_tag) == META_PROT)
    {
        printf("unknow prot.\n");
        return msg_tag_init4(0, 0, 0, -EPROTO);
    }
    else
    {
        msg_tag_t msg;
        rpc_svr_obj_t *svr_obj = meta_svr_find(meta, msg_tag_get_prot(in_tag));

        if (svr_obj == NULL)
        {
            printf("unknow prot.\n");

            return msg_tag_init4(0, 0, 0, -EPROTO);
        }
        if (svr_obj->dispatch == NULL)
        {
            printf("unknow dispatch.\n");
            return msg_tag_init4(0, 0, 0, -EPROTO);
        }
        msg = svr_obj->dispatch(svr_obj, in_tag, ipc_msg);
        return msg;
    }
}

/**
 * @brief 绑定IPC
 *
 * @param th
 * @param obj
 * @param ret_ipc_hd
 * @return int
 */
int rpc_creaite_bind_ipc(obj_handler_t th, void *obj, obj_handler_t *ret_ipc_hd)
{
    obj_handler_t ipc_hd;
    msg_tag_t tag;

    assert(ret_ipc_hd);

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
int rpc_meta_init(obj_handler_t th, obj_handler_t *ret_ipc_hd)
{
    return rpc_creaite_bind_ipc(th, &meta_obj.svr, ret_ipc_hd);
}
/**
 * @brief RPC循环处理消息
 *
 * @param ipc_hd
 * @param svr_obj
 * @param dispatch
 */
void rpc_loop(void)
{
    umword_t obj;
    msg_tag_t tag;
    umword_t buf;
    ipc_msg_t *msg;
    rpc_svr_obj_t *svr_obj;

    thread_msg_buf_get(-1, &buf, NULL);
    msg = (ipc_msg_t *)buf;
    while (1)
    {
        tag = thread_ipc_wait(ipc_timeout_create2(0, 0), &obj);
        if (msg_tag_get_val(tag) < 0)
        {
            continue;
        }
        svr_obj = (rpc_svr_obj_t *)obj;
        if (svr_obj->dispatch)
        {
            tag = svr_obj->dispatch(svr_obj, tag, msg);
        }
        thread_ipc_reply(tag, ipc_timeout_create2(0, 0));
    }
}
