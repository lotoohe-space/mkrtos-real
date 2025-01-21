#pragma once

#include <u_types.h>

int flash_get_sector_size(void);
int flash_read_sector(uint32_t sector_inx, uint32_t *p_buffer, uint16_t num_read);
int flash_write_sector(uint32_t sector_inx, uint32_t *p_buffer, uint16_t num_write);
