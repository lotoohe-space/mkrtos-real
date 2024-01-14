
#include "cons_cli.h"
#include "netconf.h"
#include "u_drv.h"
#include "stm32f4x7_eth.h"
#include <unistd.h>
#include <stdio.h>
#include "libc.h"
#include "lan8270.h"
int main(int args, char *argv[])
{
    struct __libc *lc = &libc;

    printf("libc:0x%x\n", lc);
    u_drv_init();
    cons_write_str("net init..\n");
    ETH_Mem_Malloc();
    LAN8720_Init();
    net_init();
    cons_write_str("net start success..\n");
    while (1)
    {
        // if (ETH_CheckFrameReceived())
        // {
        /* process received ethernet packet */
        // LwIP_Pkt_Handle();
        // }
        sleep(1);
    }
    return 0;
}
