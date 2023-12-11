#include "rpc_prot.h"
#include "ns_svr.h"
#include "ns_types.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_arch.h"
#include "u_rpc_buf.h"
#include "u_hd_man.h"
#include <stdio.h>

RPC_GENERATION_OP3(ns_t, NS_PROT, NS_REGISTER_OP, register,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, svr_hd,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, type)
{
    path->data[path->len - 1] = 0;

    int ret = namespace_register((char *)(path->data), rpc_hd_get(0), type->data);
    if (ret >= 0)
    {
        printf("register [%s] success.\n", (char *)(path->data));
    }
    else
    {
        printf("register [%s] fail.\n", (char *)(path->data));
    }
    return ret;
}
RPC_GENERATION_DISPATCH3(ns_t, NS_PROT, NS_REGISTER_OP, register,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, svr_hd,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, type)

RPC_GENERATION_OP2(ns_t, NS_PROT, NS_QUERY_OP, query,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_INOUT, RPC_TYPE_BUF, cli_hd)
{
    path->data[path->len - 1] = 0;

    int ret = namespace_query((char *)(path->data), &cli_hd->data);
    if (ret >= 0)
    {
        printf("The request service [%s] was successful, hd is %d.\n", (char *)(path->data), cli_hd->data);
    }
    else
    {
        printf("Failed to request service [%s]\n", (char *)(path->data));
    }
    return ret;
}

RPC_GENERATION_DISPATCH2(ns_t, NS_PROT, NS_QUERY_OP, query,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_INOUT, RPC_TYPE_BUF, cli_hd)
RPC_DISPATCH2(ns_t, NS_PROT, typeof(NS_REGISTER_OP), NS_REGISTER_OP, register, NS_QUERY_OP, query)

void ns_init(ns_t *ns)
{
    rpc_svr_obj_init(&ns->svr, rpc_ns_t_dispatch, NS_PROT);
}
