/**
 ******************************************************************************
 * @file    stm32f4x7_eth_bsp.h
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    31-July-2013
 * @brief   Header for stm32f4x7_eth_bsp.c file.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4x7_ETH_BSP_H
#define __STM32F4x7_ETH_BSP_H

#ifdef __cplusplus
extern "C"
{
#endif

  /* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4x7_eth_conf.h"

#include "lwip/netif.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define DP83848_PHY_ADDRESS ((uint16_t)0x01)

/* Specific defines for EXTI line, used to manage Ethernet link status */
#define ETH_LINK_EXTI_LINE EXTI_Line14
#define ETH_LINK_EXTI_PORT_SOURCE EXTI_PortSourceGPIOB
#define ETH_LINK_EXTI_PIN_SOURCE EXTI_PinSource14
#define ETH_LINK_EXTI_IRQn EXTI15_10_IRQn
/* PB14 */
#define ETH_LINK_PIN GPIO_Pin_14
#define ETH_LINK_GPIO_PORT GPIOB
#define ETH_LINK_GPIO_CLK RCC_AHB1Periph_GPIOB

/* Ethernet Flags for EthStatus variable */
#define ETH_INIT_FLAG 0x01 /* Ethernet Init Flag */
#define ETH_LINK_FLAG 0x10 /* Ethernet Link Flag */

/* Uncomment SERIAL_DEBUG to enables retarget of printf to serial port (COM1 on STM32 evalboard)
   for debug purpose */

// #define MII_MODE

/* Uncomment the define below to clock the PHY from external 25MHz crystal (only for MII mode) */
#ifdef MII_MODE
#define PHY_CLOCK_MCO
#endif

  extern __IO uint32_t LocalTime;

  /* Exported macro ------------------------------------------------------------*/
  /* Exported functions ------------------------------------------------------- */
  void ETH_BSP_Config(void);
  void ETH_NVIC_Config(void);
  uint32_t Eth_Link_PHYITConfig(uint16_t PHYAddress);
  void Eth_Link_EXTIConfig(void);
  void Eth_Link_ITHandler(uint16_t PHYAddress);
  void ETH_link_callback(struct netif *netif);
  void Time_Update(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4x7_ETH_BSP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
