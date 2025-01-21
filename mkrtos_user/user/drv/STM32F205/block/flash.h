#pragma once

#include <u_types.h>
int flash_init(void);
int flash_write(addr_t st_addr, addr_t end_addr, umword_t *data);
