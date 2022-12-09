//
// Created by Administrator on 2022/1/16.
//
#include <type.h>
#include <mkrtos/fs.h>
#include <mkrtos/dev.h>
#include <stm32_sys.h>
#include <stm32f2xx.h>

#define BK_CACHE_SIZE (64*1024)

static uint8_t *bk_cache = (uint8_t *)0x20010000;

#define BK_SIZE 512
#define BK_COUNT (BK_CACHE_SIZE/BK_SIZE)
#define FLASH_BK_CACHE_LEN 		2

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
	mkrtos_memcpy(bk_cache+bk_no*BK_SIZE, data, BK_SIZE);
	//kprint("w bk:%d.\n", bk_no);
	return 0;
}
static int32_t erase_bk(uint32_t bk_no) {

	if (bk_no >= BK_COUNT) {
		return -1;
	}
	mkrtos_memset(bk_cache+bk_no*BK_SIZE, 0xff, BK_SIZE);
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
	bk_dev_reg_param(BK_FLASH, BK_COUNT, FLASH_BK_CACHE_LEN, BK_SIZE, (void *)bk_cache);
	return used_dev_no;
}
int32_t bk_flash_exit(void) {
	unreg_bk_dev(used_dev_no, DEV_NAME);

}
//DEV_BK_EXPORT(bk_flash_init, bk_flash_exit, flash);
