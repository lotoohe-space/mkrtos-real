
#include "cons_cli.h"
#include "netconf.h"
#include "u_drv.h"
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "u_sleep.h"
#include "libc.h"
#include "dm9000.h"
#include "lwiperf.h"
extern void EXTI15_10_IRQHandler(void);
int main(int args, char *argv[])
{
    struct __libc *lc = &libc;

    printf("libc:0x%x\n", lc);
    assert(u_drv_init() >= 0);
    cons_write_str("net init..\n");
    if (DM9000_Init(1))
    {
        cons_write_str("err.\n");
        return 2; // 初始化DM9000AEP
    }
    net_init();
    cons_write_str("net start success..\n");
    ip_addr_t perf_server_ip;

    IP_ADDR4(&perf_server_ip, 192, 168, 3, 10);
    lwiperf_start_tcp_server(&perf_server_ip, 9527, NULL, NULL);
    while (1)
    {
        EXTI15_10_IRQHandler();
    }
    return 0;
}
