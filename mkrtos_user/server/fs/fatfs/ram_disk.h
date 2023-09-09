#pragma once

#include "u_types.h"
int ram_disk_read(uint8_t *buff, uint32_t sector, uint32_t count);
int ram_disk_write(uint8_t *buff, uint32_t sector, uint32_t count);
int ram_disk_sector_nr(void);