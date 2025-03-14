//----------------------------头文件依赖----------------------------//
#include "TP_FT5206.h"
#include <stdio.h>
// #include "dev_systick.h"

/* Tips : 本驱动例程为触摸 IC : FT5206 敦泰电子(FocalTech) */

//----------------------------本地宏定义----------------------------//
// I2C读写命令
// 按8位地址的方式定义如下
#define FT_CMD_WR 0X70 // 写命令
#define FT_CMD_RD 0X71 // 读命令

// FT5206 部分寄存器定义
#define FT_DEVIDE_MODE 0x00    // FT5206模式控制寄存器
#define FT_REG_NUM_FINGER 0x02 // 触摸状态寄存器

#define FT_TP1_REG 0X03 // 第一个触摸点数据地址
#define FT_TP2_REG 0X09 // 第二个触摸点数据地址
#define FT_TP3_REG 0X0F // 第三个触摸点数据地址
#define FT_TP4_REG 0X15 // 第四个触摸点数据地址
#define FT_TP5_REG 0X1B // 第五个触摸点数据地址

#define FT_ID_G_LIB_VERSION 0xA1  // 版本
#define FT_ID_G_MODE 0xA4         // FT5206中断模式控制寄存器
#define FT_ID_G_THGROUP 0x80      // 触摸有效值设置寄存器
#define FT_ID_G_PERIODACTIVE 0x88 // 激活状态周期设置寄存器

//----------------------------本地全局变量----------------------------//
static const uint16_t FT5206_TPX_Table[5] = {FT_TP1_REG, FT_TP2_REG, FT_TP3_REG, FT_TP4_REG, FT_TP5_REG};

//----------------------------本地函数定义----------------------------//
static inline void delay(void)
{
    for (uint32_t i = 0; i < 600; i++)
        __NOP();
}

// TP_IC 写入寄存器
static uint32_t touch_write_regs(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint32_t i;
    uint8_t ack;

    ack = I2C_Start(TP_I2C_X, FT_CMD_WR, 1);
    if (ack == 0)
        goto wr_fail;

    ack = I2C_Write(TP_I2C_X, reg & 0XFF, 1);
    if (ack == 0)
        goto wr_fail;

    for (i = 0; i < len; i++)
    {
        ack = I2C_Write(TP_I2C_X, buf[i], 1);
        if (ack == 0)
            goto wr_fail;
    }

    I2C_Stop(TP_I2C_X, 1);
    delay();
    return 0;

wr_fail:
    I2C_Stop(TP_I2C_X, 1);
    delay();
    printf("Touch WR FAIL!\n");
    return 1;
}

// TP_IC 读取寄存器
static uint32_t touch_read_regs(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ack;

    ack = I2C_Start(TP_I2C_X, FT_CMD_WR, 1);
    if (ack == 0)
        goto rd_fail;

    ack = I2C_Write(TP_I2C_X, reg & 0XFF, 1);
    if (ack == 0)
        goto rd_fail;

    delay(); // 此处必须延时等待一会再启动

    ack = I2C_Start(TP_I2C_X, FT_CMD_RD, 1); // ReStart
    if (ack == 0)
        goto rd_fail;

    for (i = 0; i < len; i++)
    {
        buf[i] = I2C_Read(TP_I2C_X, (i == (len - 1) ? 0 : 1), 1);
    }

    I2C_Stop(TP_I2C_X, 1);
    delay();
    return 0;

rd_fail:
    I2C_Stop(TP_I2C_X, 1);
    delay();
    printf("Touch RD FAIL!\n");
    return 1;
}

//----------------------------对外接口----------------------------//
/**
  \brief		TP_IC 读取触摸点
  \param [in]	tp_dev_t
  \return       0 : success    非0: error
  \note    		\
 */
uint8_t ft5206_read_points(tp_dev_t *tp_dev)
{
    uint8_t mode = 0; // 读取模式
    uint8_t buf[4];
    uint8_t i = 0;
    uint8_t res = 1;
    uint8_t temp;
    //{
    touch_read_regs(FT_REG_NUM_FINGER, &mode, 1); // 读取触摸点的状态
    // printf("0000\n");
    if ((mode & 0XF) && ((mode & 0XF) <= TP_MAX_TOUCH))
    {
        temp = 0XFF << (mode & 0XF); // 将点的个数转换为1的位数,匹配 tp_dev.sta 定义
        tp_dev->status = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
        // printf("111111\n");
        for (i = 0; i < TP_MAX_TOUCH; i++)
        {
            if (tp_dev->status & (1 << i)) // 触摸有效?
            {
                touch_read_regs(FT5206_TPX_Table[i], buf, 4); // 读取XY坐标值
#if 0                                                         // 横屏
										tp_dev->x[i] = ((uint16_t)(buf[0] & 0X0F) << 8) + buf[1];//FT5406 7INCH  //tp_dev->y[i] = ((uint16_t)(buf[0] & 0X0F) << 8) + buf[1];
										tp_dev->y[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];//FT5406 7INCH  //tp_dev->x[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];
#else                                                         // 竖屏
                tp_dev->y[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];
                tp_dev->x[i] = /*LCD_HDOT -*/ ((uint16_t)(buf[0] & 0X0F) << 8) + buf[1];
                // printf("22222\n");
#endif
            }
        }
        res = 0;
        if (tp_dev->x[0] == 0 && tp_dev->y[0] == 0)
        {
            mode = 0; // 读到的数据都是0,则忽略此次数据
        }
        // t = 0; //触发一次,则会最少连续监测10次,从而提高命中率
    }
    //}
    if ((mode & 0X1F) == 0) // 无触摸点按下
    {
        // printf("33333\n");
        if (tp_dev->status & TP_PRES_DOWN) // 之前是被按下的
        {
            tp_dev->status &= ~TP_PRES_DOWN; // 标记按键松开
        }
        else // 之前就没有被按下
        {
            tp_dev->x[0] = 0xffff;
            tp_dev->y[0] = 0xffff;
            tp_dev->status &= 0XE0; // 清除点有效标记
        }
    }
    return res;
}

/**
  \brief		TP_IC 初始化
  \param [in]	\
  \return       0 : success    非0: error
  \note    		\
 */
uint8_t ft5206_init(void)
{
    uint8_t temp[5] = {0};
    I2C_InitStructure I2C_initStruct;

    PORT_Init(TP_PORT_SCL, TP_PIN_SCL, TP_PIN_SCL_FUN, 1); // 配置为 I2C SCL 引脚
    TP_PORT_SCL->PULLU |= (1 << TP_PIN_SCL);               // 必须使能上拉
    TP_PORT_SCL->OPEND |= (1 << TP_PIN_SCL);               // 开漏
    PORT_Init(TP_PORT_SDA, TP_PIN_SDA, TP_PIN_SDA_FUN, 1); // 配置为 I2C SDA 引脚
    TP_PORT_SDA->PULLU |= (1 << TP_PIN_SDA);
    TP_PORT_SDA->OPEND |= (1 << TP_PIN_SDA);

    I2C_initStruct.Master = 1;
    I2C_initStruct.Addr10b = 0;
    I2C_initStruct.MstClk = 400000;
    I2C_initStruct.TXEmptyIEn = 0;    // 发送寄存器空中断使能
    I2C_initStruct.RXNotEmptyIEn = 0; // 接收寄存器非空中断使能
    //	I2C_initStruct.SlvAddr = 0;
    //	I2C_initStruct.SlvAddrMsk = 0;
    //	I2C_initStruct.SlvSTADetIEn = 0;//从机检测到起始中断使能
    //	I2C_initStruct.SlvSTODetIEn = 0;//从机检测到终止中断使能
    I2C_Init(TP_I2C_X, &I2C_initStruct);
    I2C_Open(TP_I2C_X);

    GPIO_Init(TP_GPIO_INT, TP_PIN_INT, 0, 0, 1, 0); // 输入，开启下拉。复位时INT为低，选择0xBA作为地址

    GPIO_Init(TP_GPIO_LED, TP_PIN_LED, 1, 1, 0, 0); // TP LED
    GPIO_ClrBit(TP_GPIO_LED, TP_PIN_LED);

    /*GPIO_Init(TP_GPIO_RST, TP_PIN_RST, 1, 1, 0, 0); //复位
    GPIO_ClrBit(TP_GPIO_RST, TP_PIN_RST);
    swm_delay_ms(200);
    GPIO_SetBit(TP_GPIO_RST, TP_PIN_RST); // 释放复位
    swm_delay_ms(120);*/

    temp[0] = 0;
    touch_write_regs(FT_DEVIDE_MODE, temp, 1); // 进入正常操作模式

    temp[0] = 1;                             // 0-查询模式		1-触发模式
    touch_write_regs(FT_ID_G_MODE, temp, 1); // 中断状态控制寄存器

    temp[0] = 22;                               // 触摸有效值，22，越小越灵敏
    touch_write_regs(FT_ID_G_THGROUP, temp, 1); // 设置触摸有效值

    temp[0] = 12; // 激活周期，不能小于12，最大14
    touch_write_regs(FT_ID_G_PERIODACTIVE, temp, 1);

    // 读取版本号，参考值：0x3003
    touch_read_regs(FT_ID_G_LIB_VERSION, &temp[0], 2);
    printf("Touch verson ID:%x, %x, %x, %x, %x\n", temp[0], temp[1], temp[2], temp[3], temp[4]);

    if ((temp[0] == 0X30 && temp[1] == 0X03) || temp[1] == 0X01 || temp[1] == 0X02)
    { // 版本:0X3003/0X0001/0X0002
        printf("Touch verson ID:%x, %x, %x, %x, %x\n", temp[0], temp[1], temp[2], temp[3], temp[4]);
        return 0;
    }
    return 1;
}
