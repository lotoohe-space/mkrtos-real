#include "appfs.h"
#include "hw_block_test.h"
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int block_size = -1;
static int block_mem_size = -1;
static int block_nr = -1;
static uint8_t *block;     // 32MB
static uint8_t *block_buf; //!< 块缓冲区

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
    fs->save.block_size = block_size;
    fs->save.block_nr = block_nr;
    fs->dev_fd = -1; // 模拟设备没有文件描述符
    fs->mem_addr = (unsigned long)block;
    fs->buf = (uint32_t *)block_buf;

    return 0;
}
int hw_read_file_all(const char *file_path, void **ret_ptr)
{
    FILE *file = fopen(file_path, "rbw+"); // 以二进制模式打开文件
    if (!file)
    {
        perror("fopen");
        return EXIT_FAILURE;
    }

    // 寻求文件末尾以确定文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size < 0)
    {
        perror("ftell");
        fclose(file);
        return EXIT_FAILURE;
    }

    // 分配内存来存储文件内容
    char *content = (char *)malloc((size_t)file_size);
    if (!content)
    {
        perror("malloc");
        fclose(file);
        return EXIT_FAILURE;
    }

    // 读取文件内容到内存中
    size_t bytes_read = fread(content, 1, (size_t)file_size, file);
    if (bytes_read != (size_t)file_size)
    {
        perror("fread");
        free(content);
        fclose(file);
        return EXIT_FAILURE;
    }
    *ret_ptr = content;
    fclose(file);
    return file_size;
}

int hw_offload_img_from_file(const char *name)
{
    int ret;
    void *ptr_file_content;

    ret = hw_read_file_all(name, &ptr_file_content);
    if (ret < 0)
    {
        return ret;
    }
    if (ret != block_mem_size)
    {
        free(ptr_file_content);
        printf("input img size is error.\n");
        return -1;
    }
    memcpy(block, ptr_file_content, block_mem_size);
    free(ptr_file_content);
    return 0;
}
int hw_dump_to_file(fs_info_t *fs, const char *name, int mode)
{
    int ret;

    unlink(name);

    int fd = open(name, O_RDWR | O_CREAT, 0777);
    if (fd < 0)
    {
        printf("%s file open faile %d.\n", name, fd);
        return fd;
    }
    int write_size;

    if (mode)
    {
        write_size = appfs_get_total_size(fs) - appfs_get_available_size(fs);
    }
    else
    {
        write_size = block_mem_size;
    }
    printf("write img size is %dB.\n", write_size);
    ret = write(fd, block, write_size);
    if (ret < 0)
    {
        return ret;
    }
    close(fd);
    return 0;
}
int hw_init_block_sim_test(fs_info_t *fs, int img_size, int blk_size)
{
    fs->cb.hw_erase_block = hw_erase_block;
    fs->cb.hw_init_fs_for_block = hw_init_fs_for_block;
    fs->cb.hw_read_block = hw_read_block;
    fs->cb.hw_write_and_erase_block = hw_write_and_erase_block;

    block = (void *)malloc(img_size);
    if (block == NULL)
    {
        return -ENOMEM;
    }
    block_buf = (void *)malloc(blk_size);
    if (block_buf == NULL)
    {
        free(block);
        return -ENOMEM;
    }
    block_size = blk_size;
    block_mem_size = (img_size / blk_size) * blk_size;
    block_nr = img_size / blk_size;
    printf("img size is %d, block nr is %d, block_size %d.\n", block_mem_size, block_nr, blk_size);
    // 使用内存模拟一个块设备
    memset(block, 0, block_mem_size);
    return 0;
}
