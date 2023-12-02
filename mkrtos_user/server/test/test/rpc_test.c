
#include "u_rpc.h"
#include <string.h>
#include <stdio.h>
typedef struct test_svr
{

} test_svr_t;

static test_svr_t test;

#if 0
RPC_GENERATION_OP1(test_svr_t, 0, register, rpc_int_t, rpc_int_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var0)
{
    printf("var0 val is %d\n", var0->data);
    var0->data = 0x112233;
    return 1;
}
RPC_GENERATION_DISPATCH1(test_svr_t, 0, register, rpc_int_t, rpc_int_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var0)
RPC_GENERATION_CALL1(test_svr_t, 0, register, rpc_int_t, rpc_int_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var0)
#endif
#if 0
RPC_GENERATION_OP2(test_svr_t, 0, query,
                   rpc_int_t, rpc_int_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var0,
                   rpc_int_t, rpc_int_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var1)
{
    printf("var0 val is %d\n", var0->data);
    printf("var1 val is %d\n", var1->data);
    var0->data = 0x112233;
    var1->data = 0x123456;
    return 2;
}

RPC_GENERATION_DISPATCH2(test_svr_t, 0, query,
                         rpc_int_t, rpc_int_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var0,
                         rpc_int_t, rpc_int_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var1)
RPC_GENERATION_CALL2(test_svr_t, 0, query,
                     rpc_int_t, rpc_int_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var0,
                     rpc_int_t, rpc_int_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var1)

msg_tag_t dispatch_test(msg_tag_t tag, ipc_msg_t *msg)
{
    tag = test_svr_t_query_dispatch(&test, tag, msg);
}
#endif

#if 0
RPC_GENERATION_OP2(test_svr_t, 0, query,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var0,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var1)
{
    printf("var0 val is %s\n", var0->data);
    printf("var1 val is %s\n", var1->data);
    strcpy(var0->data, "....");
    var0->len = 5;
    strcpy(var1->data, "____");
    var1->len = 5;
    return 2;
}

RPC_GENERATION_DISPATCH2(test_svr_t, 0, query,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var0,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var1)

RPC_GENERATION_CALL2(test_svr_t, 0, query,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var0,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var1)

msg_tag_t dispatch_test(msg_tag_t tag, ipc_msg_t *msg)
{
    tag = test_svr_t_query_dispatch(&test, tag, msg);
}
#endif
#if 0
RPC_GENERATION_OP2(test_svr_t, 0, query,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var0,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, var1)
{
    printf("var0 val is %s\n", var0->data);
    strcpy(var0->data, "....");
    var0->len = 5;
    return 2;
}

RPC_GENERATION_DISPATCH2(test_svr_t, 0, query,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var0,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, var1)

RPC_GENERATION_CALL2(test_svr_t, 0, query,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_INOUT, RPC_TYPE_DATA, var0,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, var1)

msg_tag_t dispatch_test(msg_tag_t tag, ipc_msg_t *msg)
{
    tag = test_svr_t_query_dispatch(&test, tag, msg);
}
#endif
RPC_GENERATION_OP2(test_svr_t, 0, 0, query,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_OUT, RPC_TYPE_BUF, var0,
                   rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, var1)
{
    printf("dispath ok.\n");
    var0->data = vpage_create_raw3(0, 0, 30).raw;
    return 2;
}

RPC_GENERATION_DISPATCH2(test_svr_t, 0, 0, query,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_OUT, RPC_TYPE_BUF, var0,
                         rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, var1)
RPC_GENERATION_CALL2(test_svr_t, 0, 0, query,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_OUT, RPC_TYPE_BUF, var0,
                     rpc_obj_handler_t_t, rpc_obj_handler_t_t, RPC_DIR_IN, RPC_TYPE_BUF, var1)

msg_tag_t dispatch_test(msg_tag_t tag, ipc_msg_t *msg)
{
    tag = test_svr_t_query_dispatch(&test, tag, msg);
}
void rpc_test(void)
{
    msg_tag_t tag;
#if 0
    rpc_int_t var0 = {.data = 456};
    tag = test_svr_t_register_call(15, &var0);
    printf("ret:%d, val is 0x%x\n", msg_tag_get_val(tag), var0.data);
#endif
#if 0
    rpc_int_t var1 = {.data = 123};
    tag = test_svr_t_query_call(15, &var0, &var1);
    printf("ret:%d, val is 0x%x, val is 0x%x\n", msg_tag_get_val(tag), var0.data, var1.data);
#endif
#if 0
    uint8_t data_cache[10] = "test";
    uint8_t data_cache1[10] = "tttt";
    rpc_ref_array_uint32_t_uint8_t_32_t var0 = {
        .data = data_cache,
        .len = strlen(data_cache) + 1};
    rpc_ref_array_uint32_t_uint8_t_32_t var1 = {
        .data = data_cache1,
        .len = strlen(data_cache1) + 1};
    tag = test_svr_t_query_call(15, &var0, &var1);
    printf("ret:%d, val is %s, val is %s.\n", msg_tag_get_val(tag), var0.data, var1.data);
#endif
    rpc_obj_handler_t_t var0 = {.data = 0};
    rpc_obj_handler_t_t var1 = {.data = 12};
    tag = test_svr_t_query_call(15, &var0, &var1);
    printf("ret:%d\n", msg_tag_get_val(tag));
}