#include "soft_iic.h"
#include "delay.h"

void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    IIC_SCL = 1;
    IIC_SDA = 1;
}
void IIC_Start(void)
{
    SDA_OUT();
    IIC_SDA = 1;
    IIC_SCL = 1;
    delay_us(4);
    IIC_SDA = 0;
    delay_us(4);
    IIC_SCL = 0;
}
void IIC_Stop(void)
{
    SDA_OUT();
    IIC_SCL = 0;
    IIC_SDA = 0;
    delay_us(4);
    IIC_SCL = 1;
    IIC_SDA = 1;
    delay_us(4);
}

u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    SDA_IN();
    IIC_SDA = 1;
    delay_us(1);
    IIC_SCL = 1;
    delay_us(1);
    while (READ_SDA)
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL = 0;
    return 0;
}
void IIC_Ack(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}
void IIC_NAck(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 1;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}

void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL = 0;
    for (t = 0; t < 8; t++)
    {
        IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        delay_us(2);
        IIC_SCL = 1;
        delay_us(2);
        IIC_SCL = 0;
        delay_us(2);
    }
}
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN();
    for (i = 0; i < 8; i++)
    {
        IIC_SCL = 0;
        delay_us(2);
        IIC_SCL = 1;
        receive <<= 1;
        if (READ_SDA)
            receive++;
        delay_us(1);
    }
    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();
    return receive;
}
u8 IIC_Write_1Byte(u8 SlaveAddress, u8 REG_Address, u8 REG_data)
{
    IIC_Start();
    IIC_Send_Byte(SlaveAddress << 1);
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(REG_Address);
    IIC_Wait_Ack();
    IIC_Send_Byte(REG_data);
    IIC_Wait_Ack();
    IIC_Stop();
    return 0;
}

u8 IIC_Read_1Byte(u8 SlaveAddress, u8 REG_Address, u8 *REG_data)
{
    IIC_Start();
    IIC_Send_Byte(SlaveAddress << 1);
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(REG_Address);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(SlaveAddress << 1 | 0x01);
    IIC_Wait_Ack();
    *REG_data = IIC_Read_Byte(0);
    IIC_Stop();
    return 0;
}
