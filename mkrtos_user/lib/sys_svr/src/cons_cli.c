
#include "rpc_prot.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_hd_man.h"
#include "u_env.h"
#include "ns_cli.h"
#include "cons_svr.h"
#include <u_env.h>

RPC_GENERATION_CALL1(cons_t, CONS_PROT, CONS_WRITE, write,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, data)
RPC_GENERATION_CALL1(cons_t, CONS_PROT, CONS_READ, read,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_OUT, RPC_TYPE_DATA, data)

RPC_GENERATION_CALL1(cons_t, CONS_PROT, CONS_ACTIVE, active,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)

int cons_write(const uint8_t *data, int len)
{
    int rlen = 0;
    while (rlen < len)
    {
        int r_once_len = 0;

        r_once_len = MIN(32, len - rlen);
        rpc_ref_array_uint32_t_uint8_t_32_t rpc_buf = {
            .data = (uint8_t *)data + rlen,
            .len = r_once_len,
        };
        rpc_int_t rpc_len = {
            .data = r_once_len,
        };
        msg_tag_t tag = cons_t_write_call(u_get_global_env()->ns_hd, &rpc_buf);

        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        rlen += msg_tag_get_val(tag);
        if (msg_tag_get_val(tag) != r_once_len)
        {
            break;
        }
    }
}
int cons_read(uint8_t *data, int len)
{
    rpc_ref_array_uint32_t_uint8_t_32_t rpc_path = {
        .data = (uint8_t *)data,
        .len = len > 32 ? 32 : len,
    };
    msg_tag_t tag = cons_t_read_call(u_get_global_env()->ns_hd, &rpc_path);

    return msg_tag_get_val(tag);
}
int cons_active(void)
{
    rpc_int_t rpc_flags = {
        .data = 0};
    uenv_t *env = u_get_global_env();
    msg_tag_t tag = cons_t_active_call(env->log_hd, &rpc_flags);

    return msg_tag_get_val(tag);
}
