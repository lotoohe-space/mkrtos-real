#include "wk2xx.h"
#include "delay.h"
#include "spi2.h"

/***************************WkWriteGReg***********************************/
// 函数功能：写全局寄存器函数（前提是该寄存器可写，
// 某些寄存器如果你写1，可能会自动置1，具体见数据手册)
// 参数：
//       greg:为全局寄存器的地址
//       dat:为写入寄存器的数据
//***********************************************************************/
void WkWriteGReg(unsigned char greg, unsigned char dat)
{
    u8 cmd;
    cmd = 0 | greg;
    SPI_CS_L();              // 拉低cs信号
    SPI2_ReadWriteByte(cmd); // 写指令，对于指令的构成见数据手册
    SPI2_ReadWriteByte(dat); // 写数据
    SPI_CS_H();              // 拉高cs信号
}
/****************************WkReadGReg***********************************/
// 函数功能：读全局寄存器
// 参数：
//       greg:为全局寄存器的地址
//       rec:返回的寄存器值
//***********************************************************************/
u8 WkReadGReg(unsigned char greg)
{
    u8 cmd, rec;
    cmd = 0x40 | greg;
    SPI_CS_L();                  // 拉低cs信号
    SPI2_ReadWriteByte(cmd);     // 写指令，对于指令的构成见数据手册
    rec = SPI2_ReadWriteByte(0); // 写数据
    SPI_CS_H();                  // 拉高cs信号
    return rec;
}

/**************************WkWriteSReg***********************************/
// 函数功能:
// 参数：port:为子串口
//       sreg:为子串口寄存器
//       dat:为写入寄存器的数据
// 注意：在子串口被打通的情况下，向FDAT写入的数据会通过TX引脚输出
//**********************************************************************/
void WkWriteSReg(u8 port, u8 sreg, u8 dat)
{
    u8 cmd;
    cmd = 0x0 | ((port - 1) << 4) | sreg;
    SPI_CS_L();              // 拉低cs信号
    SPI2_ReadWriteByte(cmd); // 写指令，对于指令的构成见数据手册
    SPI2_ReadWriteByte(dat); // 写数据
    SPI_CS_H();              // 拉高cs信号
}

/**************************WkReadSReg***********************************/
// 函数功能：读子串口寄存器
// 参数：port为子串口端口号
//       sreg:为子串口寄存器地址
//       rec:返回的寄存器值
//**********************************************************************/
u8 WkReadSReg(u8 port, u8 sreg)
{
    u8 cmd, rec;
    cmd = 0x40 | ((port - 1) << 4) | sreg;
    SPI_CS_L();                  // 拉低cs信号
    SPI2_ReadWriteByte(cmd);     // 写指令，对于指令的构成见数据手册
    rec = SPI2_ReadWriteByte(0); // 写数据
    SPI_CS_H();                  // 拉高cs信号
    return rec;
}
/************************WkWriteSFifo***********************************/
// 函数功能:向子串口fifo写入需要发送的数据
// 参数：port:为子串口
//       *dat:写入数据
//       num：为写入数据的个数，单次不超过256
// 注意：通过该方式写入的数据，被直接写入子串口的缓存FIFO，然后被发送
//*********************************************************************/
void WkWriteSFifo(u8 port, u8 *dat, int num)
{
    u8 cmd;
    int i;
    cmd = 0x80 | ((port - 1) << 4);
    if (num > 0)
    {
        SPI_CS_L();              // 拉低cs信号
        SPI2_ReadWriteByte(cmd); // 写指令,对于指令构成见数据手册
        for (i = 0; i < num; i++)
        {
            SPI2_ReadWriteByte(*(dat + i)); // 写数据
        }
        SPI_CS_H(); // 拉高cs信号
    }
}

/************************WkReadSFifo***********************************/
// 函数功能:从子串口的fifo中读出接收到的数据
// 参数：port:为子串口
//       *rec：接收到的数据
//       num：读出的数据个数。
// 注意：通过该方式读出子串口缓存中的数据。单次不能超过256
//*********************************************************************/
void WkReadSFifo(u8 port, u8 *rec, int num)
{
    u8 cmd;
    int n;
    cmd = 0xc0 | ((port - 1) << 4);
    if (num > 0)
    {
        SPI_CS_L(); // 拉低cs信号
        SPI2_ReadWriteByte(cmd);
        for (n = 0; n < num; n++)
        {
            *(rec + n) = SPI2_ReadWriteByte(0);
        }
        SPI_CS_H(); // 拉高cs信号
    }
}

void WK_ISR(void)
{
}

/*******WkInit*******************************************/
// 函数功能：初始化子串口
/*******************************************************/
void Wk_Init(u8 port)
{
    u8 gena, grst, gier, sier, scr;
    // 使能子串口时钟
    gena = WkReadGReg(WK2XXX_GENA);
    gena = gena | (1 << (port - 1));
    WkWriteGReg(WK2XXX_GENA, gena);
    // 软件复位子串口
    grst = WkReadGReg(WK2XXX_GRST);
    grst = grst | (1 << (port - 1));
    WkWriteGReg(WK2XXX_GRST, grst);
    // 使能串口总中断
    gier = WkReadGReg(WK2XXX_GIER);
    gier = gier | (1 << (port - 1));
    WkWriteGReg(WK2XXX_GIER, gier);
    // 使能子串口接收触点中断和超时中断
    sier = WkReadSReg(port, WK2XXX_SIER);
    sier |= WK2XXX_RFTRIG_IEN | WK2XXX_RXOUT_IEN;
    WkWriteSReg(port, WK2XXX_SIER, sier);
    // 初始化FIFO和设置固定中断触点
    WkWriteSReg(port, WK2XXX_FCR, 0XFF);
    // 设置任意中断触点，如果下面的设置有效，
    // 那么上面FCR寄存器中断的固定中断触点将失效
    WkWriteSReg(port, WK2XXX_SPAGE, 1);  // 切换到page1
    WkWriteSReg(port, WK2XXX_RFTL, 255); // 设置接收触点为256个字节
    WkWriteSReg(port, WK2XXX_TFTL, 255); // 设置发送触点为256个字节
    WkWriteSReg(port, WK2XXX_SPAGE, 0);  // 切换到page0
    // 使能子串口的发送和接收使能
    scr = WkReadSReg(port, WK2XXX_SCR);
    scr |= WK2XXX_TXEN | WK2XXX_RXEN;
    WkWriteSReg(port, WK2XXX_SCR, scr);
}

/******************************Wk_DeInit*******************************************/
// 函数功能：初始化子串口
/*********************************************************************************/
void Wk_DeInit(u8 port)
{
    u8 gena, grst, gier;
    // 关闭子串口总时钟
    gena = WkReadGReg(WK2XXX_GENA);
    gena = gena & (~(1 << (port - 1)));
    WkWriteGReg(WK2XXX_GENA, gena);
    // 使能子串口总中断
    gier = WkReadGReg(WK2XXX_GIER);
    gier = gier & (~(1 << (port - 1)));
    WkWriteGReg(WK2XXX_GIER, gier);
    // 软件复位子串口
    grst = WkReadGReg(WK2XXX_GRST);
    grst = grst | (1 << (port - 1));
    WkWriteGReg(WK2XXX_GRST, grst);
}

/**************************Wk_SetBaud*******************************************************/
// 函数功能：设置子串口波特率函数、此函数中波特率的匹配值是根据11.0592Mhz下的外部晶振计算的
//  port:子串口号
//  baud:波特率大小.波特率表示方式，
/**************************Wk2114SetBaud*******************************************************/
void Wk_SetBaud(u8 port, enum WKBaud baud)
{
    unsigned char baud1, baud0, pres, scr;
    // 如下波特率相应的寄存器值，是在外部时钟为11.0592mhz的情况下计算所得，如果使用其他晶振，需要重新计算
    switch (baud)
    {
    case B600:
        baud1 = 0x4;
        baud0 = 0x7f;
        pres = 0;
        break;
    case B1200:
        baud1 = 0x2;
        baud0 = 0x3F;
        pres = 0;
        break;
    case B2400:
        baud1 = 0x1;
        baud0 = 0x1f;
        pres = 0;
        break;
    case B4800:
        baud1 = 0x00;
        baud0 = 0x8f;
        pres = 0;
        break;
    case B9600:
        baud1 = 0x00;
        baud0 = 0x47;
        pres = 0;
        break;
    case B19200:
        baud1 = 0x00;
        baud0 = 0x23;
        pres = 0;
        break;
    case B38400:
        baud1 = 0x00;
        baud0 = 0x11;
        pres = 0;
        break;
    case B76800:
        baud1 = 0x00;
        baud0 = 0x08;
        pres = 0;
        break;
    case B1800:
        baud1 = 0x01;
        baud0 = 0x7f;
        pres = 0;
        break;
    case B3600:
        baud1 = 0x00;
        baud0 = 0xbf;
        pres = 0;
        break;
    case B7200:
        baud1 = 0x00;
        baud0 = 0x5f;
        pres = 0;
        break;
    case B14400:
        baud1 = 0x00;
        baud0 = 0x2f;
        pres = 0;
        break;
    case B28800:
        baud1 = 0x00;
        baud0 = 0x17;
        pres = 0;
        break;
    case B57600:
        baud1 = 0x00;
        baud0 = 0x0b;
        pres = 0;
        break;
    case B115200:
        baud1 = 0x00;
        baud0 = 0x05;
        pres = 0;
        break;
    case B230400:
        baud1 = 0x00;
        baud0 = 0x02;
        pres = 0;
        break;
    default:
        baud1 = 0x00;
        baud0 = 0x00;
        pres = 0;
    }
    // 关掉子串口收发使能
    scr = WkReadSReg(port, WK2XXX_SCR);
    WkWriteSReg(port, WK2XXX_SCR, 0);
    // 设置波特率相关寄存器
    WkWriteSReg(port, WK2XXX_SPAGE, 1); // 切换到page1
    WkWriteSReg(port, WK2XXX_BAUD1, baud1);
    WkWriteSReg(port, WK2XXX_BAUD0, baud0);
    WkWriteSReg(port, WK2XXX_PRES, pres);
    WkWriteSReg(port, WK2XXX_SPAGE, 0); // 切换到page0
    // 使能子串口收发使能
    WkWriteSReg(port, WK2XXX_SCR, scr);
}
/**************************WK_TxLen*******************************************/
// 函数功能:获取子串口发送FIFO剩余空间长度
//  port:端口号
//  返回值：发送FIFO剩余空间长度
/**************************WK_Len********************************************/
int wk_TxLen(u8 port)
{
    u8 fsr, tfcnt;
    int len = 0;
    fsr = WkReadSReg(port, WK2XXX_FSR);
    tfcnt = WkReadSReg(port, WK2XXX_TFCNT);
    if (fsr & WK2XXX_TFULL)
    {
        len = 0;
    }
    else
    {
        len = 256 - tfcnt;
    }
    return len;
}

/**************************WK_TxChars*******************************************/
// 函数功能:通过子串口发送固定长度数据
//  port:端口号
//  len:单次发送长度不超过256
//
/**************************WK_TxChars********************************************/
int wk_TxChars(u8 port, int len, u8 *sendbuf)
{

#if 1
    WkWriteSFifo(port, sendbuf, len); // 通过fifo方式发送数据
#else
    int num = len;
    for (num = 0; num < len; num++)
    {
        WkWriteSReg(port, WK2XXX_FDAT, *(sendbuf + num));
    }
#endif
    return 0;
}

/**************************WK_RxChars*******************************************/
// 函数功能:读取子串口fifo中的数据
//  port:端口号
//  recbuf:接收到的数据
//  返回值：接收数据的长度
/**************************WK_RxChars********************************************/
int wk_RxChars(u8 port, u8 *recbuf)
{
    u8 fsr = 0, rfcnt = 0, rfcnt2 = 0, sifr = 0;
    int len = 0;
    sifr = WkReadSReg(port, WK2XXX_SIFR);

    if ((sifr & WK2XXX_RFTRIG_INT) || (sifr & WK2XXX_RXOVT_INT)) // 有接收中断和接收超时中断
    {
        fsr = WkReadSReg(port, WK2XXX_FSR);
        rfcnt = WkReadSReg(port, WK2XXX_RFCNT);
        rfcnt2 = WkReadSReg(port, WK2XXX_RFCNT);
        // printf("rfcnt=0x%x.\n",rfcnt);
        /*判断fifo中数据个数*/
        if (fsr & WK2XXX_RDAT)
        {
            if (!(rfcnt2 >= rfcnt))
            {
                rfcnt = rfcnt2;
            }
            len = (rfcnt == 0) ? 256 : rfcnt;
        }
#if 1
        WkReadSFifo(port, recbuf, len);
#else
        for (n = 0; n < len; n++)
            *(recbuf + n) = WkReadSReg(port, WK2XXX_FDAT);
#endif
        return len;
    }
    else
    {
        len = 0;
        return len;
    }
}

void wk_wait_tx_done(int inx)
{
    // 等待发送完成
    while (1)
    {
        u8 fsr_reg = WkReadSReg(inx, WK2XXX_FSR);
        if ((fsr_reg & WK2XXX_TDAT) == 0)
        {
            break;
        }
    }
    delay_ms(2);
}
