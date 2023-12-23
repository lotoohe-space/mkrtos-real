/**
  ******************************************************************************
  * @file      startup_stm32f40xx.s
  * @author    MCD Application Team
  * @version   V1.8.1
  * @date      27-January-2022
  * @brief     STM32F40xxxx Devices vector table for RIDE7 toolchain.
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address
  *                - Configure the clock system and the external SRAM mounted on 
  *                  STM324xG-EVAL board to be used as data memory (optional, 
  *                  to be enabled by user)
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M4 processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
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
 * @retval : None
*/

    .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:  

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
  .word     entry_handler                   /* Window WatchDog              */                                        
  .word     entry_handler                    /* PVD through EXTI Line detection */                        
  .word     entry_handler             /* Tamper and TimeStamps through the EXTI line */            
  .word     entry_handler               /* RTC Wakeup through the EXTI line */                      
  .word     entry_handler                  /* FLASH                        */                                          
   .word     entry_handler  /* RCC                          */                                            
   .word     entry_handler  /* EXTI Line0                   */                        
   .word     entry_handler  /* EXTI Line1                   */                          
   .word     entry_handler  /* EXTI Line2                   */                          
   .word     entry_handler  /* EXTI Line3                   */                          
   .word     entry_handler  /* EXTI Line4                   */                          
   .word     entry_handler  /* DMA1 Stream 0                */                  
   .word     entry_handler  /* DMA1 Stream 1                */                   
   .word     entry_handler  /* DMA1 Stream 2                */                   
   .word     entry_handler  /* DMA1 Stream 3                */                   
   .word     entry_handler  /* DMA1 Stream 4                */                   
   .word     entry_handler  /* DMA1 Stream 5                */                   
   .word     entry_handler  /* DMA1 Stream 6                */                   
   .word     entry_handler  /* ADC1, ADC2 and ADC3s         */                   
   .word     entry_handler  /* CAN1 TX                      */                         
   .word     entry_handler  /* CAN1 RX0                     */                          
   .word     entry_handler  /* CAN1 RX1                     */                          
   .word     entry_handler  /* CAN1 SCE                     */                          
   .word     entry_handler  /* External Line[9:5]s          */                          
   .word     entry_handler  /* TIM1 Break and TIM9          */         
   .word     entry_handler  /* TIM1 Update and TIM10        */         
   .word     entry_handler  /* TIM1 Trigger and Commutation and TIM11 */
   .word     entry_handler  /* TIM1 Capture Compare         */                          
   .word     entry_handler  /* TIM2                         */                   
   .word     entry_handler  /* TIM3                         */                   
   .word     entry_handler  /* TIM4                         */                   
   .word     entry_handler  /* I2C1 Event                   */                          
   .word     entry_handler  /* I2C1 Error                   */                          
   .word     entry_handler  /* I2C2 Event                   */                          
   .word     entry_handler  /* I2C2 Error                   */                            
   .word     entry_handler  /* SPI1                         */                   
   .word     entry_handler  /* SPI2                         */                   
   .word     entry_handler  /* USART1                       */                   
   .word     entry_handler  /* USART2                       */                   
   .word     entry_handler  /* USART3                       */                   
   .word     entry_handler  /* External Line[15:10]s        */                          
   .word     entry_handler  /* RTC Alarm (A and B) through EXTI Line */                 
   .word     entry_handler  /* USB OTG FS Wakeup through EXTI line */                       
   .word     entry_handler  /* TIM8 Break and TIM12         */         
   .word     entry_handler  /* TIM8 Update and TIM13        */         
   .word     entry_handler  /* TIM8 Trigger and Commutation and TIM14 */
   .word     entry_handler  /* TIM8 Capture Compare         */                          
   .word     entry_handler  /* DMA1 Stream7                 */                          
   .word     entry_handler  /* FSMC                         */                   
   .word     entry_handler  /* SDIO                         */                   
   .word     entry_handler  /* TIM5                         */                   
   .word     entry_handler  /* SPI3                         */                   
   .word     entry_handler  /* UART4                        */                   
   .word     entry_handler  /* UART5                        */                   
   .word     entry_handler  /* TIM6 and DAC1&2 underrun errors */                   
   .word     entry_handler  /* TIM7                         */
   .word     entry_handler  /* DMA2 Stream 0                */                   
   .word     entry_handler  /* DMA2 Stream 1                */                   
   .word     entry_handler  /* DMA2 Stream 2                */                   
   .word     entry_handler  /* DMA2 Stream 3                */                   
   .word     entry_handler  /* DMA2 Stream 4                */                   
   .word     entry_handler  /* Ethernet                     */                   
   .word     entry_handler  /* Ethernet Wakeup through EXTI line */                     
   .word     entry_handler  /* CAN2 TX                      */                          
   .word     entry_handler  /* CAN2 RX0                     */                          
   .word     entry_handler  /* CAN2 RX1                     */                          
   .word     entry_handler  /* CAN2 SCE                     */                          
   .word     entry_handler  /* USB OTG FS                   */                   
   .word     entry_handler  /* DMA2 Stream 5                */                   
   .word     entry_handler  /* DMA2 Stream 6                */                   
   .word     entry_handler  /* DMA2 Stream 7                */                   
   .word     entry_handler  /* USART6                       */                    
   .word     entry_handler  /* I2C3 event                   */                          
   .word     entry_handler  /* I2C3 error                   */                          
   .word     entry_handler  /* USB OTG HS End Point 1 Out   */                   
   .word     entry_handler  /* USB OTG HS End Point 1 In    */                   
   .word     entry_handler  /* USB OTG HS Wakeup through EXTI */                         
   .word     entry_handler  /* USB OTG HS                   */                   
   .word     entry_handler  /* DCMI                         */                   
   .word     entry_handler  /* CRYP crypto                  */                   
   .word     entry_handler  /* Hash and Rng                 */
   .word     entry_handler  /* FPU                          */                         
                            
/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler. 
* As they are weak aliases, any function with the same name will override 
* this definition.
* 
*******************************************************************************/
  @  .weak      NMI_Handler
  @  .thumb_set NMI_Handler,Default_Handler
  
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
  
  @  .weak      WWDG_IRQHandler                   
  @  .thumb_set WWDG_IRQHandler,Default_Handler      
                  
  @  .weak      PVD_IRQHandler      
  @  .thumb_set PVD_IRQHandler,Default_Handler
               
  @  .weak      TAMP_STAMP_IRQHandler            
  @  .thumb_set TAMP_STAMP_IRQHandler,Default_Handler
            
  @  .weak      RTC_WKUP_IRQHandler                  
  @  .thumb_set RTC_WKUP_IRQHandler,Default_Handler
            
  @  .weak      FLASH_IRQHandler         
  @  .thumb_set FLASH_IRQHandler,Default_Handler
                  
  @  .weak      RCC_IRQHandler      
  @  .thumb_set RCC_IRQHandler,Default_Handler
                  
  @  .weak      EXTI0_IRQHandler         
  @  .thumb_set EXTI0_IRQHandler,Default_Handler
                  
  @  .weak      EXTI1_IRQHandler         
  @  .thumb_set EXTI1_IRQHandler,Default_Handler
                     
  @  .weak      EXTI2_IRQHandler         
  @  .thumb_set EXTI2_IRQHandler,Default_Handler 
                 
  @  .weak      EXTI3_IRQHandler         
  @  .thumb_set EXTI3_IRQHandler,Default_Handler
                        
  @  .weak      EXTI4_IRQHandler         
  @  .thumb_set EXTI4_IRQHandler,Default_Handler
                  
  @  .weak      DMA1_Stream0_IRQHandler               
  @  .thumb_set DMA1_Stream0_IRQHandler,Default_Handler
         
  @  .weak      DMA1_Stream1_IRQHandler               
  @  .thumb_set DMA1_Stream1_IRQHandler,Default_Handler
                  
  @  .weak      DMA1_Stream2_IRQHandler               
  @  .thumb_set DMA1_Stream2_IRQHandler,Default_Handler
                  
  @  .weak      DMA1_Stream3_IRQHandler               
  @  .thumb_set DMA1_Stream3_IRQHandler,Default_Handler 
                 
  @  .weak      DMA1_Stream4_IRQHandler              
  @  .thumb_set DMA1_Stream4_IRQHandler,Default_Handler
                  
  @  .weak      DMA1_Stream5_IRQHandler               
  @  .thumb_set DMA1_Stream5_IRQHandler,Default_Handler
                  
  @  .weak      DMA1_Stream6_IRQHandler               
  @  .thumb_set DMA1_Stream6_IRQHandler,Default_Handler
                  
  @  .weak      ADC_IRQHandler      
  @  .thumb_set ADC_IRQHandler,Default_Handler
               
  @  .weak      CAN1_TX_IRQHandler   
  @  .thumb_set CAN1_TX_IRQHandler,Default_Handler
            
  @  .weak      CAN1_RX0_IRQHandler                  
  @  .thumb_set CAN1_RX0_IRQHandler,Default_Handler
                           
  @  .weak      CAN1_RX1_IRQHandler                  
  @  .thumb_set CAN1_RX1_IRQHandler,Default_Handler
            
  @  .weak      CAN1_SCE_IRQHandler                  
  @  .thumb_set CAN1_SCE_IRQHandler,Default_Handler
            
  @  .weak      EXTI9_5_IRQHandler   
  @  .thumb_set EXTI9_5_IRQHandler,Default_Handler
            
  @  .weak      TIM1_BRK_TIM9_IRQHandler            
  @  .thumb_set TIM1_BRK_TIM9_IRQHandler,Default_Handler
            
  @  .weak      TIM1_UP_TIM10_IRQHandler            
  @  .thumb_set TIM1_UP_TIM10_IRQHandler,Default_Handler
      
  @  .weak      TIM1_TRG_COM_TIM11_IRQHandler      
  @  .thumb_set TIM1_TRG_COM_TIM11_IRQHandler,Default_Handler
      
  @  .weak      TIM1_CC_IRQHandler   
  @  .thumb_set TIM1_CC_IRQHandler,Default_Handler
                  
  @  .weak      TIM2_IRQHandler            
  @  .thumb_set TIM2_IRQHandler,Default_Handler
                  
  @  .weak      TIM3_IRQHandler            
  @  .thumb_set TIM3_IRQHandler,Default_Handler
                  
  @  .weak      TIM4_IRQHandler            
  @  .thumb_set TIM4_IRQHandler,Default_Handler
                  
  @  .weak      I2C1_EV_IRQHandler   
  @  .thumb_set I2C1_EV_IRQHandler,Default_Handler
                     
  @  .weak      I2C1_ER_IRQHandler   
  @  .thumb_set I2C1_ER_IRQHandler,Default_Handler
                     
  @  .weak      I2C2_EV_IRQHandler   
  @  .thumb_set I2C2_EV_IRQHandler,Default_Handler
                  
  @  .weak      I2C2_ER_IRQHandler   
  @  .thumb_set I2C2_ER_IRQHandler,Default_Handler
                           
  @  .weak      SPI1_IRQHandler            
  @  .thumb_set SPI1_IRQHandler,Default_Handler
                        
  @  .weak      SPI2_IRQHandler            
  @  .thumb_set SPI2_IRQHandler,Default_Handler
                  
  @  .weak      USART1_IRQHandler      
  @  .thumb_set USART1_IRQHandler,Default_Handler
                     
  @  .weak      USART2_IRQHandler      
  @  .thumb_set USART2_IRQHandler,Default_Handler
                     
  @  .weak      USART3_IRQHandler      
  @  .thumb_set USART3_IRQHandler,Default_Handler
                  
  @  .weak      EXTI15_10_IRQHandler               
  @  .thumb_set EXTI15_10_IRQHandler,Default_Handler
               
  @  .weak      RTC_Alarm_IRQHandler               
  @  .thumb_set RTC_Alarm_IRQHandler,Default_Handler
            
  @  .weak      OTG_FS_WKUP_IRQHandler         
  @  .thumb_set OTG_FS_WKUP_IRQHandler,Default_Handler
            
  @  .weak      TIM8_BRK_TIM12_IRQHandler         
  @  .thumb_set TIM8_BRK_TIM12_IRQHandler,Default_Handler
         
  @  .weak      TIM8_UP_TIM13_IRQHandler            
  @  .thumb_set TIM8_UP_TIM13_IRQHandler,Default_Handler
         
  @  .weak      TIM8_TRG_COM_TIM14_IRQHandler      
  @  .thumb_set TIM8_TRG_COM_TIM14_IRQHandler,Default_Handler
      
  @  .weak      TIM8_CC_IRQHandler   
  @  .thumb_set TIM8_CC_IRQHandler,Default_Handler
                  
  @  .weak      DMA1_Stream7_IRQHandler               
  @  .thumb_set DMA1_Stream7_IRQHandler,Default_Handler
                     
  @  .weak      FSMC_IRQHandler            
  @  .thumb_set FSMC_IRQHandler,Default_Handler
                     
  @  .weak      SDIO_IRQHandler            
  @  .thumb_set SDIO_IRQHandler,Default_Handler
                     
  @  .weak      TIM5_IRQHandler            
  @  .thumb_set TIM5_IRQHandler,Default_Handler
                     
  @  .weak      SPI3_IRQHandler            
  @  .thumb_set SPI3_IRQHandler,Default_Handler
                     
  @  .weak      UART4_IRQHandler         
  @  .thumb_set UART4_IRQHandler,Default_Handler
                  
  @  .weak      UART5_IRQHandler         
  @  .thumb_set UART5_IRQHandler,Default_Handler
                  
  @  .weak      TIM6_DAC_IRQHandler                  
  @  .thumb_set TIM6_DAC_IRQHandler,Default_Handler
               
  @  .weak      TIM7_IRQHandler            
  @  .thumb_set TIM7_IRQHandler,Default_Handler
         
  @  .weak      DMA2_Stream0_IRQHandler               
  @  .thumb_set DMA2_Stream0_IRQHandler,Default_Handler
               
  @  .weak      DMA2_Stream1_IRQHandler               
  @  .thumb_set DMA2_Stream1_IRQHandler,Default_Handler
                  
  @  .weak      DMA2_Stream2_IRQHandler               
  @  .thumb_set DMA2_Stream2_IRQHandler,Default_Handler
            
  @  .weak      DMA2_Stream3_IRQHandler               
  @  .thumb_set DMA2_Stream3_IRQHandler,Default_Handler
            
  @  .weak      DMA2_Stream4_IRQHandler               
  @  .thumb_set DMA2_Stream4_IRQHandler,Default_Handler
            
  @  .weak      ETH_IRQHandler      
  @  .thumb_set ETH_IRQHandler,Default_Handler
                  
  @  .weak      ETH_WKUP_IRQHandler                  
  @  .thumb_set ETH_WKUP_IRQHandler,Default_Handler
            
  @  .weak      CAN2_TX_IRQHandler   
  @  .thumb_set CAN2_TX_IRQHandler,Default_Handler
                           
  @  .weak      CAN2_RX0_IRQHandler                  
  @  .thumb_set CAN2_RX0_IRQHandler,Default_Handler
                           
  @  .weak      CAN2_RX1_IRQHandler                  
  @  .thumb_set CAN2_RX1_IRQHandler,Default_Handler
                           
  @  .weak      CAN2_SCE_IRQHandler                  
  @  .thumb_set CAN2_SCE_IRQHandler,Default_Handler
                           
  @  .weak      OTG_FS_IRQHandler      
  @  .thumb_set OTG_FS_IRQHandler,Default_Handler
                     
  @  .weak      DMA2_Stream5_IRQHandler               
  @  .thumb_set DMA2_Stream5_IRQHandler,Default_Handler
                  
  @  .weak      DMA2_Stream6_IRQHandler               
  @  .thumb_set DMA2_Stream6_IRQHandler,Default_Handler
                  
  @  .weak      DMA2_Stream7_IRQHandler               
  @  .thumb_set DMA2_Stream7_IRQHandler,Default_Handler
                  
  @  .weak      USART6_IRQHandler      
  @  .thumb_set USART6_IRQHandler,Default_Handler
                        
  @  .weak      I2C3_EV_IRQHandler   
  @  .thumb_set I2C3_EV_IRQHandler,Default_Handler
                        
  @  .weak      I2C3_ER_IRQHandler   
  @  .thumb_set I2C3_ER_IRQHandler,Default_Handler
                        
  @  .weak      OTG_HS_EP1_OUT_IRQHandler         
  @  .thumb_set OTG_HS_EP1_OUT_IRQHandler,Default_Handler
               
  @  .weak      OTG_HS_EP1_IN_IRQHandler            
  @  .thumb_set OTG_HS_EP1_IN_IRQHandler,Default_Handler
               
  @  .weak      OTG_HS_WKUP_IRQHandler         
  @  .thumb_set OTG_HS_WKUP_IRQHandler,Default_Handler
            
  @  .weak      OTG_HS_IRQHandler      
  @  .thumb_set OTG_HS_IRQHandler,Default_Handler
                  
  @  .weak      DCMI_IRQHandler            
  @  .thumb_set DCMI_IRQHandler,Default_Handler
                     
  @  .weak      CRYP_IRQHandler            
  @  .thumb_set CRYP_IRQHandler,Default_Handler
               
  @  .weak      HASH_RNG_IRQHandler                  
  @  .thumb_set HASH_RNG_IRQHandler,Default_Handler   

  @  .weak      FPU_IRQHandler                  
  @  .thumb_set FPU_IRQHandler,Default_Handler 


