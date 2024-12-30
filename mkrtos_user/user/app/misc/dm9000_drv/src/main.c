
#include <printf.h>
#include <assert.h>
#include <u_drv.h>
#include <u_prot.h>
#include <u_mm.h>
#include "dm9000.h"
#include <u_thread.h>
#include <pthread.h>
#include <u_rpc_svr.h>
#include <u_hd_man.h>
#include <net_drv_svr.h>
#include <u_share_mem.h>
#include <rpc_prot.h>
#include <ns_cli.h>

static net_drv_t net_drv; //!< 网络驱动的协议

int net_drv_write(obj_handler_t obj, int len)
{
    int ret = -1;
    addr_t addr = 0;
    umword_t size = 0;
    msg_tag_t tag = share_mem_map(obj, vma_addr_create(VPAGE_PROT_RWX, 0, 0), &addr, &size);

    if (msg_tag_get_val(tag) < 0)
    {
        handler_free_umap(obj);
        mk_printf("net write error.\n");
        return msg_tag_get_val(tag);
    }
    DM9000_SendPacket((uint8_t *)addr, MIN(size, len));
    // mk_printf("net write len:%d.\n", MIN(size, len));
    handler_free_umap(obj);
    return len;
}
int net_drv_read(obj_handler_t obj, int len)
{
    int ret = -1;
    addr_t addr = 0;
    umword_t size = 0;
    msg_tag_t tag = share_mem_map(obj, vma_addr_create(VPAGE_PROT_RWX, 0, 0), &addr, &size);
    uint32_t _err;

    // _err = sem_wait(&dm9000input); // 请求信号量
    // if (_err == 0)
    // {
    ret = DM9000_Receive_Packet((uint8_t *)addr, size);
    if (ret >= 0)
    {
        // mk_printf("net read len:%d.\n", ret);
    }
    // }
    handler_free_umap(obj);
    return ret;
}
int main(int argc, char *args[])
{
    msg_tag_t tag;
    obj_handler_t hd;
    int ret;

    u_drv_init();
    tag = mm_align_alloc(MM_PROT, (void *)0x64000000, 0x1000000);
    assert(msg_tag_get_val(tag) >= 0);

    net_drv_init(&net_drv);

    ret = rpc_meta_init(THREAD_MAIN, &hd);
    assert(ret >= 0);
    meta_reg_svr_obj(&net_drv.svr, NET_DRV_PROT);

    assert(ns_register("/dm9000", hd, MOUNT_NODE) >= 0);
    mk_printf("dm9000 reg success\n");

    assert(DM9000_Init(1) == 0);
    rpc_loop(); //!< loop
    return 0;
}
