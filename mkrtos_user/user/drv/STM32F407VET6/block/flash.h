#pragma once

#include <u_types.h>

int flash_read_sector(uint32_t sector_inx, uint32_t *p_buffer, uint16_t num_read);
int flash_write_sector(uint32_t sector_inx, uint32_t *p_buffer, uint16_t num_write);
int flash_get_sector_size(umword_t *mem_addr, int *blk_size, int *blk_nr);
int flash_erase_sector(uint32_t sector_inx);
int flash_init(void);
