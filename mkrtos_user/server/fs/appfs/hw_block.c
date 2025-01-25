#include "appfs.h"
#include "hw_block_test.h"
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <u_hd_man.h>
#include <u_vmam.h>
#include <u_prot.h>
#include <u_task.h>
#include <u_factory.h>
#include <u_share_mem.h>
#include <u_sleep.h>
#include <blk_drv_cli.h>
#include <ns_cli.h>
#include <blk_drv_types.h>

static obj_handler_t shm_hd;

static int block_mem_addr = -1;
static int block_nr = -1;
static uint8_t *block_buf; //!< 块缓冲区
static obj_handler_t blk_drv_hd;
static blk_drv_info_t blk_info;

static int hw_erase_block(fs_info_t *fs, int block_inx)
{
    int ret;
    assert(fs);
    assert(block_inx < fs->save.block_nr);

    ret = blk_drv_cli_write(blk_drv_hd, shm_hd, 0, block_inx);
    return ret;
}
static int hw_write_and_erase_block(fs_info_t *fs, int block_inx, void *buf, int size)
{
    int ret;
    assert(fs);
    assert(buf);
    assert(block_inx < fs->save.block_nr);

    ret = blk_drv_cli_write(blk_drv_hd, shm_hd, size, block_inx);
    return ret;
}
static int hw_read_block(fs_info_t *fs, int block_inx, void *buf, int size)
{
    assert(fs);
    assert(buf);
    assert(block_inx < fs->save.block_nr);
    int ret;

    ret = blk_drv_cli_read(blk_drv_hd, shm_hd, size, block_inx);
    return ret;
}
static int hw_init_fs_for_block(fs_info_t *fs)
{
    fs->save.block_size = blk_info.blk_size;
    fs->save.block_nr = blk_info.blk_nr;
    fs->dev_fd = -1;
    fs->mem_addr = (unsigned long)blk_info.blk_start_addr;
    fs->buf = (uint32_t *)block_buf;

    return 0;
}
static int hw_create_shm_obj(int size, obj_handler_t *shm_obj)
{
    msg_tag_t tag;
    obj_handler_t shm_hd;

    assert(shm_obj);

    shm_hd = handler_alloc();
    if (shm_hd == HANDLER_INVALID)
    {
        printf("handler alloc fail.\n");
        return -ENOMEM;
    }
    tag = facotry_create_share_mem(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, shm_hd),
                                   SHARE_MEM_CNT_BUDDY_CNT, size);
    if (msg_tag_get_prot(tag) < 0)
    {
        printf("facotry_create_share_mem fail.\n");
        handler_free(shm_hd);
        return -ENOMEM;
    }
    *shm_obj = shm_hd;
    return 0;
}
int hw_init_block(fs_info_t *fs, const char *dev_path)
{
    int ret;
    msg_tag_t tag;
    addr_t shm_addr;

    fs->cb.hw_erase_block = hw_erase_block;
    fs->cb.hw_init_fs_for_block = hw_init_fs_for_block;
    fs->cb.hw_read_block = hw_read_block;
    fs->cb.hw_write_and_erase_block = hw_write_and_erase_block;

again:
    ret = ns_query(dev_path, &blk_drv_hd, 0x1);
    if (ret < 0)
    {
        u_sleep_ms(50);
        goto again;
    }
    ret = blk_drv_cli_info(blk_drv_hd, &blk_info);
    if (ret < 0)
    {
        printf("blk info get fail.\n");
        return ret;
    }
    ret = hw_create_shm_obj(blk_info.blk_size, &shm_hd);
    if (ret < 0)
    {
        printf("shm obj create fail.\n");
        return ret;
    }
    tag = share_mem_map(shm_hd, vma_addr_create(VPAGE_PROT_RW, VMA_ADDR_RESV, 0), &shm_addr, NULL);
    if (msg_tag_get_prot(tag) < 0)
    {
        printf("shm mem map fail.\n");
        handler_free_umap(shm_hd);
        return -ENOMEM;
    }
    block_buf = (void *)shm_addr;
    printf("block_buf addr:0x%x\n", block_buf);
    // printf("img size is %d, block nr is %d.\n", block_mem_size, block_nr);
    return 0;
}
