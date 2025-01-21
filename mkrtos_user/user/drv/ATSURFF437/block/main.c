
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
#define STACK_COM_ITME_SIZE (1024 + 512)
ATTR_ALIGN(8)
uint8_t stack_coms[STACK_COM_ITME_SIZE];
uint8_t msg_buf_coms[MSG_BUG_LEN];
void fast_ipc_init(void)
{
    u_fast_ipc_init(stack_coms, msg_buf_coms, 1, STACK_COM_ITME_SIZE);
}
static blk_drv_t blk_drv;
int blk_drv_write(obj_handler_t obj, int len, int inx)
{
    int ret = -1;
    addr_t addr = 0;
    umword_t size = 0;
    msg_tag_t tag = share_mem_map(obj, vma_addr_create(VPAGE_PROT_RWX, 0, 0), &addr, &size);

    if (msg_tag_get_val(tag) < 0) {
        handler_free_umap(obj);
        printf("net write error.\n");
        return msg_tag_get_val(tag);
    }

    ret = flash_write_sector(inx, (void *)addr, len / 4);
    handler_free_umap(obj);
    if (ret < 0) {
        return ret;
    }
    return ret * 4;
}
int blk_drv_read(obj_handler_t obj, int len, int inx)
{
    int ret = -1;
    addr_t addr = 0;
    umword_t size = 0;
    msg_tag_t tag = share_mem_map(obj, vma_addr_create(VPAGE_PROT_RWX, 0, 0), &addr, &size);
    uint32_t _err;

    ret = flash_read_sector(inx, (void *)addr, len / 4);
    handler_free_umap(obj);
    if (ret < 0) {
        return ret;
    }
    return ret * 4;
}
int blk_drv_map(obj_handler_t *hd)
{
    return -ENOSYS;
}
int main(int argc, char *argv[])
{
    obj_handler_t hd;
    int ret;
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_blk");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_blk");
    printf("%s init..\n", argv[0]);
    fast_ipc_init();

    blk_drv_init(&blk_drv);
    ret = rpc_meta_init(THREAD_MAIN, &hd);
    assert(ret >= 0);
    ns_register("/block", hd, FILE_NODE);
    meta_reg_svr_obj(&blk_drv.svr, BLK_DRV_PROT);

    while (1) {
        rpc_loop();
    }
}
