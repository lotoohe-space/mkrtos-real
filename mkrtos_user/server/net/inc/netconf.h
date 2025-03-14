/**
 * @file netconf.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-01-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#define DEST_IP_ADDR0 192
#define DEST_IP_ADDR1 168
#define DEST_IP_ADDR2 2
#define DEST_IP_ADDR3 20

#define DEST_PORT 77

/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 3
#define IP_ADDR3 10

/*NETMASK*/
#define NETMASK_ADDR0 255
#define NETMASK_ADDR1 255
#define NETMASK_ADDR2 255
#define NETMASK_ADDR3 0

/*Gateway Address*/
#define GW_ADDR0 192
#define GW_ADDR1 168
#define GW_ADDR2 3
#define GW_ADDR3 1

void net_hw_init(void);
void LwIP_Pkt_Handle(void);
void lwip_pkt_handle_raw(uint8_t *data, int len);
