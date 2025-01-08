#pragma once

#include <u_rpc.h>
#include <u_thread.h>

#if CONFIG_THREAD_MSG_BUG_LEN == 128
#define FS_RPC_BUF_LEN 32
#define rpc_ref_file_array_t rpc_ref_array_uint32_t_uint8_t_32_t
#define rpc_file_array_t rpc_array_uint32_t_uint8_t_32_t
#elif CONFIG_THREAD_MSG_BUG_LEN == 256
#define FS_RPC_BUF_LEN (128)
#define rpc_ref_file_array_t rpc_ref_array_uint32_t_uint8_t_128_t
#define rpc_file_array_t rpc_array_uint32_t_uint8_t_128_t
#elif CONFIG_THREAD_MSG_BUG_LEN == 512
#define FS_RPC_BUF_LEN (128 + 32)
#define rpc_ref_file_array_t rpc_ref_array_uint32_t_uint8_t_160_t
#define rpc_file_array_t rpc_array_uint32_t_uint8_t_160_t
#elif
#define FS_RPC_BUF_LEN 400
#define rpc_ref_file_array_t rpc_ref_array_uint32_t_uint8_t_400_t
#define rpc_file_array_t rpc_array_uint32_t_uint8_t_400_t
#endif