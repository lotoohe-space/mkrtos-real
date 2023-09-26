/**
  ******************************************************************************
  * @file      startup_stm32f2xx.s
  * @author    MCD Application Team
  * @version   V1.1.3
  * @date      05-March-2012
  * @brief     STM32F2xx Devices vector table for RIDE7 toolchain. 
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address
  *                - Configure the system clock and the external SRAM mounted on 
  *                  STM322xG-EVAL board to be used as data memory (optional, 
  *                  to be enabled by user)
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M3 processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
    
  .syntax unified
  .cpu cortex-m3
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
/* Call the clock system intitialization function.*/
  bl  SystemInit   
/* Call the application's entry point.*/
  bl  _start
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
  .word  isr_handler
  
  /* External Interrupts */
  .word     isr_handler                   /* Window WatchDog              */                                        
  .word     isr_handler                    /* PVD through EXTI Line detection */                        
  .word     isr_handler             /* Tamper and TimeStamps through the EXTI line */            
  .word     isr_handler               /* RTC Wakeup through the EXTI line */                      
  .word     isr_handler                  /* FLASH                        */                                          
  .word     isr_handler                    /* RCC                          */                                            
  .word     isr_handler                  /* EXTI Line0                   */                        
  .word     isr_handler                  /* EXTI Line1                   */                          
  .word     isr_handler                  /* EXTI Line2                   */                          
  .word     isr_handler                  /* EXTI Line3                   */                          
  .word     isr_handler                  /* EXTI Line4                   */                          
  .word     isr_handler           /* DMA1 Stream 0                */                  
  .word     isr_handler           /* DMA1 Stream 1                */                   
  .word     isr_handler           /* DMA1 Stream 2                */                   
  .word     isr_handler           /* DMA1 Stream 3                */                   
  .word     isr_handler           /* DMA1 Stream 4                */                   
  .word     isr_handler           /* DMA1 Stream 5                */                   
  .word     isr_handler           /* DMA1 Stream 6                */                   
  .word     isr_handler                    /* ADC1, ADC2 and ADC3s         */                   
  .word     isr_handler                /* CAN1 TX                      */                         
  .word     isr_handler               /* CAN1 RX0                     */                          
  .word     isr_handler               /* CAN1 RX1                     */                          
  .word     isr_handler               /* CAN1 SCE                     */                          
  .word     isr_handler                /* External Line[9:5]s          */                          
  .word     isr_handler          /* TIM1 Break and TIM9          */         
  .word     isr_handler          /* TIM1 Update and TIM10        */         
  .word     isr_handler     /* TIM1 Trigger and Commutation and TIM11 */
  .word     isr_handler                /* TIM1 Capture Compare         */                          
  .word     isr_handler                   /* TIM2                         */                   
  .word     isr_handler                   /* TIM3                         */                   
  .word     isr_handler                   /* TIM4                         */                   
  .word     isr_handler                /* I2C1 Event                   */                          
  .word     isr_handler                /* I2C1 Error                   */                          
  .word     isr_handler                /* I2C2 Event                   */                          
  .word     isr_handler                /* I2C2 Error                   */                            
  .word     isr_handler                   /* SPI1                         */                   
  .word     isr_handler                   /* SPI2                         */                   
  .word     isr_handler                 /* USART1                       */                   
  .word     isr_handler                 /* USART2                       */                   
  .word     isr_handler                 /* USART3                       */                   
  .word     isr_handler              /* External Line[15:10]s        */                          
  .word     isr_handler              /* RTC Alarm (A and B) through EXTI Line */                 
  .word     isr_handler            /* USB OTG FS Wakeup through EXTI line */                       
  .word     isr_handler         /* TIM8 Break and TIM12         */         
  .word     isr_handler          /* TIM8 Update and TIM13        */         
  .word     isr_handler     /* TIM8 Trigger and Commutation and TIM14 */
  .word     isr_handler                /* TIM8 Capture Compare         */                          
  .word     isr_handler           /* DMA1 Stream7                 */                          
  .word     isr_handler                   /* FSMC                         */                   
  .word     isr_handler                   /* SDIO                         */                   
  .word     isr_handler                   /* TIM5                         */                   
  .word     isr_handler                   /* SPI3                         */                   
  .word     isr_handler                  /* UART4                        */                   
  .word     isr_handler                  /* UART5                        */                   
  .word     isr_handler               /* TIM6 and DAC1&2 underrun errors */                   
  .word     isr_handler                   /* TIM7                         */
  .word     isr_handler           /* DMA2 Stream 0                */                   
  .word     isr_handler           /* DMA2 Stream 1                */                   
  .word     isr_handler           /* DMA2 Stream 2                */                   
  .word     isr_handler           /* DMA2 Stream 3                */                   
  .word     isr_handler           /* DMA2 Stream 4                */                   
  .word     isr_handler                    /* Ethernet                     */                   
  .word     isr_handler               /* Ethernet Wakeup through EXTI line */                     
  .word     isr_handler                /* CAN2 TX                      */                          
  .word     isr_handler               /* CAN2 RX0                     */                          
  .word     isr_handler               /* CAN2 RX1                     */                          
  .word     isr_handler               /* CAN2 SCE                     */                          
  .word     isr_handler                 /* USB OTG FS                   */                   
  .word     isr_handler           /* DMA2 Stream 5                */                   
  .word     isr_handler           /* DMA2 Stream 6                */                   
  .word     isr_handler           /* DMA2 Stream 7                */                   
  .word     isr_handler                 /* USART6                       */                    
  .word     isr_handler                /* I2C3 event                   */                          
  .word     isr_handler                /* I2C3 error                   */                          
  .word     isr_handler         /* USB OTG HS End Point 1 Out   */                   
  .word     isr_handler          /* USB OTG HS End Point 1 In    */                   
  .word     isr_handler            /* USB OTG HS Wakeup through EXTI */                         
  .word     isr_handler                 /* USB OTG HS                   */                   
  .word     isr_handler                   /* DCMI                         */                   
  .word     isr_handler                   /* CRYP crypto                  */                   
  .word     isr_handler               /* Hash and Rng                 */
                         
                         
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
  
  .weak isr_handler
  .thumb_set isr_handler,Default_Handler

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
