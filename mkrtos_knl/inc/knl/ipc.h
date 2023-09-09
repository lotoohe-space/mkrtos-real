#pragma once
#include "thread.h"
#include "types.h"

#define IPC_MSG_SIZE 96
#define MAP_BUF_SIZE 16
#define IPC_USER_SIZE 12

#if (IPC_MSG_SIZE + MAP_BUF_SIZE + IPC_USER_SIZE) > THREAD_MSG_BUG_LEN
#error "IPC MSG len error."
#endif

typedef struct ipc_msg
{
    union
    {
        struct
        {
            umword_t msg_buf[IPC_MSG_SIZE / WORD_BYTES];
            umword_t map_buf[MAP_BUF_SIZE / WORD_BYTES];
            umword_t user[IPC_USER_SIZE / WORD_BYTES];
        };
        uint8_t data[THREAD_MSG_BUG_LEN];
    };
} ipc_msg_t;

typedef union ipc_timeout
{
    umword_t raw;
    struct
    {
        uhmword_t send_timeout;
        uhmword_t recv_timeout;
    };
} ipc_timeout_t;

static inline ipc_timeout_t ipc_timeout_create2(uhmword_t send_timeout, uhmword_t recv_timeout)
{
    return (ipc_timeout_t){
        .send_timeout = send_timeout,
        .recv_timeout = recv_timeout,
    };
}
static inline ipc_timeout_t ipc_timeout_create(umword_t raw)
{
    return (ipc_timeout_t){
        .raw = raw,
    };
}

void timeout_times_tick(void);
