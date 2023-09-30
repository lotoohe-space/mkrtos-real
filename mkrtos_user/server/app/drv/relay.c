
#include "u_types.h"
#include "sysinfo.h"
#include "stm32_sys.h"
#include <assert.h>

void relay_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void relay_ctrl(uint8_t index, uint8_t state)
{
    state = !state;
    sys_info.relay[index] = state;

    switch (index)
    {
    case 0:
        if (state)
        {
            GPIO_SetBits(GPIOA, GPIO_Pin_8);
        }
        else
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_8);
        }
        break;
    case 1:
        if (state)
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_6);
        }
        else
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_6);
        }
        break;
    case 2:
        if (state)
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_7);
        }
        else
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_7);
        }
        break;
    case 3:
        if (state)
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_8);
        }
        else
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_8);
        }
        break;
    case 4:
        if (state)
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_9);
        }
        else
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_9);
        }
        break;
    case 5:
        if (state)
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_7);
        }
        else
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_7);
        }
        break;
    default:
        assert(0);
        break;
    }
}
