    .syntax unified
    .arch armv7-m
	.thumb

/* start address for the initialization values of the .data section. 
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */  
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss

    .section .isr_vector
    .align 2
    .globl __isr_vector
__isr_vector:
    .long    _estack
    .long    Reset_Handler         
    .long    NMI_Handler          
    .long    HardFault_Handler     
    .long    MemManage_Handler     
    .long    BusFault_Handler      
    .long    UsageFault_Handler   
    .long    0                    
    .long    0                    
    .long    0                    
    .long    0                     
    .long    SVC_Handler          
    .long    DebugMon_Handler     
    .long    0                     
    .long    PendSV_Handler           
    .long    SysTick_Handler         

    /* External interrupts */
    .long    entry_handler//UART0_Handler
    .long    entry_handler//UART1_Handler
    .long    entry_handler//TIMR0_Handler
    .long    entry_handler//TIMR1_Handler
    .long    entry_handler//DMA_Handler
    .long    entry_handler//SPI0_Handler
    .long    entry_handler//PWM0_Handler
    .long    entry_handler//WDT_Handler
    .long    entry_handler//UART2_Handler
    .long    entry_handler//PWM1_Handler
    .long    entry_handler//ADC0_Handler
    .long    entry_handler//BTIMR0_Handler
    .long    entry_handler//HALL0_Handler
    .long    entry_handler//PWM2_Handler
    .long    entry_handler//PWMBRK_Handler
    .long    entry_handler//I2C0_Handler
    .long    entry_handler//CAN0_Handler
    .long    entry_handler//SPI1_Handler
    .long    entry_handler//RTC_Handler
    .long    entry_handler//PWM3_Handler
    .long    entry_handler//TIMR2_Handler
    .long    entry_handler//UART3_Handler
    .long    entry_handler//TIMR3_Handler
    .long    entry_handler//ADC1_Handler
    .long    entry_handler//BOD_Handler
    .long    entry_handler//CORDIC_Handler
    .long    entry_handler//BTIMR1_Handler
    .long    entry_handler//PWM4_Handler
    .long    entry_handler//HALL3_Handler
    .long    entry_handler//BTIMR2_Handler
    .long    entry_handler//I2C1_Handler
    .long    entry_handler//BTIMR3_Handler
    .long    entry_handler//ACMP_Handler
    .long    entry_handler//XTALSTOP_Handler
    .long    entry_handler//FSPI_Handler
    .long    entry_handler//GPIOA_Handler
    .long    entry_handler//GPIOB_Handler
    .long    entry_handler//GPIOC_Handler
    .long    entry_handler//GPIOD_Handler
    .long    entry_handler//GPIOM_Handler
    .long    entry_handler//GPION_Handler
    .long    entry_handler//GPIOA0_Handler
    .long    entry_handler//GPIOA1_Handler
    .long    entry_handler//GPIOA5_Handler
    .long    entry_handler//GPIOA6_Handler
    .long    entry_handler//GPIOA10_Handler
    .long    entry_handler//GPIOA11_Handler
    .long    entry_handler//GPIOA12_Handler
    .long    entry_handler//GPIOA13_Handler
    .long    entry_handler//GPIOB0_Handler
    .long    entry_handler//GPIOB1_Handler
    .long    entry_handler//GPIOB2_Handler
    .long    entry_handler//GPIOC0_Handler
    .long    entry_handler//GPIOC1_Handler
    .long    entry_handler//GPIOC2_Handler
    .long    entry_handler//GPIOC3_Handler
    .long    entry_handler//GPIOC4_Handler
    .long    entry_handler//GPIOD3_Handler
    .long    entry_handler//GPIOD4_Handler
    .long    entry_handler//GPIOD5_Handler
    .long    entry_handler//GPIOD6_Handler
    .long    entry_handler//GPIOD7_Handler
    .long    entry_handler//GPIOD8_Handler
    .long    entry_handler//GPIOC9_Handler
    .long    entry_handler//GPIOC10_Handler
    .long    entry_handler//GPIOC11_Handler
    .long    entry_handler//GPIOC12_Handler
    .long    entry_handler//GPIOM0_Handler
    .long    entry_handler//GPIOM1_Handler
    .long    entry_handler//GPIOM2_Handler
    .long    entry_handler//GPIOM3_Handler
    .long    entry_handler//GPIOM4_Handler
    .long    entry_handler//DIV_Handler
    .long    entry_handler//LCD_Handler
    .long    entry_handler//GPIOE_Handler
    .long    entry_handler//JPEG_Handler
    .long    entry_handler//SDIO_Handler
    .long    entry_handler//USB_Handler
    .long    entry_handler//CAN1_Handler
    .long    entry_handler//TIMR4_Handler
    .long    entry_handler//BTIMR4_Handler
    .long    entry_handler//BTIMR5_Handler
    .long    entry_handler//BTIMR6_Handler
    .long    entry_handler//BTIMR7_Handler
    .long    entry_handler//BTIMR8_Handler
    .long    entry_handler//BTIMR9_Handler
    .long    entry_handler//BTIMR10_Handler
    .long    entry_handler//BTIMR11_Handler
    .long    entry_handler//DMA2D_Handler
    .long    entry_handler//QEI_Handler


	.section .text.Reset_Handler
    .align 2
    .globl    Reset_Handler
    .type     Reset_Handler, %function
Reset_Handler:
/* Loop to copy data from read only memory to RAM. The ranges
 * of copy from/to are specified by symbols evaluated in linker script.  */
    @ ldr    sp, =__StackTop    		 /* set stack pointer */


    ldr    r1, =_sidata
    ldr    r2, =_sdata
    ldr    r3, =_edata

.Lflash_to_ram_loop:
    cmp     r2, r3
    ittt    lo
    ldrlo   r0, [r1], #4
    strlo   r0, [r2], #4
    blo    .Lflash_to_ram_loop


    ldr    r2, =__bss_start__
    ldr    r3, =__bss_end__

.Lbss_to_ram_loop:
    cmp     r2, r3
    ittt    lo
    movlo   r0, #0
    strlo   r0, [r2], #4
    blo    .Lbss_to_ram_loop

    ldr r0, = _estack
    ldr r0, [r0]
    mov sp, r0

    ldr    r0, =start_kernel
    bx     r0
    .pool    


    .text
/* Macro to define default handlers. 
   Default handler will be weak symbol and just dead loops. */
    .macro    def_default_handler    handler_name
    .align 1
    .thumb_func
    .weak    \handler_name
    .type    \handler_name, %function
\handler_name :
    b    .
    .endm

    def_default_handler    NMI_Handler
    def_default_handler    HardFault_Handler
    def_default_handler    MemManage_Handler
    def_default_handler    BusFault_Handler
    def_default_handler    UsageFault_Handler
    def_default_handler    SVC_Handler
    def_default_handler    DebugMon_Handler
    @ def_default_handler    PendSV_Handler
    @ def_default_handler    SysTick_Handler

    @ def_default_handler    UART0_Handler
    @ def_default_handler    UART1_Handler
    @ def_default_handler    TIMR0_Handler
    @ def_default_handler    TIMR1_Handler
    @ def_default_handler    DMA_Handler
    @ def_default_handler    SPI0_Handler
    @ def_default_handler    PWM0_Handler
    @ def_default_handler    WDT_Handler
    @ def_default_handler    UART2_Handler
    @ def_default_handler    PWM1_Handler
    @ def_default_handler    ADC0_Handler
    @ def_default_handler    BTIMR0_Handler
    @ def_default_handler    HALL0_Handler
    @ def_default_handler    PWM2_Handler
    @ def_default_handler    PWMBRK_Handler
    @ def_default_handler    I2C0_Handler
    @ def_default_handler    CAN0_Handler
    @ def_default_handler    SPI1_Handler
    @ def_default_handler    RTC_Handler
    @ def_default_handler    PWM3_Handler
    @ def_default_handler    TIMR2_Handler
    @ def_default_handler    UART3_Handler
    @ def_default_handler    TIMR3_Handler
    @ def_default_handler    ADC1_Handler
    @ def_default_handler    BOD_Handler
    @ def_default_handler    CORDIC_Handler
    @ def_default_handler    BTIMR1_Handler
    @ def_default_handler    PWM4_Handler
    @ def_default_handler    HALL3_Handler
    @ def_default_handler    BTIMR2_Handler
    @ def_default_handler    I2C1_Handler
    @ def_default_handler    BTIMR3_Handler
    @ def_default_handler    ACMP_Handler
    @ def_default_handler    XTALSTOP_Handler
    @ def_default_handler    FSPI_Handler
    @ def_default_handler    GPIOA_Handler
    @ def_default_handler    GPIOB_Handler
    @ def_default_handler    GPIOC_Handler
    @ def_default_handler    GPIOD_Handler
    @ def_default_handler    GPIOM_Handler
    @ def_default_handler    GPION_Handler
    @ def_default_handler    GPIOA0_Handler
    @ def_default_handler    GPIOA1_Handler
    @ def_default_handler    GPIOA5_Handler
    @ def_default_handler    GPIOA6_Handler
    @ def_default_handler    GPIOA10_Handler
    @ def_default_handler    GPIOA11_Handler
    @ def_default_handler    GPIOA12_Handler
    @ def_default_handler    GPIOA13_Handler
    @ def_default_handler    GPIOB0_Handler
    @ def_default_handler    GPIOB1_Handler
    @ def_default_handler    GPIOB2_Handler
    @ def_default_handler    GPIOC0_Handler
    @ def_default_handler    GPIOC1_Handler
    @ def_default_handler    GPIOC2_Handler
    @ def_default_handler    GPIOC3_Handler
    @ def_default_handler    GPIOC4_Handler
    @ def_default_handler    GPIOD3_Handler
    @ def_default_handler    GPIOD4_Handler
    @ def_default_handler    GPIOD5_Handler
    @ def_default_handler    GPIOD6_Handler
    @ def_default_handler    GPIOD7_Handler
    @ def_default_handler    GPIOD8_Handler
    @ def_default_handler    GPIOC9_Handler
    @ def_default_handler    GPIOC10_Handler
    @ def_default_handler    GPIOC11_Handler
    @ def_default_handler    GPIOC12_Handler
    @ def_default_handler    GPIOM0_Handler
    @ def_default_handler    GPIOM1_Handler
    @ def_default_handler    GPIOM2_Handler
    @ def_default_handler    GPIOM3_Handler
    @ def_default_handler    GPIOM4_Handler
    @ def_default_handler    DIV_Handler
    @ def_default_handler    LCD_Handler
    @ def_default_handler    GPIOE_Handler
    @ def_default_handler    JPEG_Handler
    @ def_default_handler    SDIO_Handler
    @ def_default_handler    USB_Handler
    @ def_default_handler    CAN1_Handler
    @ def_default_handler    TIMR4_Handler
    @ def_default_handler    BTIMR4_Handler
    @ def_default_handler    BTIMR5_Handler
    @ def_default_handler    BTIMR6_Handler
    @ def_default_handler    BTIMR7_Handler
    @ def_default_handler    BTIMR8_Handler
    @ def_default_handler    BTIMR9_Handler
    @ def_default_handler    BTIMR10_Handler
    @ def_default_handler    BTIMR11_Handler
    @ def_default_handler    DMA2D_Handler
    @ def_default_handler    QEI_Handler

    @ def_default_handler    Default_Handler

    .end
