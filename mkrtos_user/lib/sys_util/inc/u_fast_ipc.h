#pragma once

#include <u_types.h>

bool_t u_fast_ipc_is_init(void);
int u_fast_ipc_init(uint8_t *stack_array, uint8_t *msg_buf_array, int stack_msgbuf_array_num, size_t stack_size,
                    obj_handler_t *threads_obj);
