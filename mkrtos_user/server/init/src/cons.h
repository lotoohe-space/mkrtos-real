#pragma once
#include <u_types.h>
void console_init(void);
void console_active(mk_pid_t pid, obj_handler_t sem);
int console_write(uint8_t *data, size_t len);
int console_read(uint8_t *data, size_t len);