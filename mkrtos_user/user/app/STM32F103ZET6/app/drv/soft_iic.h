#pragma once

#include "stm32_sys.h"

#define SDA_IN()                    \
    {                               \
        GPIOB->CRL &= 0XFF0FFFFF; \
        GPIOB->CRL |= 8 << 20;    \
    }
#define SDA_OUT()                   \
    {                               \
        GPIOB->CRL &= 0XFF0FFFFF; \
        GPIOB->CRL |= 3 << 20;    \
    }

#define IIC_SCL PBout(4) // SCL
#define IIC_SDA PBout(5) // SDA
#define READ_SDA PBin(5)

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(u8 txd);
u8 IIC_Read_Byte(unsigned char ack);
u8 IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);

void IIC_Write_One_Byte(u8 daddr, u8 addr, u8 data);
u8 IIC_Read_One_Byte(u8 daddr, u8 addr);

u8 IIC_Write_1Byte(u8 SlaveAddress, u8 REG_Address, u8 REG_data);
u8 IIC_Read_1Byte(u8 SlaveAddress, u8 REG_Address, u8 *REG_data);
