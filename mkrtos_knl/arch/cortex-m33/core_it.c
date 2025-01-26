
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
  SCB->MMFAR = 0;
  SCB->BFAR = 0;
  task_t *cur_task = thread_get_current_task();
  bool_t reset_r9 = FALSE;

  if (((SCB->CFSR & 0x1) || (SCB->CFSR & 0x2)) && (SCB->CFSR & (1 << 7)))
  {
    if (SCB->CFSR & 0x1)
    {
      SCB->CFSR |= 0x1;
    }
    if (SCB->CFSR & 0x2)
    {
      SCB->CFSR |= 0x2;
    }
    // printk("instr || data 0x%x access is error.\n", fault_addr);
    if (task_vma_page_fault(&(thread_get_current_task()->mm_space.mem_vma),
                            fault_addr, NULL) < 0)
    {
      printk("[semgement fault] task:0x%x, mem_addr:0x%lx bus_addr:0x%lx .\n",
             thread_get_current_task(), fault_addr, bus_addr);
      goto end;
    }
    return;
  }
  if ((SCB->CFSR & 8))
  {
    SCB->CFSR |= 8;
    printk("push stack is error.\n");
  }
  if (SCB->CFSR & 16)
  {
    SCB->CFSR |= 16;
    printk("pop stack is error.\n");
  }
  if (SCB->CFSR & 32)
  {
    SCB->CFSR |= 32;
    printk("Floating point lazy stack error.\n");
  }
end:
  reset_r9 = task_knl_kill(thread_get_current(), is_knl);
  if (reset_r9)
  {
    do
    {
      __asm__ __volatile__(
          "mov r9, %0\n\t"
          :                                                   /* 无输出操作数 */
          : "r"(thread_get_current_task()->mm_space.mm_block) // 输入操作数，将value的值传递给R0寄存器
          :                                                   // 告诉编译器R9寄存器将被修改
      );
    } while (0);
  }
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

