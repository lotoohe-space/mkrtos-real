//----------------------------头文件依赖----------------------------//
#include "dev_touch.h"
#include "gui.h"
// #include "main.h"
// #include "irq_handler.h"

//----------------------------全局变量----------------------------//
/* 依靠 tp_dev_t 建立对 TP_IC 的抽象层, 以便于统一管理 */
tp_dev_t Dev_TP = {
    .status = 0,
    .x = 0,
    .y = 0,
    .init_cb = NULL,
    .read_cb = NULL,
    .align_cb = NULL,
}; // 实例化
volatile uint8_t TP_INT_Trigger = 0;
#include <assert.h>
#include <u_prot.h>
#include <u_intr.h>
#define IRQ_THREAD_PRIO 16
#define STACK_SIZE (512 + 512)
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
static uint8_t msg_buf[128];
obj_handler_t irq_obj;
void GPION_Handler(void)
{
    while (1)
    {
        msg_tag_t tag = uirq_wait(irq_obj, 0);
        if (msg_tag_get_val(tag) >= 0)
        {
            if (EXTI_State(TP_GPIO_INT, TP_PIN_INT))
            {
                EXTI_Clear(TP_GPIO_INT, TP_PIN_INT);

                TP_INT_Trigger = 1;
            }
        }
        uirq_ack(irq_obj, GPION_IRQn);
    }
}

//----------------------------函数定义----------------------------//
// 触摸事件处理
void touch_handler(void)
{
    if (TP_INT_Trigger) // INT 中断触发->查询扫描
    {
        TP_INT_Trigger = 0;
        Dev_TP.read_cb(&Dev_TP);
        // printf("x = [%d], y = [%d], int_trigger = [%d]\r\n", Dev_TP.x[0], Dev_TP.y[0], TP_INT_Trigger);
        // Systick_Ms_User = 0;
    }
}

// 初始化触摸IC及注册回调接口
uint8_t touch_ic_init(void)
{
#if (LCD_SCREEN_W == 480 && LCD_SCREEN_H == 272) // 480*272

#include "TP_GT911.h" //(JLT4303A)——晶力泰 4.3 寸电容屏
    Dev_TP.init_cb = gt9x_init;
    Dev_TP.read_cb = gt9x_read_points;

#elif (LCD_SCREEN_W == 480 && LCD_SCREEN_H == 480) // 480*480

#if (LCD_TYPE == TL040WVS03)

#include "TP_FT6336.h" //(TL040WVS03)——冠显光电 4.0 寸方形屏
    Dev_TP.init_cb = ft6336_init;
    Dev_TP.read_cb = ft6336_read_points;

#elif (LCD_TYPE == TL021WVC02_Round)

#include "TP_CST826.h" //(TL021WC02_Round)——冠显光电 2.1 寸圆形屏
    Dev_TP.init_cb = cst826_init;
    Dev_TP.read_cb = cst826_read_points;

#endif // LCD_TYPE

#elif (LCD_SCREEN_W == 480 && LCD_SCREEN_H == 800) // 480*800
#if (LCD_TYPE == RV_HX8363A)
#include "TP_GT911.h"
    Dev_TP.init_cb = gt9x_init;
    Dev_TP.read_cb = gt9x_read_points;

#elif (LCD_TYPE == RV_OTM8009A)

#include "TP_FT5206.h"
    Dev_TP.init_cb = ft5206_init;
    Dev_TP.read_cb = ft5206_read_points;

#endif // LCD_TYPE

#elif (LCD_SCREEN_W == 800 && LCD_SCREEN_H == 480) // 800*480

#if (LCD_TYPE == JLT4305A)

#include "TP_GT911.h" //(JLT4305A)——晶力泰 4.3 寸电容屏
    Dev_TP.init_cb = gt9x_init;
    Dev_TP.read_cb = gt9x_read_points;

#elif (LCD_TYPE == ATK_MD0700R)

#include "TP_FT5206.h" //(ATK_MD0700R)——正点原子 7 寸电容屏
    Dev_TP.init_cb = ft5206_init;
    Dev_TP.read_cb = ft5206_read_points;

#endif // LCD_TYPE

#elif (LCD_SCREEN_W == 1024 && LCD_SCREEN_H == 600) // 1024*600

#if (LCD_TYPE == WKS70170)

#include "TP_FT5206.h" //(WKS70170)——慧勤光电 7 寸电容屏
    Dev_TP.init_cb = ft5206_init;
    Dev_TP.read_cb = ft5206_read_points;

#elif (LCD_TYPE == H743)

#include "TP_GT911.h" //(H743)——玫瑰科技 7 寸电容屏
    Dev_TP.init_cb = gt9x_init;
    Dev_TP.read_cb = gt9x_read_points;

#endif // LCD_TYPE

#else // other custom

#error "You must provide your customized resolution Driver!"

#endif //(LCD_SCREEN_W == ? && LCD_SCREEN_H == ?)

    uint8_t res = 0;
    if (Dev_TP.init_cb != NULL)
    {
        res = Dev_TP.init_cb();
    }

    if (Dev_TP.read_cb != NULL)
    {
#if 1 // 中断方式选用边沿触发，则需要在 TP 驱动中设置 TP 芯片的扫描方式为中断触发，如此才能使得长时间按压时不断触发 TP_INT 边沿脉冲信号
        GPIO_Init(TP_GPIO_INT, TP_PIN_INT, 0, 0, 0, 0);
        EXTI_Init(TP_GPIO_INT, TP_PIN_INT, EXTI_FALL_EDGE); // EXTI_FALL_EDGE - 下降沿触发 / EXTI_RISE_EDGE - 上升沿触发
        // NVIC_EnableIRQ(GPION_IRQn);

        assert(u_intr_bind(GPION_IRQn, u_irq_prio_create(1, 0), IRQ_THREAD_PRIO,
                           stack0 + STACK_SIZE, msg_buf, GPION_Handler, &irq_obj) >= 0);
        uirq_ack(irq_obj, GPION_IRQn);
        EXTI_Open(TP_GPIO_INT, TP_PIN_INT);
#endif
    }

    if (Dev_TP.align_cb != NULL)
    {
        // res = Dev_TP.align_cb();//电阻屏校准--预留接口待实现
    }

    return res;
}
