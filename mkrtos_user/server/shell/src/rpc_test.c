
#include "u_rpc.h"
#include <string.h>
#include <stdio.h>
typedef struct test_svr
{

} test_svr_t;

static test_svr_t test;

RPC_GENERATION_OP1(test_svr_t, 0, register, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, var0)
{
    printf("var0 val is %d\n", var0->data);
    return 1;
}
// RPC_GENERATION_DISPATCH1(test_svr_t, 0, register, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, var0)
msg_tag_t test_svr_t_register_dispatch(test_svr_t *obj, msg_tag_t tag, ipc_msg_t *ipc_msg)
{
    rpc_int_t var0;
    size_t op_val;
    uint8_t *value = (uint8_t *)(ipc_msg->msg_buf);
    int off = 0;
    rpc_var_rpc_int_t_init(&var0);
    op_val = *((typeof(0) *)value);
    off += sizeof(typeof(0));
    off = rpc_align(off, __alignof(typeof(0)));
    do
    {
        if (1 == 1)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_svr_buf_to_msg_rpc_int_t(&var0, (uint8_t *)(value), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    short ret_val = test_svr_t_register_op(obj, &var0);
    if (ret_val < 0)
    {
        return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret_val)});
    }
    off = 0;
    do
    {
        if (1 == 1)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_svr_msg_to_buf_rpc_int_t(&var0, (uint8_t *)(value), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    return ((msg_tag_t){.flags = (0), .msg_buf_len = ((((off) / ((sizeof(void *)))) + (((off) % ((sizeof(void *)))) ? 1 : 0))), .map_buf_len = (0), .prot = (ret_val)});
}
// RPC_GENERATION_CALL1(test_svr_t, 0, register, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, var0)
msg_tag_t test_svr_t_register_call(obj_handler_t hd, rpc_int_t *var0)
{
    void *buf;
    ipc_msg_t *msg_ipc;
    thread_msg_buf_get(2, (umword_t *)(&buf), ((void *)0));
    msg_ipc = (ipc_msg_t *)buf;
    int off = 0;
    int ret = -1;
    int op_val = 0;
    rpc_memcpy(msg_ipc->msg_buf, &op_val, sizeof(op_val));
    off += rpc_align(sizeof(op_val), __alignof(0));
    do
    {
        if (1 == 1)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_int_t(var0, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off);
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
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_int_t(var0, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    // msg_tag_t tag = ipc_call(hd, ((msg_tag_t){.flags = (0), .msg_buf_len = ((((off) / ((sizeof(void *)))) + (((off) % ((sizeof(void *)))) ? 1 : 0))), .map_buf_len = (0), .prot = (0)}), ipc_timeout_create2(0, 0));
    msg_tag_t tag = test_svr_t_register_dispatch(&test, ((msg_tag_t){.flags = (0), .msg_buf_len = ((((off) / ((sizeof(void *)))) + (((off) % ((sizeof(void *)))) ? 1 : 0))), .map_buf_len = (0), .prot = (0)}), msg_ipc);

    if (((int16_t)((tag).prot)) < 0)
    {
        return tag;
    }
    off = 0;
    do
    {
        if (1 == 1)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_cli_buf_to_msg_rpc_int_t(var0, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    return tag;
}
// msg_tag_t tag = test_svr_t_register_dispatch(&test, ((msg_tag_t){.flags = (0), .msg_buf_len = ((((off) / ((sizeof(void *)))) + (((off) % ((sizeof(void *)))) ? 1 : 0))), .map_buf_len = (0), .prot = (0)}), msg_ipc);

// RPC_GENERATION_CALL2(test_svr_t, 1, query,
//                      rpc_ref_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, var0, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, var1)

void rpc_test(void)
{
    rpc_int_t var0 = {.data = 1};
    msg_tag_t tag = test_svr_t_register_call(15, &var0);

    rpc_ref_array_uint32_t_uint8_t_32_t str_tst = {
        .data = "test",
        .len = strlen("test") + 1};

    // test_svr_t_query_call(15, &str_tst, &var0);
}