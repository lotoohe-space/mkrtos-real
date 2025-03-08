/**
 * @file u_rpc_svr.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-08-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "u_ipc.h"
#include "u_factory.h"
#include "u_hd_man.h"
#include "u_thread.h"
#include "u_rpc_svr.h"
#include "rpc_prot.h"
#include "u_err.h"
#include "u_rpc_buf.h"
#include "cons_cli.h"
#include "u_env.h"
#include "u_sleep.h"
#include "u_thread_util.h"
#include "u_slist.h"
#include "cons_cli.h"
#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

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
    return NULL;
}
rpc_svr_obj_t *meta_find_svr_obj(umword_t prot)
{
    return meta_svr_find(&meta_obj, prot);
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
void meta_obj_init_def(void)
{
    meta_obj_init(&meta_obj);
}
void meta_obj_init(meta_t *meta)
{
    if (!meta->svr.dispatch)
    {
        // 防止重复初始化
        rpc_svr_obj_init(&meta->svr, rpc_meta_t_dispatch, META_PROT);
    }
}
static msg_tag_t rpc_meta_t_dispatch(struct rpc_svr_obj *obj, msg_tag_t in_tag, ipc_msg_t *ipc_msg)
{
    meta_t *meta = (meta_t *)obj;

    if (msg_tag_get_prot(in_tag) == META_PROT)
    {
#if 0
        cons_write_str("unknow prot.\n"); //FIXME:
#endif
        return msg_tag_init4(0, 0, 0, -EPROTO);
    }
    else
    {
        msg_tag_t msg;
        rpc_svr_obj_t *svr_obj = meta_svr_find(meta, msg_tag_get_prot(in_tag));

        if (svr_obj == NULL)
        {
#if 0
        cons_write_str("unknow prot.\n"); //FIXME:
#endif
            return msg_tag_init4(0, 0, 0, -EPROTO);
        }
        if (svr_obj->dispatch == NULL)
        {
            cons_write_str("unknow dispatch.\n");
            return msg_tag_init4(0, 0, 0, -EPROTO);
        }
        msg = svr_obj->dispatch(svr_obj, in_tag, ipc_msg);
        return msg;
    }
}
int rpc_meta_init_def(obj_handler_t tk_hd, obj_handler_t *ret_ipc_hd)
{
    return rpc_meta_init(&meta_obj, tk_hd, ret_ipc_hd);
}
int rpc_meta_init(meta_t *meta_obj, obj_handler_t tk_hd, obj_handler_t *ret_ipc_hd)
{
    int ret;

    if (meta_obj->is_init)
    {
        *ret_ipc_hd = meta_obj->hd_meta;
        return 0;
    }
    ret = rpc_creaite_bind_ipc(tk_hd, &meta_obj->svr, ret_ipc_hd);
    if (ret < 0)
    {
        return ret;
    }
    meta_obj->hd_meta = *ret_ipc_hd;
    meta_obj->is_init = TRUE;
    return 0;
}
/**
 * @brief 绑定IPC
 *
 * @param th
 * @param obj
 * @param ret_ipc_hd
 * @return int
 */
int rpc_creaite_bind_ipc(obj_handler_t tk, void *obj, obj_handler_t *ret_ipc_hd)
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
    tag = ipc_bind(ipc_hd, tk, (umword_t)obj);
    if (msg_tag_get_val(tag) < 0)
    {
        handler_free_umap(ipc_hd);
        return msg_tag_get_val(tag);
    }
    *ret_ipc_hd = ipc_hd;
    return 0;
}
