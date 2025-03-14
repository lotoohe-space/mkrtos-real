
#include "flash.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include "u_sys.h"
#include "u_prot.h"
#include <u_vmam.h>

#define SECTOR_SIZE 4096
static sys_info_t sys_info;

int flash_get_sector_size(umword_t *mem_addr, int *blk_size, int *blk_nr)
{
    assert(blk_size);
    assert(mem_addr);
    assert(blk_nr);
    msg_tag_t tag;

    tag = sys_read_info(SYS_PROT, &sys_info, 0);
    if (msg_tag_get_val(tag) < 0)
    {
        return -ENOENT;
    }

    *mem_addr = sys_info.bootfs_start_addr;
    *blk_size = SECTOR_SIZE;
    *blk_nr = (CONFIG_SYS_TEXT_SIZE - CONFIG_BOOTSTRAP_TEXT_SIZE - CONFIG_KNL_TEXT_SIZE - CONFIG_DTBO_TEXT_SIZE) / SECTOR_SIZE;

    printf("flash mem_addr:0x%x blk_size:0x%x blk_nr:0x%x\n", *mem_addr, *blk_size, *blk_nr);
    return 0;
}
int flash_read_sector(uint32_t sector_inx, uint32_t *p_buffer, uint16_t num_read)
{
    uint16_t i;
    uint32_t read_addr;

    if (num_read < SECTOR_SIZE / 4)
    {
        return -1;
    }
    read_addr = sector_inx * SECTOR_SIZE + sys_info.bootfs_start_addr;

    for (i = 0; i < num_read; i++)
    {
        p_buffer[i] = *(uint32_t *)(read_addr);
        read_addr += 4;
    }
    return num_read;
}
int flash_erase_sector(uint32_t sector_inx)
{
    return -ENOSYS;
}
int flash_write_sector(uint32_t sector_inx, uint32_t *p_buffer, uint16_t num_write)
{
    return -ENOSYS;
}

int flash_init(void)
{
    return 0;
}
