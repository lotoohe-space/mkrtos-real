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
#include "stm32_sys.h"
#include "printk.h"
#include "mm_man.h"
#include "task.h"
#include "thread.h"
#include "map.h"
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
  printk("%s\n", __FUNCTION__);
  umword_t status;
  status = cpulock_lock();
  task_kill(thread_get_current_task());
  cpulock_set(status);
  /* Go to infinite loop when Hard Fault exception occurs */
  // while (1)
  // {
  // }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
  addr_t fault_addr = (addr_t)(SCB->MMFAR);
  task_t *cur_task = thread_get_current_task();
  umword_t status;

  // printk("%s\n", __FUNCTION__);
  if (SCB->CFSR & 0x1)
  {
    printk("指令访问错误\n");
  }
  if (SCB->CFSR & 2)
  {
    printk("数据访问错误\n");
  }
  if (SCB->CFSR & 128)
  {
    fault_addr = (addr_t)(SCB->MMFAR);
    // printk("%x\t", SCB->MMFAR);

    if (mm_page_alloc_fault(&cur_task->mm_space.mm_pages, fault_addr) == NULL)
    {
      printk("semgement fault.\n");
      /*TODO:杀死进程*/
      // while (1)
      // {
      // }
      goto end;
    }
    return;
  }
  if ((SCB->CFSR & 8))
  {
    printk("出栈错误\n");
    fault_addr = arch_get_user_sp();

    if (mm_page_alloc_fault(&cur_task->mm_space.mm_pages, fault_addr) == NULL)
    {
      printk("semgement fault.\n");
      /*TODO:杀死进程*/
      // while (1)
      // {
      // }
      goto end;
    }
    return;
  }
  if (SCB->CFSR & 16)
  {
    printk("压栈错误\n");
    fault_addr = arch_get_user_sp();

    if (mm_page_alloc_fault(&cur_task->mm_space.mm_pages, fault_addr) == NULL)
    {
      printk("semgement fault.\n");
      /*TODO:杀死进程*/
      goto end;
    }
    return;
  }
  if (SCB->CFSR & 32)
  {
    printk("浮点惰性压栈错误\n");
  }

  printk("semgement fault.\n");
  /*TODO:杀死进程*/
  // while (1)
  // {
  // }
end:
  status = cpulock_lock();
  task_kill(thread_get_current_task());
  cpulock_set(status);
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
  umword_t status;
  printk("%s\n", __FUNCTION__);

  /* Go to infinite loop when Bus Fault exception occurs */
  // while (1)
  // {
  // }
  status = cpulock_lock();
  task_kill(thread_get_current_task());
  cpulock_set(status);
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
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
  /* Go to infinite loop when Usage Fault exception occurs */
  // while (1)
  // {
  // }
  umword_t status;
  status = cpulock_lock();
  task_kill(thread_get_current_task());
  cpulock_set(status);
}

// /**
//   * @brief  This function handles SVCall exception.
//   * @param  None
//   * @retval None
//   */
// void SVC_Handler(void)
// {
// }

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
  printk("%s\n", __FUNCTION__);
}

// /**
//   * @brief  This function handles PendSVC exception.
//   * @param  None
//   * @retval None
//   */
// void PendSV_Handler(void)
// {
// }

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