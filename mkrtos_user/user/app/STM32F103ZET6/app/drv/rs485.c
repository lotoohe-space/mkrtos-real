
#include "stm32_sys.h"

void rs485_init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); // 使能PORTC口时钟
    BKP_TamperPinCmd(DISABLE);                                                  // 关闭入侵检测功能，也就是PC13，可以当普通IO使用
    BKP_ITConfig(DISABLE);                                                      // 这样就可以当输出用。

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 最低速率2MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_11 | GPIO_Pin_12); // 默认输出
}