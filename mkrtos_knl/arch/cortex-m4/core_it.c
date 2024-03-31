/**
 ******************************************************************************
 * @file    Project/STM32F2xx_StdPeriph_Template/stm32f2xx_it.c
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    13-April-2012
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "mk_sys.h"
#include "printk.h"
#include "mm_man.h"
#include "task.h"
#include "thread.h"
#include "map.h"
#include "thread_knl.h"
/** @addtogroup Template_Project
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
  printk("%s\n", __FUNCTION__);
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
  bool_t is_knl = is_run_knl();

  printk("%s\n", __FUNCTION__);
  task_knl_kill(thread_get_current(), is_knl);
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
  bool_t is_knl = is_run_knl();
  addr_t fault_addr = (addr_t)(SCB->MMFAR);
  task_t *cur_task = thread_get_current_task();
  umword_t status;

  // printk("%s\n", __FUNCTION__);
  if (SCB->CFSR & 0x1)
  {
    printk("instr 0x%x access is error.\n", fault_addr);
  }
  if (SCB->CFSR & 2)
  {
    printk("data 0x%x access is error.\n", fault_addr);
  }
  if (SCB->CFSR & 128)
  {
    fault_addr = (addr_t)(SCB->MMFAR);
    // if (mm_page_alloc_fault(&cur_task->mm_space.mm_pages, fault_addr) == NULL)
    // {
    //   goto end;
    // }
    // return;
    goto end;
  }
  if ((SCB->CFSR & 8))
  {
    printk("出栈错误\n");
    // fault_addr = arch_get_user_sp();

    // if (mm_page_alloc_fault(&cur_task->mm_space.mm_pages, fault_addr) == NULL)
    // {
    //   goto end;
    // }
    // return;
    goto end;
  }
  if (SCB->CFSR & 16)
  {
    printk("压栈错误\n");
    // fault_addr = arch_get_user_sp();

    // if (mm_page_alloc_fault(&cur_task->mm_space.mm_pages, fault_addr) == NULL)
    // {
    //   goto end;
    // }
    // return;
    goto end;
  }
  if (SCB->CFSR & 32)
  {
    printk("浮点惰性压栈错误\n");
  }

end:
  printk("task:0x%x, semgement fault.\n", thread_get_current_task());
  task_knl_kill(thread_get_current(), is_knl);
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
  bool_t is_knl = is_run_knl();

  printk("%s\n", __FUNCTION__);
  task_knl_kill(thread_get_current(), is_knl);
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
  bool_t is_knl = is_run_knl();
  printk("%s\n", __FUNCTION__);
  if (SCB->CFSR & (1 << 16))
  {
    printk("Undefined instruction error\n");
  }
  if (SCB->CFSR & (1 << 17))
  {
    printk("Trying to switch to the wrong state\n");
  }
  if (SCB->CFSR & (1 << 18))
  {
    printk("An exception attempting to execute an EXC_RETURN error\n");
  }
  if (SCB->CFSR & (1 << 19))
  {
    printk("An attempt was made to execute a coprocessor instruction\n");
  }
  if (SCB->CFSR & (1 << 24))
  {
    printk("A non-access error was generated\n");
  }
  if (SCB->CFSR & (1 << 25))
  {
    printk("Division by zero error\n");
  }
  task_knl_kill(thread_get_current(), is_knl);
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
  printk("%s\n", __FUNCTION__);
}

/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f2xx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
{
}*/

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
