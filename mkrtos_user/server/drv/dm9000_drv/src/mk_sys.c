#include "mk_sys.h"

// 外部中断配置函数
// 只针对GPIOA~G;不包括PVD,RTC和USB唤醒这三个
// 参数:
// GPIOx:0~6,代表GPIOA~G
// BITx:需要使能的位;
// TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
// 该函数一次只能配置1个IO口,多个IO口,需多次调用
// 该函数会自动开启对应中断,以及屏蔽线
void Ex_NVIC_Config(u8 GPIOx, u8 BITx, u8 TRIM)
{
    u8 EXTADDR;
    u8 EXTOFFSET;
    EXTADDR = BITx / 4; // 得到中断寄存器组的编号
    EXTOFFSET = (BITx % 4) * 4;
    RCC->APB2ENR |= 0x01;                            // 使能io复用时钟
    AFIO->EXTICR[EXTADDR] &= ~(0x000F << EXTOFFSET); // 清除原来设置！！！
    AFIO->EXTICR[EXTADDR] |= GPIOx << EXTOFFSET;     // EXTI.BITx映射到GPIOx.BITx
    // 自动设置
    EXTI->IMR |= 1 << BITx; //  开启line BITx上的中断
                            // EXTI->EMR|=1<<BITx;//不屏蔽line BITx上的事件 (如果不屏蔽这句,在硬件上是可以的,但是在软件仿真的时候无法进入中断!)
    if (TRIM & 0x01)
        EXTI->FTSR |= 1 << BITx; // line BITx上事件下降沿触发
    if (TRIM & 0x02)
        EXTI->RTSR |= 1 << BITx; // line BITx上事件上升降沿触发
}
