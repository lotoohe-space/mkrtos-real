/**
 ******************************************************************************
 * @file    netconf.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    31-October-2011
 * @brief   Network connection configuration
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; Portions COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */
/**
 ******************************************************************************
 * <h2><center>&copy; Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.</center></h2>
 * @file    netconf.c
 * @author  CMP Team
 * @version V1.0.0
 * @date    28-December-2012
 * @brief   Network connection configuration
 *          Modified to support the STM32F4DISCOVERY, STM32F4DIS-BB and
 *          STM32F4DIS-LCD modules.
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, Embest SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 * OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "lwip/netif.h"
#include "stm32f4x7_eth.h"
#include "stm32f4x7_eth_bsp.h"
#include <stdio.h>
#include "cons_cli.h"
#include "ethernetif.h"
/* Private typedef -----------------------------------------------------------*/
#define MAX_DHCP_TRIES 4
struct netif gnetif;
uint32_t IPaddress = 0;

#ifdef USE_DHCP
uint32_t DHCPfineTimer = 0;
uint32_t DHCPcoarseTimer = 0;
DHCP_State_TypeDef DHCP_state = DHCP_START;
#endif
extern __IO uint32_t  EthStatus;
/**
 * @brief  Initializes the lwIP stack
 * @param  None
 * @retval None
 */
void net_init(void)
{
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;

#ifdef USE_DHCP
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
#else
    // uint8_t iptab[4];
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    // printf("ip:%d.%d.%d.%d", IP_ADDR3, IP_ADDR2, IP_ADDR1, IP_ADDR0);
#endif
    tcpip_init(NULL, NULL);
    cons_write_str("tcpip init success.\n");
    /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
              struct ip_addr *netmask, struct ip_addr *gw,
              void *state, err_t (* init)(struct netif *netif),
              err_t (* input)(struct pbuf *p, struct netif *netif))

     Adds your network interface to the netif_list. Allocate a struct
    netif and pass a pointer to this structure as the first argument.
    Give pointers to cleared ip_addr structures when using DHCP,
    or fill them with sane numbers otherwise. The state pointer may be NULL.

    The init function pointer must point to a initialization function for
    your ethernet netif interface. The following code illustrates it's use.*/
    /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
    struct ip_addr *netmask, struct ip_addr *gw,
    void *state, err_t (* init)(struct netif *netif),
    err_t (* input)(struct pbuf *p, struct netif *netif))

    Adds your network interface to the netif_list. Allocate a struct
    netif and pass a pointer to this structure as the first argument.
    Give pointers to cleared ip_addr structures when using DHCP,
    or fill them with sane numbers otherwise. The state pointer may be NULL.

    The init function pointer must point to a initialization function for
    your ethernet netif interface. The following code illustrates it's use.*/
    sys_lock_tcpip_core();
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    /*  Registers the default network interface.*/
    netif_set_default(&gnetif);

    if (EthStatus == (ETH_INIT_FLAG | ETH_LINK_FLAG))
    {
        /* Set Ethernet link flag */
        gnetif.flags |= NETIF_FLAG_LINK_UP;

        /* When the netif is fully configured this function must be called */
        netif_set_up(&gnetif);

#ifdef USE_DHCP
        DHCP_state = DHCP_START;
#else

#endif /* USE_DHCP */
    }
    else
    {
        /* When the netif link is down this function must be called */
        netif_set_down(&gnetif);

#ifdef USE_DHCP
        DHCP_state = DHCP_LINK_DOWN;
#endif  /* USE_DHCP */
        /* Set the LCD Text Color */
    }

    /* Set the link callback function, this function is called on change of link status*/
    netif_set_link_callback(&gnetif, ETH_link_callback);
    sys_unlock_tcpip_core();
}
void ethernetif_input(void *pvParameters);
/**
 * @brief  Called when a frame is received
 * @param  None
 * @retval None
 */
void LwIP_Pkt_Handle(void)
{
    /* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
    ethernetif_input(&gnetif);
}
