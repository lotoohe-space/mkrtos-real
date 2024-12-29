
#include <mk_sys.h>
#include <util.h>
#include <boot_info.h>
#include "at32f435_437_clock.h"
#include "at_surf_f437_board_qspi_sram.h"
#include "at_surf_f437_board_sdram.h"

//! 内核镜像的开始地址
#define KERNEL_IMG_START_ADDR (CONFIG_SYS_TEXT_ADDR + CONFIG_BOOTSTRAP_TEXT_SIZE + CONFIG_DTBO_TEXT_SIZE)

static boot_info_t boot_info = {
    .flash_layer = {
        /*flash布局*/
        .flash_layer_list = {
            {
                .st_addr = CONFIG_SYS_TEXT_ADDR, /*bootstrap*/
                .size = CONFIG_BOOTSTRAP_TEXT_SIZE,
                .name = "bootstrap",
            },
            {
                .st_addr = CONFIG_SYS_TEXT_ADDR + CONFIG_BOOTSTRAP_TEXT_SIZE, /*dtbo*/
                .size = CONFIG_DTBO_TEXT_SIZE,
                .name = "dtbo",
            },
            {
                .st_addr = KERNEL_IMG_START_ADDR, /*kernel*/
                .size = CONFIG_KNL_TEXT_SIZE,
                .name = "kernel",
            },
            {
                .st_addr = CONFIG_SYS_TEXT_ADDR + CONFIG_BOOTSTRAP_TEXT_SIZE + CONFIG_KNL_TEXT_SIZE + CONFIG_DTBO_TEXT_SIZE, /*bootfs*/
                .size = CONFIG_SYS_TEXT_SIZE - (CONFIG_BOOTSTRAP_TEXT_SIZE + CONFIG_KNL_TEXT_SIZE + CONFIG_DTBO_TEXT_SIZE),
                .name = "bootfs",
            },
        },
        .flash_layer_num = 4,
    }, /*flash布局*/
    .flash = {
        .flash_list = {
            {
                .addr = CONFIG_SYS_TEXT_ADDR,
                .size = CONFIG_SYS_TEXT_SIZE,
                .is_sys_mem = 1,
                .speed = 0,
            },
        },
        .flash_num = 1,
    },
    .mem = {
        .mem_list = {
            {
                .addr = CONFIG_SYS_DATA_ADDR,
                .size = CONFIG_SYS_DATA_SIZE,
                .is_sys_mem = 1,
                .speed = 0,
            },
            {
                .addr = 0xC0000000,
                .size = 32 * 1024 * 1024,
                .is_sys_mem = 0,
                .speed = 1,
            },
            {
                .addr = QSPI_SRAM_MEM_BASE,
                .size = 8 * 1024 * 1024,
                .is_sys_mem = 0,
                .speed = 2,
            },
        },
        .mem_num = 3,
    },
};
void delay_ms(int ms)
{
    for (volatile int i = 0; i < 1000000; i++)
        ;
}
static void mem_init(void)
{
    /* initialize qspi sram */
    qspi_sram_init();

    /* initialize sdram */
    sdram_init();
}

void jump2kernel(addr_t cpio_start, addr_t cpio_end)
{
    uint32_t jump_addr;
    SystemInit();
    system_clock_config();
    mem_init();
    //!< 检查栈顶地址是否合法,即检查此段Flash中是否已有APP程序
    if (((*(__IO uint32_t *)KERNEL_IMG_START_ADDR) & 0x2FFE0000) == CONFIG_SYS_DATA_ADDR)
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
        // __set_MSP(*(__IO uint32_t *)KERNEL_IMG_START_ADDR);
        __set_CONTROL(0);

        __ISB();

        __disable_irq();

        /* Set new vector table pointer */
        jump_addr = *(__IO uint32_t *)(KERNEL_IMG_START_ADDR + 4);
        _main = (void *)jump_addr;

        _main(&boot_info);
    }
}
