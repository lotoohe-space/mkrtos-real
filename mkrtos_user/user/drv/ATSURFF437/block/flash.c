
#include "flash.h"
#include "at32f435_437_conf.h"
#if defined(AT32F437xM) || defined(AT32F435xM) || defined(AT32F437xD) || defined(AT32F435xD)
#define SECTOR_SIZE 4096 /* this parameter depends on the specific model of the chip */
#else
#define SECTOR_SIZE 2048 /* this parameter depends on the specific model of the chip */
#endif

int flash_get_sector_size(void)
{
    return SECTOR_SIZE;
}
int flash_read_sector(uint32_t sector_inx, uint32_t *p_buffer, uint16_t num_read)
{
    uint16_t i;
    uint32_t read_addr;

    if (num_read < SECTOR_SIZE / 4) {
        return -1;
    }
    read_addr = sector_inx * SECTOR_SIZE + FLASH_BASE;

    for (i = 0; i < num_read; i++) {
        p_buffer[i] = *(uint32_t *)(read_addr);
        read_addr += 4;
    }
    return num_read;
}
int flash_write_sector(uint32_t sector_inx, uint32_t *p_buffer, uint16_t num_write)
{
    flash_status_type status = FLASH_OPERATE_DONE;
    uint32_t write_addr = sector_inx * SECTOR_SIZE + FLASH_BASE;

    if (num_write < SECTOR_SIZE / 4) {
        return -1;
    }

    flash_unlock();
    status = flash_sector_erase(sector_inx * SECTOR_SIZE + FLASH_BASE);
    if (status != FLASH_OPERATE_DONE) {
        flash_lock();
        return -1;
    }
    for (int i = 0; i < num_write; i++) {
        status = flash_word_program(write_addr, p_buffer[i]);
        if (status != FLASH_OPERATE_DONE) {
            flash_lock();
            return -1;
        }
        write_addr += 4;
    }
    flash_lock();
    return num_write;
}
