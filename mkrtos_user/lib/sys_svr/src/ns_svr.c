#include "rpc_prot.h"
#include "ns_svr.h"
#include "ns_types.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_hd_man.h"
#include <stdio.h>

RPC_GENERATION_OP2(ns_t, NS_REGISTER_OP, register,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, svr_hd)
{
    path->data[path->len - 1] = 0;

    int ret = namespace_register((char *)(path->data), obj->hd);
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

RPC_GENERATION_DISPATCH2(ns_t, NS_REGISTER_OP, register,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, svr_hd)

RPC_GENERATION_OP2(ns_t, NS_QUERY_OP, query,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_INOUT, RPC_TYPE_BUF, cli_hd)
{
    path->data[path->len - 1] = 0;

    int ret = namespace_query((char *)(path->data), &cli_hd->data);
    if (ret >= 0)
    {
        printf("The request service [%s] was successful.", (char *)(path->data));
    }
    else
    {
        printf("Failed to request service [%s]", (char *)(path->data));
    }
    return ret;
}

// RPC_GENERATION_DISPATCH2(ns_t, NS_QUERY_OP, query,
//                          rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
//                          rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_INOUT, RPC_TYPE_BUF, cli_hd)
msg_tag_t ns_t_query_dispatch(ns_t *obj, msg_tag_t tag, ipc_msg_t *ipc_msg)
{
    rpc_array_uint32_t_uint8_t_32_t var0;
    rpc_obj_handler_t_t var1;
    size_t op_val;
    uint8_t *value = (uint8_t *)(ipc_msg->msg_buf);
    int off = 0;
    rpc_var_rpc_array_uint32_t_uint8_t_32_t_init(&var0);
    rpc_var_rpc_obj_handler_t_t_init(&var1);
    op_val = *((typeof(((uint16_t)1)) *)value);
    if (op_val != ((uint16_t)1))
    {
        return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (-100)});
    }
    off += sizeof(typeof(((uint16_t)1)));
    off = rpc_align(off, __alignof(typeof(((uint16_t)1))));
    do
    {
        if (1 == 1)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_svr_buf_to_msg_rpc_array_uint32_t_uint8_t_32_t(&var0, (uint8_t *)(value), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (2 == 1)
        {
            if (4 == 1 || 4 == 4)
            {
                int ret = rpc_svr_buf_to_msg_rpc_obj_handler_t_t(&var1, (uint8_t *)(value), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    short ret_val = ns_t_query_op(obj, &var0, &var1);
    if (ret_val < 0)
    {
        return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret_val)});
    }
    off = 0;
    int off_map = 0;
    do
    {
        if (1 == 1)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_svr_msg_to_buf_rpc_array_uint32_t_uint8_t_32_t(&var0, (uint8_t *)(value), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (2 == 1)
        {
            if (4 == 2 || 4 == 4)
            {
                int ret = rpc_svr_msg_to_buf_rpc_obj_handler_t_t(&var1, (uint8_t *)(value), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 2)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_svr_msg_to_buf_rpc_array_uint32_t_uint8_t_32_t(&var0, (uint8_t *)(ipc_msg->map_buf), off_map);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_map = ret;
            }
        }
    } while (0);
    do
    {
        if (2 == 2)
        {
            if (4 == 2 || 4 == 4)
            {
                int ret = rpc_svr_msg_to_buf_rpc_obj_handler_t_t(&var1, (uint8_t *)(ipc_msg->map_buf), off_map);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_map = ret;
            }
        }
    } while (0);
    return ((msg_tag_t){.flags = (0), .msg_buf_len = ((((off) / ((sizeof(void *)))) + (((off) % ((sizeof(void *)))) ? 1 : 0))), .map_buf_len = ((((off_map) / ((sizeof(void *)))) + (((off_map) % ((sizeof(void *)))) ? 1 : 0))), .prot = (ret_val)});
}
RPC_DISPATCH2(ns_t, typeof(NS_REGISTER_OP), NS_REGISTER_OP, register, NS_QUERY_OP, query)

void ns_init(ns_t *ns)
{
    rpc_svr_obj_init(&ns->svr, rpc_ns_t_dispatch, NS_PROT);
    for (int i = 0; i < NAMESAPCE_NR; i++)
    {
        ns->ne_list[i].hd = HANDLER_INVALID;
    }
    ns->hd = HANDLER_INVALID;
}
