/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"
#include "at32_emac.h"
#include "netconf.h"
#include "ip_addr.h"

/** @addtogroup AT32F437_periph_examples
  * @{
  */

/** @addtogroup 437_EMAC_wake_on_lan EMAC_wake_on_lan
  * @{
  */

#define DELAY                            100
#define FAST                             1
#define SLOW                             4

uint8_t g_speed = FAST;
volatile uint32_t local_time = 0;

void emac_wake_on_lan_init(void);

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  error_status status;

  system_clock_config();

  at32_board_init();
  
  uart_print_init(115200);
  
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  delay_init();

  status = emac_system_init();

  while(status == ERROR);

  tcpip_stack_init();

  emac_wake_on_lan_init();

  while(1)
  {
    /* lwip receive handle */
    lwip_rx_loop_handler();
    
    /*timeout handle*/
    lwip_periodic_handle(local_time);
  }
}

/**
  * @brief  wake on lan configuration, it will be woken by magic packet
  * @param  none
  * @retval none
  */
void emac_wake_on_lan_init(void)
{
  exint_init_type exint_init_structure;

  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(EMAC_WKUP_IRQn, 2, 0);

  emac_dma_operations_set(EMAC_DMA_OPS_START_STOP_TRANSMIT, FALSE);
  emac_trasmitter_enable(FALSE);

  emac_receiver_enable(FALSE);
  emac_dma_operations_set(EMAC_DMA_OPS_START_STOP_RECEIVE, FALSE);

  exint_init_structure.line_select = EXINT_LINE_19;
  exint_init_structure.line_enable = TRUE;
  exint_init_structure.line_mode = EXINT_LINE_INTERRUPT;
  exint_init_structure.line_polarity = EXINT_TRIGGER_BOTH_EDGE;
  exint_init(&exint_init_structure);

  emac_magic_packet_enable(TRUE);

  emac_power_down_set(TRUE);

  emac_receiver_enable(TRUE);

  SysTick->CTRL = FALSE;
  SysTick->VAL = FALSE;

  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
  exint_flag_clear(EXINT_LINE_19);

  pwc_voltage_regulate_set(PWC_REGULATOR_LOW_POWER);
  pwc_deep_sleep_mode_enter(PWC_DEEP_SLEEP_ENTER_WFI);
}

/**
  * @}
  */

/**
  * @}
  */
