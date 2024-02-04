
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
        cons_write_str("unknow prot.\n");
        return msg_tag_init4(0, 0, 0, -EPROTO);
    }
    else
    {
        msg_tag_t msg;
        rpc_svr_obj_t *svr_obj = meta_svr_find(meta, msg_tag_get_prot(in_tag));

        if (svr_obj == NULL)
        {
            cons_write_str("unknow prot.\n");

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
    umword_t obj = 0;
    msg_tag_t tag;
    umword_t buf;
    ipc_msg_t *msg;
    rpc_svr_obj_t *svr_obj;

    thread_msg_buf_get(-1, &buf, NULL);
    msg = (ipc_msg_t *)buf;
    while (1)
    {
        rpc_hd_alloc();
        tag = thread_ipc_wait(ipc_timeout_create2(0, 0), &obj, -1);
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
#define RPC_MTD_TH_STACK_SIZE (1024+256)
typedef struct mtd_params
{
    rpc_svr_obj_t *obj;
    void *stack;
    int is_del;
    msg_tag_t in_tag;
    obj_handler_t ipc_obj;
    obj_handler_t th_obj;
    slist_head_t node;
} mtd_params_t;
static slist_head_t th_head;
static pthread_spinlock_t lock;
static void rpc_mtc_thread(void *arg)
{
    rpc_svr_obj_t *svr_obj;
    msg_tag_t tag;
    ipc_msg_t *msg;
    mtd_params_t *params = (mtd_params_t *)arg;

    thread_msg_buf_get(-1, (umword_t *)(&msg), NULL);
    svr_obj = (rpc_svr_obj_t *)params->obj;
    if (svr_obj->dispatch)
    {
        tag = svr_obj->dispatch(svr_obj, params->in_tag, msg);
    }
    thread_ipc_send(tag, params->ipc_obj, ipc_timeout_create2(0, 0));
    params->is_del = 1;
    while (1)
    {
        u_sleep_ms(1000);
    }
}
static void check_release_stack_mem(void)
{
    mtd_params_t *pos;

    pthread_spin_lock(&lock);
    slist_foreach_not_next(pos, &th_head, node)
    {
        mtd_params_t *next = slist_next_entry(pos, &th_head, node);

        if (pos->is_del == 1)
        {
            slist_del(&pos->node);
            // void *stack = (void *)((char *)pos - (RPC_MTD_TH_STACK_SIZE + MSG_BUG_LEN));
            handler_del_umap(pos->ipc_obj);
            u_thread_del(pos->th_obj);
            free(pos->stack);
        }
        pos = next;
    }
    pthread_spin_unlock(&lock);
}
extern void __pthread_new_thread_entry__(void);
int rpc_mtd_loop(void)
{
    umword_t obj = 0;
    msg_tag_t tag;
    umword_t buf;
    obj_handler_t ipc_hd;
    uint8_t *main_msg_buf;

    thread_msg_buf_get(-1, (umword_t *)(&main_msg_buf), NULL);
    slist_init(&th_head);
    while (1)
    {
        rpc_hd_alloc();
        check_release_stack_mem();
        ipc_hd = handler_alloc();
        if (ipc_hd == HANDLER_INVALID)
        {
            // cons_write_str("mtd alloc is fial.\n");
            // u_sleep_ms(1000);
            continue;
        }
        tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(0, 0, ipc_hd));
        if (msg_tag_get_val(tag) < 0)
        {
            // cons_write_str("mtd factory ipc fail.\n");
            handler_free(ipc_hd);
            // u_sleep_ms(1000);
            continue;
        }

        tag = thread_ipc_wait(ipc_timeout_create2(1000, 1000), &obj, ipc_hd);
        if (msg_tag_get_val(tag) < 0)
        {
            handler_free_umap(ipc_hd);
            continue;
        }
    again_create:;
        obj_handler_t th_obj;
        void *stack;

        stack = memalign(sizeof(void *) * 2,
                         RPC_MTD_TH_STACK_SIZE + MSG_BUG_LEN + sizeof(mtd_params_t));
        if (!stack)
        {
            // cons_write_str("mtd no stack mem.\n");
            check_release_stack_mem();
            // u_sleep_ms(1000);
            goto again_create;
        }
        uint8_t *msg_buf = (uint8_t *)stack + RPC_MTD_TH_STACK_SIZE;

        
        int ret_val;
        umword_t *stack_tmp = (umword_t *)((uint8_t *)stack + RPC_MTD_TH_STACK_SIZE);
        mtd_params_t *params = (mtd_params_t *)((char *)stack + RPC_MTD_TH_STACK_SIZE + MSG_BUG_LEN);

        // 设置调用参数等
        *(--stack_tmp) = (umword_t)(params);
        *(--stack_tmp) = (umword_t)0; // 保留
        *(--stack_tmp) = (umword_t)rpc_mtc_thread;

        //
        params->in_tag = tag;
        params->ipc_obj = ipc_hd;
        params->obj = (rpc_svr_obj_t *)obj;
        params->stack = stack;
        params->is_del = 0;

        slist_init(&params->node);
        pthread_spin_lock(&lock);
        slist_add(&th_head, &params->node);
        pthread_spin_unlock(&lock);
    again_th_create:
        ret_val = u_thread_create(&params->th_obj,
                                  (char *)stack_tmp,
                                  (char *)stack + RPC_MTD_TH_STACK_SIZE,
                                  (void (*)(void))__pthread_new_thread_entry__);

        if (ret_val < 0)
        {
            // cons_write_str("mtd no mem.\n");
            check_release_stack_mem();
            // u_sleep_ms(1000);
            goto again_th_create;
        }
        memcpy(msg_buf, main_msg_buf, MSG_BUG_LEN - IPC_USER_SIZE);
        ipc_msg_t *msg = (ipc_msg_t *)msg_buf;
        
        msg->user[2] = thread_get_src_pid();
        u_thread_run(params->th_obj, 2);

        // thread_ipc_reply(tag, ipc_timeout_create2(0, 0));
    }
    return 0;
}
