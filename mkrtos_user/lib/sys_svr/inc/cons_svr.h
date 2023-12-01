#pragma once

#include <u_rpc_svr.h>
#include <pthread.h>
#include <u_queue.h>
#include <u_types.h>

#define CONS_WRITE_BUF_SIZE (128 + 1)

typedef struct cons
{
    rpc_svr_obj_t svr;
    queue_t r_queue;
    uint8_t r_data[CONS_WRITE_BUF_SIZE];
    pthread_spinlock_t r_lock;
    pthread_mutex_t w_lock;
    pid_t active_pid;
    obj_handler_t hd_cons_read;
    uint8_t r_data_buf[16];
} cons_t;

void cons_svr_obj_init(cons_t *cons);

int console_write(uint8_t *data, size_t len);
int console_read(uint8_t *data, size_t len);
void console_active(void);
