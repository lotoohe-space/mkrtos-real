
#include "u_rpc.h"
typedef struct test_svr
{

} test_svr_t;

RPC_GENERATION_CALL1(test_svr_t, 0, register, rpc_int_t, RPC_DIR_IN, RPC_TYPE_BUF, var0)
RPC_GENERATION_CALL2(test_svr_t, 1, query,
                     rpc_ref_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, var0, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, var1)

#include <string.h>
void rpc_test(void)
{
    rpc_int_t var0 = {.data = 1};
    msg_tag_t tag = test_svr_t_register_call(15, &var0);

    rpc_ref_array_uint32_t_uint8_t_32_t str_tst = {
        .data = "test",
        .len = strlen("test") + 1};

    test_svr_t_query_call(15, &str_tst, &var0);
}