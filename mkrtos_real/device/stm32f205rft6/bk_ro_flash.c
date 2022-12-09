//
// Created by Administrator on 2022/1/16.
//
#include <type.h>
#include <mkrtos/fs.h>
#include <mkrtos/dev.h>
#include <stm32_sys.h>
#include <stm32f2xx.h>

#define BK_CACHE_SIZE (512*1024)

static volatile uint8_t *bk_cache = (volatile uint8_t *)0x08040000;

#define BK_SIZE 512
#define BK_COUNT (BK_CACHE_SIZE/BK_SIZE)
#define FLASH_BK_CACHE_LEN 1

static int32_t read_bk(uint32_t bk_no, uint8_t *data) {
	if (bk_no >= BK_COUNT) { 
		return -1;
	}
	mkrtos_memcpy(data, bk_cache+bk_no*BK_SIZE, BK_SIZE);
	return 0;
}
static int32_t write_bk(uint32_t bk_no, uint8_t *data) {

	if (bk_no >= BK_COUNT) {
		return -1;
	}
	//mkrtos_memcpy(bk_cache+bk_no*BK_SIZE, data, BK_SIZE);
	//kprint("w bk:%d.\n", bk_no);
	return 0;
}
static int32_t erase_bk(uint32_t bk_no) {

	if (bk_no >= BK_COUNT) {
		return -1;
	}
	//mkrtos_memset(bk_cache+bk_no*BK_SIZE, 0xff, BK_SIZE);
	return 0;
}

static struct bk_operations bk_flash = { .read_bk = read_bk, .write_bk =
		write_bk, .erase_bk = erase_bk };

static struct file_operations bk_ops = { .open = bk_file_open, .read =
		bk_file_read, .write = bk_file_write, .fsync = bK_file_fsync, .release =
		bk_file_release };

#define DEV_NAME "ro_flash"

static int32_t ro_bk_flash_init(void) {
	if (request_bk_no(BK_RO_FLASH) < 0) {
		
		return -1;
	}

	if (reg_bk_dev(BK_RO_FLASH, DEV_NAME, &bk_flash, &bk_ops) < 0) {
		return -1;
	}
	bk_dev_reg_param(BK_RO_FLASH, BK_COUNT, FLASH_BK_CACHE_LEN, BK_SIZE, (void *)bk_cache);
	return BK_RO_FLASH;
}
static int32_t ro_bk_flash_exit(void) {
	unreg_bk_dev(BK_RO_FLASH, DEV_NAME);
}
DEV_BK_EXPORT(ro_bk_flash_init, ro_bk_flash_exit, ro_flash);
