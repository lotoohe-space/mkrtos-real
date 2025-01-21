
#include "cons_cli.h"
#include "netconf.h"
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "u_sleep.h"
// #include "libc.h"
#include "lwiperf.h"
#include "u_prot.h"

#include <u_hd_man.h>
#include <u_task.h>
#include <u_factory.h>
#include <u_share_mem.h>
#include <u_sema.h>
#include <blk_drv_cli.h>
#include <ns_cli.h>
umword_t addr;
umword_t size;
obj_handler_t net_drv_hd;

int main(int args, char *argv[])
{
    int ret;
    msg_tag_t tag;
    printf("net startup..\n");
again:
    ret = ns_query("/eth", &net_drv_hd, 0x1);
    if (ret < 0)
    {
        u_sleep_ms(50);
        goto again;
    }
    cons_write_str("net init..\n");
    net_init();
    cons_write_str("net start success..\n");
    ip_addr_t perf_server_ip;

    IP_ADDR4(&perf_server_ip, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    lwiperf_start_tcp_server(&perf_server_ip, 9527, NULL, NULL);

    obj_handler_t sem_hd;

    if (blk_drv_cli_map(net_drv_hd, &sem_hd) < 0)
    {
        printf("net drv sem map error.\n");
        return -1;
    }

    obj_handler_t shm_hd = handler_alloc();
    assert(shm_hd != HANDLER_INVALID);
    tag = facotry_create_share_mem(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, shm_hd),
                                   SHARE_MEM_CNT_BUDDY_CNT, 2048);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = share_mem_map(shm_hd, vma_addr_create(VPAGE_PROT_RW, VMA_ADDR_RESV, 0), &addr, &size);
    assert(msg_tag_get_prot(tag) >= 0);

    while (1)
    {
        if (msg_tag_get_prot(u_sema_down(sem_hd)) < 0)
        {
            printf("error.\n");
        }
        int ret = blk_drv_cli_read(net_drv_hd, shm_hd, 0, 0);

        if (ret > 0)
        {
            lwip_pkt_handle_raw((uint8_t *)addr, ret);
        }
    }
    return 0;
}
