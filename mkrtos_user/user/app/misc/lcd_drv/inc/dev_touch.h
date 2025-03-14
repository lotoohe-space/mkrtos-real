#ifndef __DEV_TOUCH_H__
#define __DEV_TOUCH_H__

//----------------------------头文件依赖----------------------------//
#include "SWM341.h"
#include "version.h"

//----------------------------宏定义----------------------------//
// 端口配置

#if (SWM34S_LCM_PCBV == SWM34SRE_PIN64_A001) // SWM34SRET6_Pin64

#define TP_GPIO_RST GPIOC
#define TP_PIN_RST PIN8

#define TP_GPIO_LED GPIOA
#define TP_PIN_LED PIN8

#define TP_GPIO_INT GPION
#define TP_PIN_INT PIN5

#define TP_PORT_SCL PORTC
#define TP_PIN_SCL PIN5
#define TP_PIN_SCL_FUN PORTC_PIN5_I2C1_SCL

#define TP_PORT_SDA PORTC
#define TP_PIN_SDA PIN4
#define TP_PIN_SDA_FUN PORTC_PIN4_I2C1_SDA

#define TP_I2C_X I2C1

#else // SWM34SVET6_Pin100

#define TP_GPIO_RST GPIOA
#define TP_PIN_RST PIN5

#define TP_GPIO_INT GPIOD
#define TP_PIN_INT PIN0

#if (SWM34S_LCM_PCBV == SWM34SVE_PIN100_A001)
#define TP_PORT_SCL PORTA
#define TP_PIN_SCL PIN7
#define TP_PIN_SCL_FUN PORTA_PIN7_I2C1_SCL

#define TP_PORT_SDA PORTA
#define TP_PIN_SDA PIN6
#define TP_PIN_SDA_FUN PORTA_PIN6_I2C1_SDA

#define TP_I2C_X I2C1

#elif (SWM34S_LCM_PCBV == SWM34SVE_PIN100_A002)
#define TP_PORT_SCL PORTA
#define TP_PIN_SCL PIN1
#define TP_PIN_SCL_FUN PORTA_PIN1_I2C0_SCL

#define TP_PORT_SDA PORTA
#define TP_PIN_SDA PIN0
#define TP_PIN_SDA_FUN PORTA_PIN0_I2C0_SDA

#define TP_I2C_X I2C0

#endif // SWM34SVE_PIN100_A00X

#endif // SWM34S_LCM_PCBV

#define TP_PRES_DOWN (0x80) // 触屏被按下
#define TP_CATH_PRES (0x40) // 有按键按下
#define TP_MAX_TOUCH (5)    // 电容屏支持的点数,固定最大为5点

//----------------------------类型声明----------------------------//
// 触摸屏控制器
typedef struct _tp_dev_t
{
    uint16_t x[TP_MAX_TOUCH]; // 当前坐标
    uint16_t y[TP_MAX_TOUCH]; // 电容屏有最多5组坐标(目前没有使用到多点触控, 默认取第一个数据[0])
    uint8_t status;           // 笔的状态
                    // b7:按下1/松开0;
                    // b6~b5:保留
                    // b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)

    /* 以下接口仅供内部调用 */
    // uint8_t int_trigger;	  //TP_INT边沿触发标志 : 0-未触发  1-已触发
    uint8_t (*init_cb)(void);
    uint8_t (*read_cb)(struct _tp_dev_t *);
    uint8_t (*align_cb)(uint16_t *, uint8_t); // 电阻屏四点校准标定(校准点坐标数组, 数组大小)
} tp_dev_t;

//----------------------------外部变量----------------------------//
extern tp_dev_t Dev_TP;

//----------------------------函数定义----------------------------//
uint8_t touch_ic_init(void);
void touch_handler(void);

#endif //__DEV_TOUCH_H__
