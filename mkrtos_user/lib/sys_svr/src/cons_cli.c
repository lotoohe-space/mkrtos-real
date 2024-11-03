
#include "cons_svr.h"
#include "ns_cli.h"
#include "rpc_prot.h"
#include "u_env.h"
#include "u_hd_man.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include <u_env.h>

RPC_GENERATION_CALL1(cons_t, CONS_PROT, CONS_WRITE, write,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, data)
// RPC_GENERATION_CALL2(cons_t, CONS_PROT, CONS_READ, read,
//                      rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_OUT, RPC_TYPE_DATA, data,
//                      rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
msg_tag_t cons_t_read_call(obj_handler_t hd, rpc_ref_array_uint32_t_uint8_t_32_t *var0, rpc_int_t *var1)
{
    void *buf;
    ipc_msg_t *msg_ipc;
    int off = 0;
    int off_buf = 0;
    int ret = -1;
    umword_t op_val = ((umword_t)1);

    thread_msg_buf_get(-1, (umword_t *)(&buf), ((void *)0));
    msg_ipc = (ipc_msg_t *)buf;
    msg_ipc->msg_buf[0] = op_val;
    off += rpc_align(sizeof(op_val), __alignof(((umword_t)1)));

    do
    {
        if (1 == 1)
        {
            if (2 == 1 || 2 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_ref_array_uint32_t_uint8_t_32_t(var0, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off);
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
            if (2 == 1 || 2 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_ref_array_uint32_t_uint8_t_32_t(var0, (uint8_t *)((uint8_t *)msg_ipc->map_buf), off_buf);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_buf = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 1)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_cli_msg_to_buf_rpc_int_t(var1, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off);
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
                int ret = rpc_cli_msg_to_buf_rpc_int_t(var1, (uint8_t *)((uint8_t *)msg_ipc->map_buf), off_buf);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_buf = ret;
            }
        }
    } while (0);
    msg_tag_t tag = thread_ipc_call(((msg_tag_t){.flags = (0), .msg_buf_len = ((((off) / ((sizeof(void *)))) + (((off) % ((sizeof(void *)))) ? 1 : 0))), .map_buf_len = ((((off_buf) / ((sizeof(void *)))) + (((off_buf) % ((sizeof(void *)))) ? 1 : 0))), .prot = (0x0006)}), hd, ipc_timeout_create2(0, 0));
    if (((int)((tag).prot)) < 0)
    {
        return tag;
    }
    off = 0;
    do
    {
        if (1 == 1)
        {
            if (2 == 2 || 2 == 4)
            {
                int ret = rpc_cli_buf_to_msg_rpc_ref_array_uint32_t_uint8_t_32_t(var0, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off, tag.msg_buf_len * (sizeof(void *)));
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
        if (1 == 1)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_cli_buf_to_msg_rpc_int_t(var1, (uint8_t *)((uint8_t *)msg_ipc->msg_buf), off, tag.msg_buf_len * (sizeof(void *)));
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
RPC_GENERATION_CALL1(cons_t, CONS_PROT, CONS_ACTIVE, active,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)

int cons_write(const uint8_t *data, int len)
{
    int rlen = 0;
    uenv_t *env = u_get_global_env();

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
        msg_tag_t tag = cons_t_write_call(env->log_hd, &rpc_buf);

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
    return rlen;
}
int cons_read(uint8_t *data, int len)
{
    rpc_ref_array_uint32_t_uint8_t_32_t rpc_path = {
        .data = (uint8_t *)data,
    };
    rpc_int_t rpc_len = {
        .data = len > 32 ? 32 : len,
    };
    uenv_t *env = u_get_global_env();

    msg_tag_t tag = cons_t_read_call(env->log_hd, &rpc_path, &rpc_len);

    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    return rpc_path.len;
}
int cons_active(void)
{
    rpc_int_t rpc_flags = {
        .data = 0};
    uenv_t *env = u_get_global_env();
    msg_tag_t tag = cons_t_active_call(env->log_hd, &rpc_flags);

    return msg_tag_get_val(tag);
}
