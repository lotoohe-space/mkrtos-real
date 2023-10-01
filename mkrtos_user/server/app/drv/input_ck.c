
#include "input_ck.h"
#include "stm32_sys.h"
void ext_input_check(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    PWR_BackupAccessCmd(ENABLE); /* 允许修改RTC和后备寄存器*/
    RCC_LSEConfig(RCC_LSE_OFF);  /* 关闭外部低速时钟,PC14+PC15可以用作普通IO*/
    BKP_TamperPinCmd(DISABLE);   /* 关闭入侵检测功能,PC13可以用作普通IO*/

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    PWR_BackupAccessCmd(DISABLE); /* 禁止修改RTC和后备寄存器*/
}
