
#include <mk_sys.h>
#include "sram.h"
//! 内核镜像的开始地址
#define KERNEL_IMG_START_ADDR (CONFIG_KNL_TEXT_ADDR + CONFIG_KNL_OFFSET)
uint32_t jump_addr;
void (*_main)(void);

void sram_init(void)
{
    FSMC_SRAM_Init();
}
void jump2kernel(addr_t cpio_start, addr_t cpio_end)
{
#if CONFIG_KNL_EXRAM
    sram_init();
    // sram_test();
#endif
    if (((*(__IO uint32_t *)KERNEL_IMG_START_ADDR) & 0x2FFE0000) == 0x20000000) // 检查栈顶地址是否合法,即检查此段Flash中是否已有APP程序
    {
        __set_PRIMASK(1);

        /* disable UART */
        // USART_Reset(USART1);
        /* disable GPIO */
        // GPIO_Reset(GPIOA);
        /* disable RCC */
        // RCC_Reset();
        /* disable EXTI */
        // EXTI_Reset();

        /* disable and clean up all interrupts. */
        {
            int i;

            for (i = 0; i < 8; i++)
            {
                /* disable interrupts. */
                NVIC->ICER[i] = 0xFFFFFFFF;

                /* clean up interrupts flags. */
                NVIC->ICPR[i] = 0xFFFFFFFF;
            }
        }

        /* reset register values */
        __set_BASEPRI(0);
        __set_FAULTMASK(0);

        /* initialize main stack pointer */

        __set_CONTROL(0);

        __ISB();

        __disable_irq();

        //  __set_MSP(*(__IO uint32_t *)KERNEL_IMG_START_ADDR);
        /* Set new vector table pointer */
        jump_addr = *(__IO uint32_t *)(KERNEL_IMG_START_ADDR + 4);
        _main = (void *)jump_addr;

        _main();
    }
}
