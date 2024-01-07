
#include "cons_cli.h"
#include "netconf.h"
#include "u_drv.h"
#include "stm32f4x7_eth.h"
#include "stm32f4x7_eth_bsp.h"
#include <unistd.h>
#include <stdio.h>
#include "libc.h"
int main(int args, char *argv[])
{
    struct __libc *lc = &libc;

    printf("libc:0x%x\n", lc);
    u_drv_init();
    cons_write_str("net init..\n");
    ETH_BSP_Config();
    net_init();
    cons_write_str("net start success..\n");
    while (1)
    {
        // if (ETH_CheckFrameReceived())
        // {
            /* process received ethernet packet */
            LwIP_Pkt_Handle();
        // }
    }
    return 0;
}
