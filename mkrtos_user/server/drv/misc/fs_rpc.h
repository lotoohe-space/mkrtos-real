#pragma once

#include <u_types.h>

void fs_svr_init(obj_handler_t ipc);
void fs_svr_loop(void);
void *file_temp_buf_get(void);
