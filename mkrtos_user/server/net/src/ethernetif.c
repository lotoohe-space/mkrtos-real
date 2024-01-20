#include "ethernetif.h"
// #include "lwip_comm.h"
// #include "malloc.h"
#include "netif/etharp.h"
#include "string.h"
// #include "includes.h"
#include "lwip/timeouts.h"
#include "netconf.h"
#include <semaphore.h>
#include <pthread.h>
#include <cons_cli.h>
#include "dm9000.h"

extern sem_t dm9000input;				// DM9000接收数据信号量
extern pthread_mutex_t dm9000lock; // DM9000读写互锁控制信号量
/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */

#define IFNAME0 'e'
#define IFNAME1 'n'
// 由ethernetif_init()调用用于初始化硬件
// netif:网卡结构体指针
// 返回值:ERR_OK,正常
//        其他,失败
static err_t low_level_init(struct netif *netif)
{
	// INT8U err;
	netif->hwaddr_len = ETHARP_HWADDR_LEN; // 设置MAC地址长度,为6个字节
	// 初始化MAC地址,设置什么地址由用户自己设置,但是不能与网络中其他设备MAC地址重复
	netif->hwaddr[0] = dm9000cfg.mac_addr[0];
	netif->hwaddr[1] = dm9000cfg.mac_addr[1];
	netif->hwaddr[2] = dm9000cfg.mac_addr[2];
	netif->hwaddr[3] = dm9000cfg.mac_addr[3];
	netif->hwaddr[4] = dm9000cfg.mac_addr[4];
	netif->hwaddr[5] = dm9000cfg.mac_addr[5];
	netif->mtu = 1500; // 最大允许传输单元,允许该网卡广播和ARP功能
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
	return ERR_OK;
}
// 用于发送数据包的最底层函数(lwip通过netif->linkoutput指向该函数)
// netif:网卡结构体指针
// p:pbuf数据结构体指针
// 返回值:ERR_OK,发送正常
//        ERR_MEM,发送失败
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	DM9000_SendPacket(p); // 发送数据
	return ERR_OK;
}
// 用于接收数据包的最底层函数
// neitif:网卡结构体指针
// 返回值:pbuf数据结构体指针
static struct pbuf *low_level_input(struct netif *netif)
{
	struct pbuf *p;
	p = DM9000_Receive_Packet();
	return p;
}
err_t ethernetif_input(struct netif *netif)
{
	uint32_t _err;
	err_t err = 0;
	struct pbuf *p;
	// while (1)
	{
		// _err = sem_wait(&dm9000input); // 请求信号量
		// cons_write_str("*\n");
		if (_err == 0)
		{
			while (1)
			{
				p = low_level_input(netif); // 调用low_level_input函数接收数据
				if (p != NULL)
				{
					err = netif->input(p, netif); // 调用netif结构体中的input字段(一个函数)来处理数据包
					if (err != ERR_OK)
					{
						LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
						pbuf_free(p);
						p = NULL;
					}
				}
				else
					break;
			}
		}
	}
}
// static void arp_timer(void *arg);
// 使用low_level_init()函数来初始化网络
// netif:网卡结构体指针
// 返回值:ERR_OK,正常
//        其他,失败
err_t ethernetif_init(struct netif *netif)
{
	LWIP_ASSERT("netif!=NULL", (netif != NULL));
#if LWIP_NETIF_HOSTNAME		  // LWIP_NETIF_HOSTNAME
	netif->hostname = "lwip"; // 初始化名称
#endif
	netif->name[0] = IFNAME0;			  // 初始化变量netif的name字段
	netif->name[1] = IFNAME1;			  // 在文件外定义这里不用关心具体值
	netif->output = etharp_output;		  // IP层发送数据包函数
	netif->linkoutput = low_level_output; // ARP模块发送数据包函数
	low_level_init(netif);				  // 底层硬件初始化函数

	//  etharp_init();
	// sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
	return ERR_OK;
}
