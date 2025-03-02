#pragma once

#include "u_types.h"
#include "u_prot.h"
#include "u_thread.h"
#include <pthread.h>

struct rpc_svr_obj;
typedef msg_tag_t (*rpc_dispatch_func)(struct rpc_svr_obj *obj, msg_tag_t tag, ipc_msg_t *msg);

typedef struct rpc_svr_obj
{
    mword_t prot;               //!< 支持的协议
    rpc_dispatch_func dispatch; //!< 分发函数
} rpc_svr_obj_t;

static inline int rpc_svr_obj_init(rpc_svr_obj_t *obj, rpc_dispatch_func dis, mword_t prot)
{
    obj->dispatch = dis;
    obj->prot = prot;
}

#define META_PROT_NR 4
typedef struct meta
{
    rpc_svr_obj_t svr;
    rpc_svr_obj_t *svr_list[META_PROT_NR];
    umword_t svr_list_prot[META_PROT_NR];
    pthread_spinlock_t lock;

    obj_handler_t hd_meta;
    bool_t is_init;
} meta_t;

int rpc_meta_init(obj_handler_t tk, obj_handler_t *ret_ipc_hd);
void meta_obj_init(void);
void meta_unreg_svr_obj_raw(meta_t *meta, umword_t prot);
void meta_unreg_svr_obj(umword_t prot);
rpc_svr_obj_t *meta_find_svr_obj(umword_t prot);
int meta_reg_svr_obj(rpc_svr_obj_t *svr_obj, umword_t prot);
int meta_reg_svr_obj_raw(meta_t *meta, rpc_svr_obj_t *svr_obj, umword_t prot);
int rpc_creaite_bind_ipc(obj_handler_t tk, void *obj, obj_handler_t *ipc_hd);
#if 0
void rpc_loop(void);
int rpc_mtd_loop(void);
#endif
