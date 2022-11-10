//
// Created by Administrator on 2022/1/16.
//
#include <type.h>
#include <mkrtos/fs.h>
#include <bsp/lib/sys.h>
#include <mkrtos/dev.h>
#include <gd32f10x.h>

#define BK_SIZE 2048
#define BK_COUNT 61
#define FLASH_W_BASE_ADDR        (0x8000000 + 67*BK_SIZE)
#define FLASH_BK_CACHE_LEN 2

static int32_t read_bk(uint32_t bk_no, uint8_t *data) {
	if (bk_no >= BK_COUNT) {
		return -1;
	}
	for (int i = 0; i < BK_SIZE / 4; i++) {
		((uint32_t*) data)[i] = *((volatile uint32_t*) (FLASH_W_BASE_ADDR
				+ bk_no * BK_SIZE + (i << 2)));
	}

	return 0;
}
static int32_t write_bk(uint32_t bk_no, uint8_t *data) {
	fmc_state_enum fmcstatus;

	if (bk_no >= BK_COUNT) {
		return -1;
	}

	fmc_unlock();
    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
	for (int i = 0; i < BK_SIZE / 4; i++) {
		fmcstatus = fmc_word_program(
				FLASH_W_BASE_ADDR + bk_no * BK_SIZE + (i << 2),
				((uint32_t*) data)[i]);
		if (fmcstatus != FMC_READY) {
			fmc_lock();
			return -1;
		}
	    fmc_flag_clear(FMC_FLAG_BANK0_END);
	    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
	    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
	}
	fmc_lock();
	//kprint("w bk:%d.\n", bk_no);
	return 0;
}
static int32_t erase_bk(uint32_t bk_no) {
	fmc_state_enum fmcstatus;
	if (bk_no >= BK_COUNT) {
		return -1;
	}
	fmc_unlock();
	fmcstatus = fmc_page_erase(FLASH_W_BASE_ADDR + (BK_SIZE * bk_no));
    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

	if (fmcstatus != FMC_READY) {
		fmc_lock();
		return -1;
	}
	fmc_lock();
	//kprint("e bk:%d.\n", bk_no);
	return 0;
}

static struct bk_operations bk_flash = { .read_bk = read_bk, .write_bk =
		write_bk, .erase_bk = erase_bk };

static struct file_operations bk_ops = { .open = bk_file_open, .read =
		bk_file_read, .write = bk_file_write, .fsync = bK_file_fsync, .release =
		bk_file_release };
dev_t used_dev_no = BK_FLASH;
#define DEV_NAME "flash"
/**
 * 注册设备驱动，注册成功返回设备驱动号
 * @return
 */
int32_t bk_flash_init(void) {
	if (request_bk_no(BK_FLASH) < 0) {
		sys_mknod("/dev/flash", MKDEV(0777, BK_FLASH), BK_FLASH);
		return -1;
	}

	if (reg_bk_dev(BK_FLASH, DEV_NAME, &bk_flash, &bk_ops) < 0) {
		return -1;
	}
	bk_dev_reg_param(BK_FLASH, BK_COUNT, FLASH_BK_CACHE_LEN, BK_SIZE);
	return used_dev_no;
}
int32_t bk_flash_exit(void) {
	unreg_bk_dev(used_dev_no, DEV_NAME);

}
//DEV_BK_EXPORT(bk_flash_init, bk_flash_exit, flash);
