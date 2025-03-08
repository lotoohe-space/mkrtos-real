
#include "rpc_prot.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_hd_man.h"
#include "cons_svr.h"
#include <stdio.h>
/*write*/
RPC_GENERATION_OP1(cons_t, CONS_PROT, CONS_WRITE, write,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, data)
{
    int16_t ret = -1;

    ret = console_write(data->data, data->len);
    return ret;
}

RPC_GENERATION_DISPATCH1(cons_t, CONS_PROT, CONS_WRITE, write,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, data)
/*read*/
RPC_GENERATION_OP2(cons_t, CONS_PROT, CONS_READ, read,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_OUT, RPC_TYPE_DATA, data,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
{
    int16_t ret = -1;

    ret = console_read(data->data, len->data);
    if (ret >= 0)
    {
        data->len = ret;
    }
    return ret;
}

RPC_GENERATION_DISPATCH2(cons_t, CONS_PROT, CONS_READ, read,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_OUT, RPC_TYPE_DATA, data,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
/*active*/
RPC_GENERATION_OP1(cons_t, CONS_PROT, CONS_ACTIVE, active,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
{
    printf("warrning: to map sema obj.\n");
    console_active(thread_get_src_pid(), -1 /*TODO:映射sem*/);
    return 0;
}

RPC_GENERATION_DISPATCH1(cons_t, CONS_PROT, CONS_ACTIVE, active,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)

/*dispatch*/
RPC_DISPATCH3(cons_t, CONS_PROT, typeof(CONS_WRITE), CONS_WRITE, write, CONS_READ, read, CONS_ACTIVE, active)

void cons_svr_obj_init(cons_t *cons)
{
    rpc_svr_obj_init(&cons->svr, rpc_cons_t_dispatch, CONS_PROT);
    cons->active_pid = -1;
    q_init(&cons->r_queue, cons->r_data, CONS_WRITE_BUF_SIZE);
    // pthread_spin_init(&cons->r_lock, 0);
    // pthread_mutex_init(&cons->w_lock, NULL);
}
