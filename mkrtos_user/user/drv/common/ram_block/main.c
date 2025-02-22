
#include "u_share_mem.h"
#include <assert.h>
#include <blk_drv_cli.h>
#include <blk_drv_svr.h>
#include <errno.h>
#include <ns_cli.h>
#include <rpc_prot.h>
#include <stdio.h>
#include <sys/stat.h>
#include <u_fast_ipc.h>
#include <u_hd_man.h>
#include <u_sleep.h>
#include <u_sys.h>
#include <u_task.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <getopt.h>
#include "u_hd_man.h"
#include "u_vmam.h"
#define STACK_COM_ITME_SIZE (1024 + 512)
ATTR_ALIGN(8)
static uint8_t stack_coms[STACK_COM_ITME_SIZE];
static uint8_t msg_buf_coms[MSG_BUG_LEN];
static obj_handler_t com_th_obj;

static void fast_ipc_init(void)
{
    com_th_obj = handler_alloc();
    assert(com_th_obj != HANDLER_INVALID);
    u_fast_ipc_init(stack_coms, msg_buf_coms, 1, STACK_COM_ITME_SIZE, &com_th_obj);
}

static blk_drv_t blk_drv;
static uint8_t *blk_data;
static int blk_nr;
static int blk_size;

int blk_drv_write(obj_handler_t obj, int len, int inx)
{
    int ret = -1;
    addr_t addr = 0;
    umword_t size = 0;
    msg_tag_t tag;

    if (len == 0)
    {
        memset(blk_data + inx * blk_size, 0xff, len);
    }
    else
    {
        tag = share_mem_map(obj, vma_addr_create(VPAGE_PROT_RWX, 0, 0), &addr, &size);
        if (msg_tag_get_val(tag) < 0)
        {
            handler_free_umap(obj);
            printf("net write error.\n");
            return msg_tag_get_val(tag);
        }
        memcpy(blk_data + inx * blk_size, (void *)addr, len);
        handler_free_umap(obj);
    }
    return MIN(len, size);
}
int blk_drv_read(obj_handler_t obj, int len, int inx)
{
    int ret = -1;
    addr_t addr = 0;
    umword_t size = 0;
    uint32_t _err;

    if (len % blk_size)
    {
        return -EINVAL;
    }
    if (inx + (len / blk_size) >= blk_nr)
    {
        return -1;
    }
    msg_tag_t tag;

    tag = share_mem_map(obj, vma_addr_create(VPAGE_PROT_RWX, 0, 0), &addr, &size); /*FIXME:优化*/

    if (msg_tag_get_val(tag) < 0)
    {
        handler_free_umap(obj);
        printf("ramblock write error.\n");
        return msg_tag_get_val(tag);
    }
    memcpy((void *)addr, blk_data + inx * blk_size, MIN(len, size));
    handler_free_umap(obj);
    return MIN(len, size);
}
int blk_drv_map(obj_handler_t *hd)
{
    return -ENOSYS;
}
int blk_drv_info(blk_drv_info_t *info)
{
    info->blk_nr = blk_nr;
    info->blk_size = blk_size;
    info->blk_start_addr = blk_data;
    return 0;
}

int main(int argc, char *argv[])
{
    obj_handler_t hd;

    int opt;
    int ret = 0;
    char *dev_path = NULL;
    // appfs_test_func();

    while ((opt = getopt(argc, argv, "s:n:p:")) != -1)
    {
        switch (opt)
        {
        case 's':
            blk_size = atol(optarg);
            break;
        case 'n':
            blk_nr = atol(optarg);
            break;
        case 'p':
            dev_path = optarg;
        default:
            break;
        }
    }
    if (blk_nr == 0 || blk_size == 0 || (blk_size % MK_PAGE_SIZE) != 0)
    {
        printf("params is error.\n");
        return -1;
    }
    if (dev_path == NULL)
    {
        printf("please set dev path.\n");
        printf("example:ram_block -p /block -s 512 -n 2048");
        return -1;
    }
    blk_data = mmap(0, blk_nr * blk_size, PROT_PFS | PROT_READ | PROT_WRITE, MAP_PRIVATE, -1, 0);
    if (blk_data == MAP_FAILED)
    {
        printf("sys not have memory.\n");
        return -1;
    }

    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_blk");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_blk");
    printf("%s init..\n", argv[0]);
    fast_ipc_init();

    blk_drv_init(&blk_drv);
    ret = rpc_meta_init(THREAD_MAIN, &hd);
    assert(ret >= 0);
    ns_register(dev_path, hd, FILE_NODE);
    meta_reg_svr_obj(&blk_drv.svr, BLK_DRV_PROT);

    while (1)
    {
        u_sleep_ms(0);
    }
}
