#pragma once

#include "u_rpc_svr.h"
#include "u_types.h"

typedef struct fs
{
    rpc_svr_obj_t svr;
    obj_handler_t ipc;
} fs_t;

void fs_init(fs_t *fs);

int fs_svr_open(const char *path, int flags, int mode);
