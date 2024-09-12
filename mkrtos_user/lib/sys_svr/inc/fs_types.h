#pragma once

#include <u_rpc.h>
#include <u_thread.h>

#if CONFIG_THREAD_MSG_BUG_LEN == 128
#define FS_RPC_BUF_LEN 400
#define rpc_ref_file_array_t rpc_ref_array_uint32_t_uint8_t_400_t
#define rpc_file_array_t rpc_array_uint32_t_uint8_t_400_t
#else
#define FS_RPC_BUF_LEN 32
#define rpc_ref_file_array_t rpc_ref_array_uint32_t_uint8_t_32_t
#define rpc_file_array_t rpc_array_uint32_t_uint8_t_32_t
#endif
