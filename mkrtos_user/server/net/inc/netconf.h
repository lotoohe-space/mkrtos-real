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
#define MAC_ADDR0 2
#define MAC_ADDR1 4
#define MAC_ADDR2 3
#define MAC_ADDR3 7
#define MAC_ADDR4 8
#define MAC_ADDR5 9
void net_init(void);
void LwIP_Pkt_Handle(void);
