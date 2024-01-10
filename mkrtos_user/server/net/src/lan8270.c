#include "lan8720.h"
#include "stm32f4x7_eth.h"
#include <stm32f4xx.h>
#include "u_sleep.h"
#include <malloc.h>

ETH_DMADESCTypeDef *DMARxDscrTab;
ETH_DMADESCTypeDef *DMATxDscrTab;
uint8_t *Rx_Buff;
uint8_t *Tx_Buff;

static void ETHERNET_NVICConfiguration(void);

u8 LAN8720_Init(void)
{
    u8 rval = 0;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
    /*RMII
      ETH_MDIO -------------------------> PA2
      ETH_MDC --------------------------> PC1
      ETH_RMII_REF_CLK------------------> PA1
      ETH_RMII_CRS_DV ------------------> PA7
      ETH_RMII_RXD0 --------------------> PC4
      ETH_RMII_RXD1 --------------------> PC5
      ETH_RMII_TX_EN -------------------> PG11
      ETH_RMII_TXD0 --------------------> PG13
      ETH_RMII_TXD1 --------------------> PG14
      ETH_RESET-------------------------> PD3
      */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);

    // RST
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    LAN8720_RST = 0;
    delay_ms(50);
    LAN8720_RST = 1;
    ETHERNET_NVICConfiguration();
    rval = ETH_MACDMA_Config();
    return !rval;
}
#include "u_intr.h"
#define IRQ_THREAD_PRIO 2
#define STACK_SIZE (1024)
static obj_handler_t irq_obj;
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];

void ETH_IRQHandler(void);

void ETHERNET_NVICConfiguration(void)
{
    // NVIC_InitTypeDef NVIC_InitStructure;

    // NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0X06;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0X00;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);

    assert(u_intr_bind(ETH_IRQn, (u_irq_prio_t){.prio_p = 0X06, .prio_s = 0}, IRQ_THREAD_PRIO,
                       stack0 + STACK_SIZE, NULL, ETH_IRQHandler, &irq_obj) >= 0);
}

u8 LAN8720_Get_Speed(void)
{
    u8 speed;
    speed = ((ETH_ReadPHYRegister(0x00, 31) & 0x1C) >> 2);
    return speed;
}

u8 ETH_MACDMA_Config(void)
{
    u8 rval;
    ETH_InitTypeDef ETH_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx | RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);

    ETH_DeInit();
    ETH_SoftwareReset();
    while (ETH_GetSoftwareResetStatus() == SET)
        ;
    ETH_StructInit(&ETH_InitStructure);

    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
    ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
    ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
    ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
    ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
    ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
    ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
    rval = ETH_Init(&ETH_InitStructure, LAN8720_PHY_ADDRESS);
    if (rval == ETH_SUCCESS)
    {
        ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
    }
    return rval;
}

extern void lwip_pkt_handle(void);
void ETH_IRQHandler(void)
{
    while (ETH_GetRxPktSize(DMARxDescToGet) != 0)
    {
        lwip_pkt_handle();
    }
    ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
    ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
}

FrameTypeDef ETH_Rx_Packet(void)
{
    u32 framelength = 0;
    FrameTypeDef frame = {0, 0};
    if ((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (u32)RESET)
    {
        frame.length = ETH_ERROR;
        if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)
        {
            ETH->DMASR = ETH_DMASR_RBUS;
            ETH->DMARPDR = 0;
        }
        return frame;
    }
    if (((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (u32)RESET) &&
        ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (u32)RESET) &&
        ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (u32)RESET))
    {
        framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4;
        frame.buffer = DMARxDescToGet->Buffer1Addr;
    }
    else
        framelength = ETH_ERROR;
    frame.length = framelength;
    frame.descriptor = DMARxDescToGet;
    DMARxDescToGet = (ETH_DMADESCTypeDef *)(DMARxDescToGet->Buffer2NextDescAddr);
    return frame;
}
u8 ETH_Tx_Packet(u16 FrameLength)
{
    if ((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
        return ETH_ERROR;
    DMATxDescToSet->ControlBufferSize = (FrameLength & ETH_DMATxDesc_TBS1);
    DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;
    DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;
    if ((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET)
    {
        ETH->DMASR = ETH_DMASR_TBUS;
        ETH->DMATPDR = 0;
    }
    DMATxDescToSet = (ETH_DMADESCTypeDef *)(DMATxDescToSet->Buffer2NextDescAddr);
    return ETH_SUCCESS;
}
u32 ETH_GetCurrentTxBuffer(void)
{
    return DMATxDescToSet->Buffer1Addr;
}

u8 ETH_Mem_Malloc(void)
{
    DMARxDscrTab = malloc(ETH_RXBUFNB * sizeof(ETH_DMADESCTypeDef));
    DMATxDscrTab = malloc ETH_TXBUFNB * sizeof(ETH_DMADESCTypeDef));
    Rx_Buff = malloc(ETH_RX_BUF_SIZE * ETH_RXBUFNB);
    Tx_Buff = malloc(ETH_TX_BUF_SIZE * ETH_TXBUFNB);
    if (!DMARxDscrTab || !DMATxDscrTab || !Rx_Buff || !Tx_Buff)
    {
        ETH_Mem_Free();
        return 1;
    }
    return 0;
}

void ETH_Mem_Free(void)
{
    free(DMARxDscrTab);
    free(DMATxDscrTab);
    free(Rx_Buff);
    free(Tx_Buff);
}
