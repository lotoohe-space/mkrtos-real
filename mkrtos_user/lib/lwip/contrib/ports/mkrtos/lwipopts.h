/**
 ******************************************************************************
 * @file    lwipopts.h
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    31-July-2013
 * @brief   lwIP Options Configuration.
 *          This file is based on Utilities\lwip_v1.4.1\src\include\lwip\opt.h
 *          and contains the lwIP configuration for the STM32F4x7 demonstration.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */


///************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#define SYS_LIGHTWEIGHT_PROT 1    // 为1时使用实时操作系统的轻量级保护,保护关键代码不被中断打断
#define NO_SYS 0                  // 使用UCOS操作系统
#define MEM_ALIGNMENT 4           // 使用4字节对齐模式
#define MEM_SIZE (8 * 1024)       // 内存堆heap大小
#define MEMP_NUM_PBUF 16          // MEMP_NUM_PBUF:memp结构的pbuf数量,如果应用从ROM或者静态存储区发送大量数据时,这个值应该设置大一点
#define MEMP_NUM_UDP_PCB 4        // MEMP_NUM_UDP_PCB:UDP协议控制块(PCB)数量.每个活动的UDP"连接"需要一个PCB.
#define MEMP_NUM_TCP_PCB 4        // MEMP_NUM_TCP_PCB:同时建立激活的TCP数量
#define MEMP_NUM_TCP_PCB_LISTEN 4 // MEMP_NUM_TCP_PCB_LISTEN:能够监听的TCP连接数量
#define MEMP_NUM_TCP_SEG 64       // MEMP_NUM_TCP_SEG:最多同时在队列中的TCP段数量
#define MEMP_NUM_SYS_TIMEOUT 4    // MEMP_NUM_SYS_TIMEOUT:能够同时激活的timeout个数

// pbuf选项
#define PBUF_POOL_SIZE 32     // PBUF_POOL_SIZE:pbuf内存池个数
#define PBUF_POOL_BUFSIZE 1600 // PBUF_POOL_BUFSIZE:每个pbuf内存池大小

#define LWIP_TCP 1  // 使用TCP
#define TCP_TTL 255 // 生存时间

#undef TCP_QUEUE_OOSEQ
#define TCP_QUEUE_OOSEQ 0 // 当TCP的数据段超出队列时的控制位,当设备的内存过小的时候此项应为0

#undef TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE 32 // tcpip创建主线程时的消息邮箱大小

#undef DEFAULT_TCP_RECVMBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE 32

#undef DEFAULT_ACCEPTMBOX_SIZE
#define DEFAULT_ACCEPTMBOX_SIZE 32

#define TCP_MSS (1500 - 40)                          // 最大TCP分段,TCP_MSS = (MTU - IP报头大小 - TCP报头大小
#define TCP_SND_BUF (8 * TCP_MSS)                    // TCP发送缓冲区大小(bytes).
#define TCP_SND_QUEUELEN (4 * TCP_SND_BUF / TCP_MSS) // TCP_SND_QUEUELEN: TCP发送缓冲区大小(pbuf).这个值最小为(2 * TCP_SND_BUF/TCP_MSS)
#define TCP_WND (2 * TCP_MSS)                        // TCP发送窗口
#define LWIP_ICMP 1                                  // 使用ICMP协议
#define LWIP_DHCP 0                                  // 使用DHCP
#define LWIP_DNS 1
#define LWIP_UDP 1  // 使用UDP服务
#define UDP_TTL 255 // UDP数据包生存时间
#define LWIP_STATS 0
#define LWIP_PROVIDE_ERRNO 1
#define LWIP_CALLBACK_API 1

#define LWIP_NETCONN 1 // LWIP_NETCONN==1:使能NETCON函数(要求使用api_lib.c)
#define LWIP_SOCKET 1  // LWIP_SOCKET==1:使能Sicket API(要求使用sockets.c)
#define LWIP_COMPAT_MUTEX 0
#define LWIP_SO_RCVTIMEO 1 // 通过定义LWIP_SO_RCVTIMEO使能netconn结构体中recv_timeout,使用recv_timeout可以避免阻塞线程

// 有关系统的选项
#define TCPIP_THREAD_PRIO 3        // 定义内核任务的优先级为5
#define TCPIP_THREAD_STACKSIZE 512 // 内核任务堆栈大小
#define DEFAULT_UDP_RECVMBOX_SIZE 512
#define DEFAULT_THREAD_STACKSIZE 512

// LWIP调试选项
#define LWIP_DEBUG 0            // 关闭DEBUG选项
#define ICMP_DEBUG LWIP_DBG_OFF // 开启/关闭ICMPdebug

#define LWIP_COMPAT_MUTEX_ALLOWED
// #define LWIP_SKIP_CONST_CHECK

#define ETHARP_TRUST_IP_MAC 1
#define IP_REASSEMBLY 1
#define IP_FRAG 1
#define ARP_QUEUEING 1
#define LWIP_DNS_SECURE 0

#define LWIP_COMPAT_SOCKETS 1
#define LWIP_POSIX_SOCKETS_IO_NAMES 1

#define LWIP_TCPIP_CORE_LOCKING_INPUT 1
#endif /* __LWIPOPTS_H__ */
