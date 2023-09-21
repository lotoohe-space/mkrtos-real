
#include "u_types.h"
#include "ram_disk.h"
#include <string.h>
#define RAM_DISK_SECTOR_NR 512

static uint8_t ram_disk[RAM_DISK_SECTOR_NR * 512];

int ram_disk_sector_nr(void)
{
    return RAM_DISK_SECTOR_NR;
}
int ram_disk_read(uint8_t *buff, uint32_t sector, uint32_t count)
{
    if (sector + count >= RAM_DISK_SECTOR_NR)
    {
        return -1;
    }
    for (int i = sector; i < sector + count; i++)
    {
        memcpy(buff + (i - sector) * 512, &ram_disk[i * 512], 512);
    }
    return 0;
}
int ram_disk_write(uint8_t *buff, uint32_t sector, uint32_t count)
{
    if (sector + count >= RAM_DISK_SECTOR_NR)
    {
        return -1;
    }
    for (int i = sector; i < sector + count; i++)
    {
        memcpy(&ram_disk[i * 512], buff + (i - sector) * 512, 512);
    }
    return 0;
}
