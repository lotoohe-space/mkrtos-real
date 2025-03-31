
#include "flash.h"
#include "u_share_mem.h"
#include <assert.h>
#include <blk_drv_cli.h>
#include <blk_drv_svr.h>
#include <errno.h>
#include <mk_dtb_parse.h>
#include <ns_cli.h>
#include <rpc_prot.h>
#include <stdio.h>
#include <sys/stat.h>
#include <u_fast_ipc.h>
#include <u_hd_man.h>
#include <u_sleep.h>
#include <u_sys.h>
#include <u_task.h>
#include <u_thread.h>
#include "u_hd_man.h"
#define STACK_COM_ITME_SIZE (2048)
ATTR_ALIGN(8)
uint8_t stack_coms[STACK_COM_ITME_SIZE];
uint8_t msg_buf_coms[MSG_BUG_LEN];
static obj_handler_t com_th_obj;
static umword_t cons_map_buf[1][CONFIG_THREAD_MAP_BUF_LEN];

void fast_ipc_init(void)
{
    com_th_obj = handler_alloc();
    assert(com_th_obj != HANDLER_INVALID);
    u_fast_ipc_init(stack_coms, msg_buf_coms, 1, STACK_COM_ITME_SIZE, &com_th_obj, cons_map_buf);
}
static blk_drv_t blk_drv;
int blk_drv_write(obj_handler_t obj, int len, int inx)
{
    int ret = -1;
    addr_t addr = 0;
    umword_t size = 0;
    msg_tag_t tag;

    if (len == 0)
    {
        ret = flash_erase_sector(inx);
    }
    else
    {
        tag = u_share_mem_map(obj, vma_addr_create(VPAGE_PROT_RWX, 0, 0), &addr, &size);
        if (msg_tag_get_val(tag) < 0)
        {
            handler_free_umap(obj);
            printf("net write error.\n");
            return msg_tag_get_val(tag);
        }
        ret = flash_write_sector(inx, (void *)addr, len / 4);
        handler_free_umap(obj);
    }
    if (ret < 0)
    {
        return ret;
    }
    return ret * 4;
}
int blk_drv_read(obj_handler_t obj, int len, int inx)
{
    int ret = -1;
    addr_t addr = 0;
    umword_t size = 0;
    uint32_t _err;
    msg_tag_t tag = u_share_mem_map(obj, vma_addr_create(VPAGE_PROT_RWX, 0, 0), &addr, &size);

    if (msg_tag_get_val(tag) < 0)
    {
        handler_free_umap(obj);
        printf("net write error.\n");
        return msg_tag_get_val(tag);
    }
    ret = flash_read_sector(inx, (void *)addr, len / 4);
    handler_free_umap(obj);
    if (ret < 0)
    {
        return ret;
    }
    return ret * 4;
}
int blk_drv_map(obj_handler_t *hd)
{
    return -ENOSYS;
}
int blk_drv_info(blk_drv_info_t *info)
{
    umword_t mem_addr;
    int blk_size;
    int blk_nr;
    int ret;

    ret = flash_get_sector_size(&mem_addr, &blk_size, &blk_nr);
    if (ret < 0)
    {
        printf("blk drv info error : %d\n", ret);
        return ret;
    }
    info->blk_nr = blk_nr;
    info->blk_size = blk_size;
    info->blk_start_addr = mem_addr;
    printf("[block] 0x%x %d\n", info->blk_start_addr, info->blk_size);
    return 0;
}
int main(int argc, char *argv[])
{
    obj_handler_t hd;
    int ret;

    if (argc < 2)
    {
        printf("please set dev path.\n");
        printf("example:block /block");
        return -1;
    }
    printf("%s init..\n", argv[0]);
    fast_ipc_init();

    flash_init();
    blk_drv_init(&blk_drv);
    ret = rpc_meta_init_def(TASK_THIS, &hd);
    assert(ret >= 0);
    meta_reg_svr_obj(&blk_drv.svr, BLK_DRV_PROT);
    ns_register(argv[1], hd, 0);

    while (1)
    {
        u_sleep_ms(U_SLEEP_ALWAYS);
    }
}
