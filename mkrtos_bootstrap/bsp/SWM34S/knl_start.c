
#include <mk_sys.h>
#include <SWM341.h>
#include <system_SWM341.h>
void uart_init(void)
{
    UART_InitStructure UART_initStruct;

    PORT_Init(PORTM, PIN0, PORTM_PIN0_UART0_RX, 1); // GPIOM.0ÅäÖÃÎªUART0ÊäÈëÒý½Å
    PORT_Init(PORTM, PIN1, PORTM_PIN1_UART0_TX, 0); // GPIOM.1ÅäÖÃÎªUART0Êä³öÒý½Å

    UART_initStruct.Baudrate = 115200;
    UART_initStruct.DataBits = UART_DATA_8BIT;
    UART_initStruct.Parity = UART_PARITY_NONE;
    UART_initStruct.StopBits = UART_STOP_1BIT;
    UART_initStruct.RXThreshold = 3;
    UART_initStruct.RXThresholdIEn = 0;
    UART_initStruct.TXThreshold = 3;
    UART_initStruct.TXThresholdIEn = 0;
    UART_initStruct.TimeoutTime = 10;
    UART_initStruct.TimeoutIEn = 0;
    UART_Init(UART0, &UART_initStruct);
    UART_Open(UART0);
}
void putc(int ch)
{
    UART_WriteByte(UART0, ch);

    while (UART_IsTXBusy(UART0))
        ;
}
void print_str(const char *str)
{
    for (int i = 0; str[i]; i++)
    {
        putc(str[i]);
    }
}
static void sdram_init(void)
{
    SDRAM_InitStructure SDRAM_InitStruct;

    PORT_Init(PORTM, PIN13, PORTM_PIN13_SDR_CLK, 0);
    PORT_Init(PORTM, PIN14, PORTM_PIN14_SDR_CKE, 0);
    PORT_Init(PORTB, PIN7, PORTB_PIN7_SDR_WE, 0);
    PORT_Init(PORTB, PIN8, PORTB_PIN8_SDR_CAS, 0);
    PORT_Init(PORTB, PIN9, PORTB_PIN9_SDR_RAS, 0);
    PORT_Init(PORTB, PIN10, PORTB_PIN10_SDR_CS, 0);
    PORT_Init(PORTE, PIN15, PORTE_PIN15_SDR_BA0, 0);
    PORT_Init(PORTE, PIN14, PORTE_PIN14_SDR_BA1, 0);
    PORT_Init(PORTN, PIN14, PORTN_PIN14_SDR_A0, 0);
    PORT_Init(PORTN, PIN13, PORTN_PIN13_SDR_A1, 0);
    PORT_Init(PORTN, PIN12, PORTN_PIN12_SDR_A2, 0);
    PORT_Init(PORTN, PIN11, PORTN_PIN11_SDR_A3, 0);
    PORT_Init(PORTN, PIN10, PORTN_PIN10_SDR_A4, 0);
    PORT_Init(PORTN, PIN9, PORTN_PIN9_SDR_A5, 0);
    PORT_Init(PORTN, PIN8, PORTN_PIN8_SDR_A6, 0);
    PORT_Init(PORTN, PIN7, PORTN_PIN7_SDR_A7, 0);
    PORT_Init(PORTN, PIN6, PORTN_PIN6_SDR_A8, 0);
    PORT_Init(PORTN, PIN3, PORTN_PIN3_SDR_A9, 0);
    PORT_Init(PORTN, PIN15, PORTN_PIN15_SDR_A10, 0);
    PORT_Init(PORTN, PIN2, PORTN_PIN2_SDR_A11, 0);
    PORT_Init(PORTM, PIN15, PORTM_PIN15_SDR_A12, 0);
    PORT_Init(PORTE, PIN7, PORTE_PIN7_SDR_D0, 1);
    PORT_Init(PORTE, PIN6, PORTE_PIN6_SDR_D1, 1);
    PORT_Init(PORTE, PIN5, PORTE_PIN5_SDR_D2, 1);
    PORT_Init(PORTE, PIN4, PORTE_PIN4_SDR_D3, 1);
    PORT_Init(PORTE, PIN3, PORTE_PIN3_SDR_D4, 1);
    PORT_Init(PORTE, PIN2, PORTE_PIN2_SDR_D5, 1);
    PORT_Init(PORTE, PIN1, PORTE_PIN1_SDR_D6, 1);
    PORT_Init(PORTE, PIN0, PORTE_PIN0_SDR_D7, 1);
    PORT_Init(PORTE, PIN8, PORTE_PIN8_SDR_D8, 1);
    PORT_Init(PORTE, PIN9, PORTE_PIN9_SDR_D9, 1);
    PORT_Init(PORTE, PIN10, PORTE_PIN10_SDR_D10, 1);
    PORT_Init(PORTE, PIN11, PORTE_PIN11_SDR_D11, 1);
    PORT_Init(PORTE, PIN12, PORTE_PIN12_SDR_D12, 1);
    PORT_Init(PORTE, PIN13, PORTE_PIN13_SDR_D13, 1);
    PORT_Init(PORTC, PIN14, PORTC_PIN14_SDR_D14, 1);
    PORT_Init(PORTC, PIN15, PORTC_PIN15_SDR_D15, 1);
    PORT_Init(PORTB, PIN6, PORTB_PIN6_SDR_LDQM, 0);
    PORT_Init(PORTM, PIN12, PORTM_PIN12_SDR_UDQM, 0);

    SDRAM_InitStruct.Size = SDRAM_SIZE_8MB;
    SDRAM_InitStruct.ClkDiv = SDRAM_CLKDIV_1;
    SDRAM_InitStruct.CASLatency = SDRAM_CASLATENCY_2;
    SDRAM_InitStruct.TimeTRP = SDRAM_TRP_3;
    SDRAM_InitStruct.TimeTRCD = SDRAM_TRCD_3;
    SDRAM_InitStruct.TimeTRFC = SDRAM_TRFC_15;
    SDRAM_Init(&SDRAM_InitStruct);
}

int sram_test(void)
{
    volatile unsigned int *sram3_addr = (volatile unsigned int *)0x80000000;
    int i = 0;
    for (i = 0; i < 1024 * 1024 / 4; i++)
    {
        sram3_addr[i] = i;
    }
    for (i = 0; i < 1024 * 1024 / 4; i++)
    {
        if (i != sram3_addr[i])
        {
            return 0;
        }
    }
    return 1;
}

//! 内核镜像的开始地址
#define KERNEL_IMG_START_ADDR (CONFIG_KNL_TEXT_ADDR + CONFIG_KNL_OFFSET)
void jump2kernel(addr_t cpio_start, addr_t cpio_end)
{
    SystemInit();
    uart_init();
#if CONFIG_KNL_EXRAM
    sdram_init();
    if (sram_test() == 0)
    {
        print_str("sdram fail.\n");
    }
#endif
    print_str("system startup.\n");
    uint32_t jump_addr;
    void (*_main)(void);

    if (((*(__IO uint32_t *)KERNEL_IMG_START_ADDR) & 0x2FFE0000) == 0x20000000) // 检查栈顶地址是否合法,即检查此段Flash中是否已有APP程序
    {
        print_str("starting.\n");
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

        _main();
    }
}
