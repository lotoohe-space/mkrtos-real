#pragma once
#include "stm32_sys.h"

#define SPI_CS_L() GPIO_ResetBits(GPIOB, GPIO_Pin_3)
#define SPI_CS_H() GPIO_SetBits(GPIOB, GPIO_Pin_3)

void SPI2_Init(void);             // 初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet);  // 设置SPI速度
u8 SPI2_ReadWriteByte(u8 TxData); // SPI总线读写一个字节
