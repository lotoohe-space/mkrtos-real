#include "hw_block_test.h"
#include "appfs.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE 512  //!< 块大小
#define BLOCK_NR   1792 //!< 块数量

#define BOCK_MEM_SIZE (BLOCK_NR * BLOCK_SIZE) //!< 块设备内存大小
static uint8_t block[BOCK_MEM_SIZE];          // 32MB
static uint8_t block_buf[BLOCK_SIZE];         //!< 块缓冲区

static int hw_erase_block(fs_info_t *fs, int block_inx)
{
    assert(fs);
    assert(block_inx < fs->save.block_nr);
    memset(block + block_inx * fs->save.block_size, 0xff, fs->save.block_size);
    return 0;
}
static int hw_write_and_erase_block(fs_info_t *fs, int block_inx, void *buf, int size)
{
    assert(fs);
    assert(buf);
    assert(block_inx < fs->save.block_nr);
    memset(block + block_inx * fs->save.block_size, 0xff, fs->save.block_size);
    memcpy(block + block_inx * fs->save.block_size, buf, size);
    return 0;
}
static int hw_read_block(fs_info_t *fs, int block_inx, void *buf, int size)
{
    assert(fs);
    assert(buf);
    assert(block_inx < fs->save.block_nr);
    memcpy(buf, block + block_inx * fs->save.block_size, size);
    return 0;
}
static int hw_init_fs_for_block(fs_info_t *fs)
{
    fs->save.block_size = BLOCK_SIZE;
    fs->save.block_nr = BLOCK_NR;
    fs->dev_fd = -1; // 模拟设备没有文件描述符
    fs->mem_addr = (unsigned long)block;
    fs->buf = (uint32_t *)block_buf;

    return 0;
}
int hw_init_block_test(fs_info_t *fs)
{
    fs->cb.hw_erase_block = hw_erase_block;
    fs->cb.hw_init_fs_for_block = hw_init_fs_for_block;
    fs->cb.hw_read_block = hw_read_block;
    fs->cb.hw_write_and_erase_block = hw_write_and_erase_block;

    // 使用内存模拟一个块设备
    memset(block, 0, BOCK_MEM_SIZE);
    return 0;
}
