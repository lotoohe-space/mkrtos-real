/**
  ******************************************************************************
  * @file     startup_at32f435_437.s
  * @brief    at32f435_437 devices vector table for gcc toolchain.
  *           this module performs:
  *           - set the initial sp
  *           - set the initial pc == reset_handler,
  *           - set the vector table entries with the exceptions isr address
  *           - configure the clock system and the external sram to
  *             be used as data memory (optional, to be enabled by user)
  *           - branches to main in the c library (which eventually
  *             calls main()).
  *           after reset the cortex-m4 processor is in thread mode,
  *           priority is privileged, and the stack is set to main.
  ******************************************************************************
  */

  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler

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
/* stack used for SystemInit_ExtMemCtl; always internal RAM used */

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval None
*/

    .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:
  mov r7, r0

/* Copy the data segment initializers from flash to SRAM */
  movs  r1, #0
  b  LoopCopyDataInit

CopyDataInit:
  ldr  r3, =_sidata
  ldr  r3, [r3, r1]
  str  r3, [r0, r1]
  adds  r1, r1, #4

LoopCopyDataInit:
  ldr  r0, =_sdata
  ldr  r3, =_edata
  adds  r2, r0, r1
  cmp  r2, r3
  bcc  CopyDataInit
  ldr  r2, =_sbss
  b  LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
  movs  r3, #0
  str  r3, [r2], #4

LoopFillZerobss:
  ldr  r3, = _ebss
  cmp  r2, r3
  bcc  FillZerobss

/* Set sp */
  ldr r0, = _estack
  ldr r0, [r0]
  msr msp, r0

/*set boot info. */
  mov r0, r7
/* Call the clock system intitialization function.*/
 // bl  SystemInit
/* Call static constructors */
//  bl __libc_init_array
/* Call the application's entry point.*/
  bl  start_kernel
  bx  lr
.size  Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 * @param  None
 * @retval None
*/
    .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex M3. Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
*******************************************************************************/
   .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
  .word  _estack
  .word  Reset_Handler
  .word  NMI_Handler
  .word  HardFault_Handler
  .word  MemManage_Handler
  .word  BusFault_Handler
  .word  UsageFault_Handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  SVC_Handler
  .word  DebugMon_Handler
  .word  0
  .word  PendSV_Handler
  .word  SysTick_Handler

  /* External Interrupts */
  .word  entry_handler        /* Window Watchdog Timer                   */
  .word  entry_handler        /* PVM through EXINT Line detect           */
  .word  entry_handler        /* Tamper and TimeStamps through the EXINT line */
  .word  entry_handler        /* ERTC Wakeup through the EXINT line      */
  .word  entry_handler        /* Flash                                   */
  .word  entry_handler        /* CRM                                     */
  .word  entry_handler        /* EXINT Line 0                            */
  .word  entry_handler        /* EXINT Line 1                            */
  .word  entry_handler        /* EXINT Line 2                            */
  .word  entry_handler        /* EXINT Line 3                            */
  .word  entry_handler        /* EXINT Line 4                            */
  .word  entry_handler        /* EDMA Stream 1                           */
  .word  entry_handler        /* EDMA Stream 2                           */
  .word  entry_handler        /* EDMA Stream 3                           */
  .word  entry_handler        /* EDMA Stream 4                           */
  .word  entry_handler        /* EDMA Stream 5                           */
  .word  entry_handler        /* EDMA Stream 6                           */
  .word  entry_handler        /* EDMA Stream 7                           */
  .word  entry_handler        /* ADC1 & ADC2 & ADC3                      */
  .word  entry_handler        /* CAN1 TX                                 */
  .word  entry_handler        /* CAN1 RX0                                */
  .word  entry_handler        /* CAN1 RX1                                */
  .word  entry_handler        /* CAN1 SE                                 */
  .word  entry_handler        /* EXINT Line [9:5]                        */
  .word  entry_handler        /* TMR1 Brake and TMR9                     */
  .word  entry_handler        /* TMR1 Overflow and TMR10                 */
  .word  entry_handler      /* TMR1 Trigger and hall and TMR11         */
  .word  entry_handler        /* TMR1 Channel                            */
  .word  entry_handler        /* TMR2                                    */
  .word  entry_handler        /* TMR3                                    */
  .word  entry_handler        /* TMR4                                    */
  .word  entry_handler        /* I2C1 Event                              */
  .word  entry_handler        /* I2C1 Error                              */
  .word  entry_handler        /* I2C2 Event                              */
  .word  entry_handler        /* I2C2 Error                              */
  .word  entry_handler        /* SPI1                                    */
  .word  entry_handler        /* SPI2                                    */
  .word  entry_handler        /* USART1                                  */
  .word  entry_handler        /* USART2                                  */
  .word  entry_handler        /* USART3                                  */
  .word  entry_handler        /* EXINT Line [15:10]                      */
  .word  entry_handler        /* RTC Alarm through EXINT Line            */
  .word  entry_handler        /* OTGFS1 Wakeup from suspend              */
  .word  entry_handler        /* TMR8 Brake and TMR12                    */
  .word  entry_handler        /* TMR8 Overflow and TMR13                 */
  .word  entry_handler      /* TMR8 Trigger and hall and TMR14         */
  .word  entry_handler        /* TMR8 Channel                            */
  .word  entry_handler        /* EDMA Stream 8                           */
  .word  entry_handler        /* XMC                                     */
  .word  entry_handler        /* SDIO1                                   */
  .word  entry_handler        /* TMR5                                    */
  .word  entry_handler        /* SPI3                                    */
  .word  entry_handler        /* UART4                                   */
  .word  entry_handler        /* UART5                                   */
  .word  entry_handler        /* TMR6 & DAC                              */
  .word  entry_handler        /* TMR7                                    */
  .word  entry_handler        /* DMA1 Channel 1                          */
  .word  entry_handler        /* DMA1 Channel 2                          */
  .word  entry_handler        /* DMA1 Channel 3                          */
  .word  entry_handler        /* DMA1 Channel 4                          */
  .word  entry_handler        /* DMA1 Channel 5                          */
  .word  entry_handler        /* EMAC                                    */
  .word  entry_handler        /* EMAC Wakeup                             */
  .word  entry_handler        /* CAN2 TX                                 */
  .word  entry_handler        /* CAN2 RX0                                */
  .word  entry_handler        /* CAN2 RX1                                */
  .word  entry_handler        /* CAN2 SE                                 */
  .word  entry_handler        /* OTGFS1                                  */
  .word  entry_handler        /* DMA1 Channel 6                          */
  .word  entry_handler        /* DMA1 Channel 7                          */
  .word  0                                   /* Reserved                                */
  .word  entry_handler          /* USART6                                  */
  .word  entry_handler          /* I2C3 Event                              */
  .word  entry_handler          /* I2C3 Error                              */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  entry_handler         /* OTGFS2 Wakeup from suspend              */
  .word  entry_handler         /* OTGFS2                                  */
  .word  entry_handler         /* DVP                                     */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  entry_handler            /* FPU                                     */
  .word  entry_handler            /* UART7                                   */
  .word  entry_handler            /* UART8                                   */
  .word  entry_handler            /* SPI4                                    */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  entry_handler                    /* QSPI2                                   */
  .word  entry_handler                    /* QSPI1                                   */
  .word  0                                   /* Reserved                                */
  .word  entry_handler                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  0                                   /* Reserved                                */
  .word  entry_handler           /* SDIO2                                   */
  .word  entry_handler           /* ACC                                     */
  .word  entry_handler           /* TMR20 Brake                             */
  .word  entry_handler           /* TMR20 Overflow                          */
  .word  entry_handler           /* TMR20 Trigger and hall                  */
  .word  entry_handler           /* TMR20 Channel                           */
  .word  entry_handler           /* DMA2 Channel 1                          */
  .word  entry_handler           /* DMA2 Channel 2                          */
  .word  entry_handler           /* DMA2 Channel 3                          */
  .word  entry_handler           /* DMA2 Channel 4                          */
  .word  entry_handler           /* DMA2 Channel 5                          */
  .word  entry_handler           /* DMA2 Channel 6                          */
  .word  entry_handler           /* DMA2 Channel 7                          */

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/
  @  .weak      entry_handler
  @  .thumb_set entry_handler,Default_Handler

  @  .weak      HardFault_Handler
  @  .thumb_set HardFault_Handler,Default_Handler

  @  .weak      MemManage_Handler
  @  .thumb_set MemManage_Handler,Default_Handler

  @  .weak      BusFault_Handler
  @  .thumb_set BusFault_Handler,Default_Handler

  @  .weak      UsageFault_Handler
  @  .thumb_set UsageFault_Handler,Default_Handler

  @  .weak      SVC_Handler
  @  .thumb_set SVC_Handler,Default_Handler

  @  .weak      DebugMon_Handler
  @  .thumb_set DebugMon_Handler,Default_Handler

  @  .weak      PendSV_Handler
  @  .thumb_set PendSV_Handler,Default_Handler

  @  .weak      SysTick_Handler
  @  .thumb_set SysTick_Handler,Default_Handler

  @  .weak      WWDT_IRQHandler
  @  .thumb_set WWDT_IRQHandler,Default_Handler

  @  .weak      PVM_IRQHandler
  @  .thumb_set PVM_IRQHandler,Default_Handler

  @  .weak      TAMP_STAMP_IRQHandler
  @  .thumb_set TAMP_STAMP_IRQHandler,Default_Handler

  @  .weak      ERTC_WKUP_IRQHandler
  @  .thumb_set ERTC_WKUP_IRQHandler,Default_Handler

  @  .weak      FLASH_IRQHandler
  @  .thumb_set FLASH_IRQHandler,Default_Handler

  @  .weak      CRM_IRQHandler
  @  .thumb_set CRM_IRQHandler,Default_Handler

  @  .weak      EXINT0_IRQHandler
  @  .thumb_set EXINT0_IRQHandler,Default_Handler

  @  .weak      EXINT1_IRQHandler
  @  .thumb_set EXINT1_IRQHandler,Default_Handler

  @  .weak      EXINT2_IRQHandler
  @  .thumb_set EXINT2_IRQHandler,Default_Handler

  @  .weak      EXINT3_IRQHandler
  @  .thumb_set EXINT3_IRQHandler,Default_Handler

  @  .weak      EXINT4_IRQHandler
  @  .thumb_set EXINT4_IRQHandler,Default_Handler

  @  .weak      EDMA_Stream1_IRQHandler
  @  .thumb_set EDMA_Stream1_IRQHandler,Default_Handler

  @  .weak      EDMA_Stream2_IRQHandler
  @  .thumb_set EDMA_Stream2_IRQHandler,Default_Handler

  @  .weak      EDMA_Stream3_IRQHandler
  @  .thumb_set EDMA_Stream3_IRQHandler,Default_Handler

  @  .weak      EDMA_Stream4_IRQHandler
  @  .thumb_set EDMA_Stream4_IRQHandler,Default_Handler

  @  .weak      EDMA_Stream5_IRQHandler
  @  .thumb_set EDMA_Stream5_IRQHandler,Default_Handler

  @  .weak      EDMA_Stream6_IRQHandler
  @  .thumb_set EDMA_Stream6_IRQHandler,Default_Handler

  @  .weak      EDMA_Stream7_IRQHandler
  @  .thumb_set EDMA_Stream7_IRQHandler,Default_Handler

  @  .weak      ADC1_2_3_IRQHandler
  @  .thumb_set ADC1_2_3_IRQHandler,Default_Handler

  @  .weak      CAN1_TX_IRQHandler
  @  .thumb_set CAN1_TX_IRQHandler,Default_Handler

  @  .weak      CAN1_RX0_IRQHandler
  @  .thumb_set CAN1_RX0_IRQHandler,Default_Handler

  @  .weak      CAN1_RX1_IRQHandler
  @  .thumb_set CAN1_RX1_IRQHandler,Default_Handler

  @  .weak      CAN1_SE_IRQHandler
  @  .thumb_set CAN1_SE_IRQHandler,Default_Handler

  @  .weak      EXINT9_5_IRQHandler
  @  .thumb_set EXINT9_5_IRQHandler,Default_Handler

  @  .weak      TMR1_BRK_TMR9_IRQHandler
  @  .thumb_set TMR1_BRK_TMR9_IRQHandler,Default_Handler

  @  .weak      TMR1_OVF_TMR10_IRQHandler
  @  .thumb_set TMR1_OVF_TMR10_IRQHandler,Default_Handler

  @  .weak      TMR1_TRG_HALL_TMR11_IRQHandler
  @  .thumb_set TMR1_TRG_HALL_TMR11_IRQHandler,Default_Handler

  @  .weak      TMR1_CH_IRQHandler
  @  .thumb_set TMR1_CH_IRQHandler,Default_Handler

  @  .weak      TMR2_GLOBAL_IRQHandler
  @  .thumb_set TMR2_GLOBAL_IRQHandler,Default_Handler

  @  .weak      TMR3_GLOBAL_IRQHandler
  @  .thumb_set TMR3_GLOBAL_IRQHandler,Default_Handler

  @  .weak      TMR4_GLOBAL_IRQHandler
  @  .thumb_set TMR4_GLOBAL_IRQHandler,Default_Handler

  @  .weak      I2C1_EVT_IRQHandler
  @  .thumb_set I2C1_EVT_IRQHandler,Default_Handler

  @  .weak      I2C1_ERR_IRQHandler
  @  .thumb_set I2C1_ERR_IRQHandler,Default_Handler

  @  .weak      I2C2_EVT_IRQHandler
  @  .thumb_set I2C2_EVT_IRQHandler,Default_Handler

  @  .weak      I2C2_ERR_IRQHandler
  @  .thumb_set I2C2_ERR_IRQHandler,Default_Handler

  @  .weak      SPI1_IRQHandler
  @  .thumb_set SPI1_IRQHandler,Default_Handler

  @  .weak      SPI2_I2S2EXT_IRQHandler
  @  .thumb_set SPI2_I2S2EXT_IRQHandler,Default_Handler

  @  .weak      USART1_IRQHandler
  @  .thumb_set USART1_IRQHandler,Default_Handler

  @  .weak      USART2_IRQHandler
  @  .thumb_set USART2_IRQHandler,Default_Handler

  @  .weak      USART3_IRQHandler
  @  .thumb_set USART3_IRQHandler,Default_Handler

  @  .weak      EXINT15_10_IRQHandler
  @  .thumb_set EXINT15_10_IRQHandler,Default_Handler

  @  .weak      ERTCAlarm_IRQHandler
  @  .thumb_set ERTCAlarm_IRQHandler,Default_Handler

  @  .weak      OTGFS1_WKUP_IRQHandler
  @  .thumb_set OTGFS1_WKUP_IRQHandler,Default_Handler

  @  .weak      TMR8_BRK_TMR12_IRQHandler
  @  .thumb_set TMR8_BRK_TMR12_IRQHandler,Default_Handler

  @  .weak      TMR8_OVF_TMR13_IRQHandler
  @  .thumb_set TMR8_OVF_TMR13_IRQHandler,Default_Handler

  @  .weak      TMR8_TRG_HALL_TMR14_IRQHandler
  @  .thumb_set TMR8_TRG_HALL_TMR14_IRQHandler,Default_Handler

  @  .weak      TMR8_CH_IRQHandler
  @  .thumb_set TMR8_CH_IRQHandler,Default_Handler

  @  .weak      EDMA_Stream8_IRQHandler
  @  .thumb_set EDMA_Stream8_IRQHandler,Default_Handler

  @  .weak      XMC_IRQHandler
  @  .thumb_set XMC_IRQHandler,Default_Handler

  @  .weak      SDIO1_IRQHandler
  @  .thumb_set SDIO1_IRQHandler,Default_Handler

  @  .weak      TMR5_GLOBAL_IRQHandler
  @  .thumb_set TMR5_GLOBAL_IRQHandler,Default_Handler

  @  .weak      SPI3_I2S3EXT_IRQHandler
  @  .thumb_set SPI3_I2S3EXT_IRQHandler,Default_Handler

  @  .weak      UART4_IRQHandler
  @  .thumb_set UART4_IRQHandler,Default_Handler

  @  .weak      UART5_IRQHandler
  @  .thumb_set UART5_IRQHandler,Default_Handler

  @  .weak      TMR6_DAC_GLOBAL_IRQHandler
  @  .thumb_set TMR6_DAC_GLOBAL_IRQHandler,Default_Handler

  @  .weak      TMR7_GLOBAL_IRQHandler
  @  .thumb_set TMR7_GLOBAL_IRQHandler,Default_Handler

  @  .weak      DMA1_Channel1_IRQHandler
  @  .thumb_set DMA1_Channel1_IRQHandler,Default_Handler

  @  .weak      DMA1_Channel2_IRQHandler
  @  .thumb_set DMA1_Channel2_IRQHandler,Default_Handler

  @  .weak      DMA1_Channel3_IRQHandler
  @  .thumb_set DMA1_Channel3_IRQHandler,Default_Handler

  @  .weak      DMA1_Channel4_IRQHandler
  @  .thumb_set DMA1_Channel4_IRQHandler,Default_Handler

  @  .weak      DMA1_Channel5_IRQHandler
  @  .thumb_set DMA1_Channel5_IRQHandler,Default_Handler

  @  .weak      EMAC_IRQHandler
  @  .thumb_set EMAC_IRQHandler,Default_Handler

  @  .weak      EMAC_WKUP_IRQHandler
  @  .thumb_set EMAC_WKUP_IRQHandler,Default_Handler

  @  .weak      CAN2_TX_IRQHandler
  @  .thumb_set CAN2_TX_IRQHandler,Default_Handler

  @  .weak      CAN2_RX0_IRQHandler
  @  .thumb_set CAN2_RX0_IRQHandler ,Default_Handler

  @  .weak      CAN2_RX1_IRQHandler
  @  .thumb_set CAN2_RX1_IRQHandler ,Default_Handler

  @  .weak      CAN2_SE_IRQHandler
  @  .thumb_set CAN2_SE_IRQHandler,Default_Handler

  @  .weak      OTGFS1_IRQHandler
  @  .thumb_set OTGFS1_IRQHandler,Default_Handler

  @  .weak      DMA1_Channel6_IRQHandler
  @  .thumb_set DMA1_Channel6_IRQHandler,Default_Handler

  @  .weak      DMA1_Channel7_IRQHandler
  @  .thumb_set DMA1_Channel7_IRQHandler,Default_Handler

  @  .weak      USART6_IRQHandler
  @  .thumb_set USART6_IRQHandler,Default_Handler

  @  .weak      I2C3_EVT_IRQHandler
  @  .thumb_set I2C3_EVT_IRQHandler,Default_Handler

  @  .weak      I2C3_ERR_IRQHandler
  @  .thumb_set I2C3_ERR_IRQHandler,Default_Handler

  @  .weak      OTGFS2_WKUP_IRQHandler
  @  .thumb_set OTGFS2_WKUP_IRQHandler,Default_Handler

  @  .weak      OTGFS2_IRQHandler
  @  .thumb_set OTGFS2_IRQHandler,Default_Handler

  @  .weak      DVP_IRQHandler
  @  .thumb_set DVP_IRQHandler,Default_Handler

  @  .weak      FPU_IRQHandler
  @  .thumb_set FPU_IRQHandler,Default_Handler

  @  .weak      UART7_IRQHandler
  @  .thumb_set UART7_IRQHandler,Default_Handler

  @  .weak      UART8_IRQHandler
  @  .thumb_set UART8_IRQHandler,Default_Handler

  @  .weak      SPI4_IRQHandler
  @  .thumb_set SPI4_IRQHandler,Default_Handler

  @  .weak      QSPI2_IRQHandler
  @  .thumb_set QSPI2_IRQHandler,Default_Handler

  @  .weak      QSPI1_IRQHandler
  @  .thumb_set QSPI1_IRQHandler,Default_Handler

  @  .weak      DMAMUX_IRQHandler
  @  .thumb_set DMAMUX_IRQHandler ,Default_Handler

  @  .weak      SDIO2_IRQHandler
  @  .thumb_set SDIO2_IRQHandler ,Default_Handler

  @  .weak      ACC_IRQHandler
  @  .thumb_set ACC_IRQHandler,Default_Handler

  @  .weak      TMR20_BRK_IRQHandler
  @  .thumb_set TMR20_BRK_IRQHandler,Default_Handler

  @  .weak      TMR20_OVF_IRQHandler
  @  .thumb_set TMR20_OVF_IRQHandler,Default_Handler

  @  .weak      TMR20_TRG_HALL_IRQHandler
  @  .thumb_set TMR20_TRG_HALL_IRQHandler,Default_Handler

  @  .weak      TMR20_CH_IRQHandler
  @  .thumb_set TMR20_CH_IRQHandler,Default_Handler

  @  .weak      DMA2_Channel1_IRQHandler
  @  .thumb_set DMA2_Channel1_IRQHandler,Default_Handler

  @  .weak      DMA2_Channel2_IRQHandler
  @  .thumb_set DMA2_Channel2_IRQHandler,Default_Handler

  @  .weak      DMA2_Channel3_IRQHandler
  @  .thumb_set DMA2_Channel3_IRQHandler,Default_Handler

  @  .weak      DMA2_Channel4_IRQHandler
  @  .thumb_set DMA2_Channel4_IRQHandler,Default_Handler

  @  .weak      DMA2_Channel5_IRQHandler
  @  .thumb_set DMA2_Channel5_IRQHandler,Default_Handler

  @  .weak      DMA2_Channel6_IRQHandler
  @  .thumb_set DMA2_Channel6_IRQHandler,Default_Handler

  @  .weak      DMA2_Channel7_IRQHandler
  @  .thumb_set DMA2_Channel7_IRQHandler,Default_Handler
