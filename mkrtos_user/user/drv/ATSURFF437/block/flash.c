
#include "flash.h"
#include "at32f435_437_conf.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include "u_sys.h"
#include "u_prot.h"
#include <u_vmam.h>
#if defined(AT32F437xM) || defined(AT32F435xM) || defined(AT32F437xD) || defined(AT32F435xD)
#define SECTOR_SIZE 4096 /* this parameter depends on the specific model of the chip */
#else
#define SECTOR_SIZE 2048 /* this parameter depends on the specific model of the chip */
#endif
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
    flash_status_type status = FLASH_OPERATE_DONE;

    flash_unlock();
    status = flash_sector_erase(sector_inx * SECTOR_SIZE + sys_info.bootfs_start_addr);
    if (status != FLASH_OPERATE_DONE)
    {
        flash_lock();
        return -1;
    }
    flash_lock();
    return 0;
}
int flash_write_sector(uint32_t sector_inx, uint32_t *p_buffer, uint16_t num_write)
{
    flash_status_type status = FLASH_OPERATE_DONE;
    uint32_t write_addr = sector_inx * SECTOR_SIZE + sys_info.bootfs_start_addr;

    if (num_write < SECTOR_SIZE / 4)
    {
        return -1;
    }

    flash_unlock();
    status = flash_sector_erase(sector_inx * SECTOR_SIZE + sys_info.bootfs_start_addr);
    if (status != FLASH_OPERATE_DONE)
    {
        flash_lock();
        return -1;
    }
    for (int i = 0; i < num_write; i++)
    {
        status = flash_word_program(write_addr, p_buffer[i]);
        if (status != FLASH_OPERATE_DONE)
        {
            flash_lock();
            return -1;
        }
        write_addr += 4;
    }
    flash_lock();
    return num_write;
}

int flash_init(void)
{
    msg_tag_t tag;
    addr_t vaddr;

    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RW, VMA_ADDR_PAGE_FAULT_SIM, 0),
                       0x400 /*TODO:*/, 0x40023C00, &vaddr);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }

    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RW, VMA_ADDR_PAGE_FAULT_SIM, 0),
                       32 * 1024 * 1024 /*TODO:*/, 0x8000000, &vaddr);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    return 0;
}
