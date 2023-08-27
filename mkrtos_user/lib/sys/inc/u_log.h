#pragma once

#include "u_arch.h"
#include "u_types.h"
#define ULOG_RW_MAX_BYTES (WORD_BYTES * 5)
void ulog_write_bytes(obj_handler_t obj_inx, const uint8_t *data, umword_t len);
void ulog_write_str(obj_handler_t obj_inx, const char *str);
