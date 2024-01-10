#ifndef __LAN8720_H
#define __LAN8720_H
#include "sys.h"
#include "stm32f4x7_eth.h"

#define LAN8720_PHY_ADDRESS 0x00
#define LAN8720_RST PDout(3)

extern ETH_DMADESCTypeDef *DMARxDscrTab;
extern ETH_DMADESCTypeDef *DMATxDscrTab;
extern uint8_t *Rx_Buff;
extern uint8_t *Tx_Buff;
extern ETH_DMADESCTypeDef *DMATxDescToSet;
extern ETH_DMADESCTypeDef *DMARxDescToGet;
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;

u8 LAN8720_Init(void);
u8 LAN8720_Get_Speed(void);
u8 ETH_MACDMA_Config(void);
FrameTypeDef ETH_Rx_Packet(void);
u8 ETH_Tx_Packet(u16 FrameLength);
u32 ETH_GetCurrentTxBuffer(void);
u8 ETH_Mem_Malloc(void);
void ETH_Mem_Free(void);
#endif
