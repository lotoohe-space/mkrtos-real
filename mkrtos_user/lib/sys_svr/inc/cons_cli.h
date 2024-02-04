#pragma once
#include <u_types.h>
#include <string.h>
int cons_write(const uint8_t *data, int len);
static inline int cons_write_str(const char *str)
{
    return cons_write((const uint8_t *)str, (int)strlen(str));
}
int cons_read(uint8_t *data, int len);
int cons_active(void);
