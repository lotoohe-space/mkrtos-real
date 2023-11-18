#pragma once

#include "u_types.h"
#include "u_prot.h"
#include "u_ipc.h"

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

int rpc_creaite_bind_ipc(obj_handler_t th, void *obj, obj_handler_t *ipc_hd);
void rpc_loop(obj_handler_t ipc_hd, rpc_svr_obj_t *svr_obj);
