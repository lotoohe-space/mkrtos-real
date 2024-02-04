
#include "cons_cli.h"
#include "netconf.h"
#include "u_drv.h"
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "u_sleep.h"
#include "libc.h"
#include "lwiperf.h"
#include "u_prot.h"
#include "u_mm.h"
#include <u_hd_man.h>
#include <u_task.h>
#include <u_factory.h>
#include <u_share_mem.h>
#include <net_drv_cli.h>
#include <ns_cli.h>
umword_t addr;
umword_t size;
obj_handler_t net_drv_hd;

extern void EXTI15_10_IRQHandler(void);
int main(int args, char *argv[])
{
    int ret;
    msg_tag_t tag;

    ret = ns_query("/dm9000", &net_drv_hd);
    assert(ret >= 0);

    cons_write_str("net init..\n");
    net_init();
    cons_write_str("net start success..\n");
    ip_addr_t perf_server_ip;

    IP_ADDR4(&perf_server_ip, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    lwiperf_start_tcp_server(&perf_server_ip, 9527, NULL, NULL);

    obj_handler_t shm_hd = handler_alloc();
    assert(shm_hd != HANDLER_INVALID);
    tag = facotry_create_share_mem(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, shm_hd), 2048);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = share_mem_map(shm_hd, 3, &addr, &size);
    assert(msg_tag_get_prot(tag) >= 0);

    while (1)
    {
        int ret = net_drv_cli_read(net_drv_hd, shm_hd);

        if (ret > 0)
        {
            lwip_pkt_handle_raw((uint8_t *)addr, ret);
        }
    }
    return 0;
}
