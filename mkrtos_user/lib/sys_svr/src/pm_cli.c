
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "rpc_prot.h"
#include "u_env.h"
#include "u_prot.h"
#include "u_hd_man.h"
#include "pm_svr.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

RPC_GENERATION_CALL2(pm_t, PM_PROT, PM_RUN_APP, run_app,
                     rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)

int pm_run_app(const char *path, int flags)
{
    rpc_ref_array_uint32_t_uint8_t_32_t rpc_path = {
        .data = (uint8_t *)path,
        .len = strlen(path) + 1,
    };
    rpc_int_t rpc_flags = {
        .data = flags,
    };
    msg_tag_t tag = pm_t_run_app_call(u_get_global_env()->ns_hd, &rpc_path, &rpc_flags);

    return msg_tag_get_val(tag);
}
