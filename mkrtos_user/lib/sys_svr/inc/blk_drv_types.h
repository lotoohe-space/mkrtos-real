#pragma once
#include "u_types.h"
#include "u_rpc_svr.h"
#include "u_slist.h"
#include "u_rpc.h"
typedef struct blk_drv_info
{
    umword_t blk_start_addr; /*内存中的起始地址*/
    size_t blk_size;         /*块大小*/
    size_t blk_nr;           /*blk块数量*/
} blk_drv_info_t;

RPC_TYPE_DEF_ALL(blk_drv_info_t)

typedef struct blk_drv
{
    rpc_svr_obj_t svr;
} blk_drv_t;
