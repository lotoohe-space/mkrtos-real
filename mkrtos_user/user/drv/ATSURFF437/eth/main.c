

#include <stdio.h>
#include <u_vmam.h>
#include <at32f435_437_conf.h>
#include <u_sleep.h>
#include <u_sys.h>
#include <u_task.h>
#include <mk_dtb_parse.h>
#include <mk_dev.h>
#include <mk_drv.h>
#include "ns_cli.h"
#include "rpc_prot.h"
#include "u_hd_man.h"
#include "u_share_mem.h"
#include <blk_drv_svr.h>
#include <assert.h>
#include <sys/stat.h>
#include "mk_eth_drv_impl.h"
#include <u_fast_ipc.h>

#include <errno.h>
#include "at_surf_f437_board_emac.h"
#include "u_hd_man.h"
static blk_drv_t net_drv; //!< 网络驱动的协议
#define STACK_COM_ITME_SIZE (2048)
ATTR_ALIGN(8)
uint8_t stack_coms[STACK_COM_ITME_SIZE];
uint8_t msg_buf_coms[MSG_BUG_LEN];
static obj_handler_t com_th_obj;

void fast_ipc_init(void)
{
    com_th_obj = handler_alloc();
    assert(com_th_obj != HANDLER_INVALID);
    u_fast_ipc_init(stack_coms, msg_buf_coms, 1, STACK_COM_ITME_SIZE, &com_th_obj);
}
int blk_drv_write(obj_handler_t obj, int len, int inx)
{
    int ret = -1;
    addr_t addr = 0;
    umword_t size = 0;
    msg_tag_t tag = share_mem_map(obj, vma_addr_create(VPAGE_PROT_RWX, 0, 0), &addr, &size);

    if (msg_tag_get_val(tag) < 0)
    {
        handler_free_umap(obj);
        printf("net write error.\n");
        return msg_tag_get_val(tag);
    }

    emac_send_packet((uint8_t *)addr, MIN(size, len));
    // mk_printf("net write len:%d.\n", MIN(size, len));
    handler_free_umap(obj);
    return len;
}
int blk_drv_read(obj_handler_t obj, int len, int inx)
{
    int ret = -1;
    addr_t addr = 0;
    umword_t size = 0;
    msg_tag_t tag = share_mem_map(obj, vma_addr_create(VPAGE_PROT_RWX, 0, 0), &addr, &size);
    uint32_t _err;
    if (msg_tag_get_val(tag) < 0)
    {
        handler_free_umap(obj);
        printf("net read error.\n");
        return msg_tag_get_val(tag);
    }
    ret = emac_read_packet((uint8_t *)addr, size);
    handler_free_umap(obj);
    return ret;
}
int blk_drv_map(obj_handler_t *hd)
{
    *hd = emac_get_sema();
    return 0;
}
int blk_drv_info(blk_drv_info_t *info)
{
    return -ENOSYS;
}
int main(int argc, char *argv[])
{
    obj_handler_t hd;
    int ret;
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_eth");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_eth");
    printf("%s init..\n", argv[0]);
    fast_ipc_init();
    mk_drv_init();
    mk_dev_init();
    drv_eth_init();
    dtb_parse_init();

    blk_drv_init(&net_drv);
    ret = rpc_meta_init_def(TASK_THIS, &hd);
    assert(ret >= 0);
    // fs_svr_init();
    // mkdir("/dev", 0777);
    ns_register("/eth", hd, 0);
    meta_reg_svr_obj(&net_drv.svr, BLK_DRV_PROT);
    while (1)
    {
        u_sleep_ms(U_SLEEP_ALWAYS);
    }
}
