#pragma once

void MySPI_Init(void);
void MySPI_SetSpeed(uint8_t SPI_BaudRatePrescaler);
uint8_t MySPI_SwapByte(uint8_t ByteSend);
