
#include "mk_sys.h"
#include "printk.h"
#include "mm_man.h"
#include "task.h"
#include "thread.h"
#include "map.h"
#include "thread_knl.h"

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
  addr_t bus_addr = (addr_t)(SCB->BFAR);
  task_t *cur_task = thread_get_current_task();

  if ((SCB->CFSR & 0x1) || (SCB->CFSR & 0x2))
  {
    // printk("instr || data 0x%x access is error.\n", fault_addr);
    if (task_vma_page_fault(&(thread_get_current_task()->mm_space.mem_vma),
                            ALIGN_DOWN(fault_addr, PAGE_SIZE), NULL) < 0)
    {
      printk("task:0x%x, mem_addr:0x%lx bus_addr:0x%lx semgement fault.\n",
             thread_get_current_task(), fault_addr, bus_addr);
      task_knl_kill(thread_get_current(), is_knl);
    }
    return;
  }
  if ((SCB->CFSR & 8))
  {
    printk("push stack is error.\n");
  }
  if (SCB->CFSR & 16)
  {
    printk("pop stack is error.\n");
  }
  if (SCB->CFSR & 32)
  {
    printk("Floating point lazy stack error.\n");
  }
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

