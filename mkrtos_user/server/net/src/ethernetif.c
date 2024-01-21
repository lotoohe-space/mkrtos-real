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
#include <net_drv_cli.h>
#include <u_hd_man.h>
#include <u_factory.h>
#include <u_share_mem.h>
#include <u_task.h>
#include <ns_cli.h>
#include <assert.h>
static obj_handler_t net_drv_hd;
extern sem_t dm9000input;		   // DM9000接收数据信号量
extern pthread_mutex_t dm9000lock; // DM9000读写互锁控制信号量
obj_handler_t send_shm_hd;

umword_t send_shm_addr;
umword_t send_shm_size;

#define IFNAME0 'e'
#define IFNAME1 'n'
// 由ethernetif_init()调用用于初始化硬件
// netif:网卡结构体指针
// 返回值:ERR_OK,正常
//        其他,失败
static err_t low_level_init(struct netif *netif)
{
	umword_t tmp = *(vu32 *)(0x1FFFF7E8);
	// INT8U err;
	netif->hwaddr_len = ETHARP_HWADDR_LEN; // 设置MAC地址长度,为6个字节
	// 初始化MAC地址,设置什么地址由用户自己设置,但是不能与网络中其他设备MAC地址重复
	netif->hwaddr[0] = 2;
	netif->hwaddr[1] = 0;
	netif->hwaddr[2] = 0;
	netif->hwaddr[3] = (tmp >> 16) & 0XFF; // 低三字节用STM32的唯一ID
	netif->hwaddr[4] = (tmp >> 8) & 0XFF;
	netif->hwaddr[5] = tmp & 0XFF;
	netif->mtu = 1500; // 最大允许传输单元,允许该网卡广播和ARP功能
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

	assert(ns_query("/dm9000", &net_drv_hd) >= 0);

	msg_tag_t tag;
	send_shm_hd = handler_alloc();
	assert(send_shm_hd != HANDLER_INVALID);
	tag = facotry_create_share_mem(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, send_shm_hd), 2048);
	assert(msg_tag_get_prot(tag) >= 0);
	tag = share_mem_map(send_shm_hd, 3, &send_shm_addr, &send_shm_size);
	assert(msg_tag_get_prot(tag) >= 0);

	return ERR_OK;
}
// 用于发送数据包的最底层函数(lwip通过netif->linkoutput指向该函数)
// netif:网卡结构体指针
// p:pbuf数据结构体指针
// 返回值:ERR_OK,发送正常
//        ERR_MEM,发送失败
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	int ret;
	// DM9000_SendPacket(p); // 发送数据
	memcpy((void *)send_shm_addr, p->payload, p->len);
	ret = net_drv_cli_write(net_drv_hd, send_shm_hd, p->len);
	return ret >= 0 ? ERR_OK : ERR_IF;
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
err_t ethernetif_input_raw(struct netif *netif, uint8_t *data, int len)
{
	err_t err = ERR_MEM;
	struct pbuf *p;

	p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL); // pbufs内存池分配pbuf
	if (p != NULL)							  // 内存申请成功
	{
		memcpy(p->payload, data, len);
		err = netif->input(p, netif); // 调用netif结构体中的input字段(一个函数)来处理数据包
		if (err != ERR_OK)
		{
			LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
			pbuf_free(p);
			p = NULL;
		}
	}
	return err;
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
