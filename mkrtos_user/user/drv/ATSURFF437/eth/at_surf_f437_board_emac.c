/**
 **************************************************************************
 * @file     at32_emac.c
 * @brief    emac config program
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

/* includes ------------------------------------------------------------------*/
#include "at_surf_f437_board_emac.h"
#include "u_sleep.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "mk_pin_drv.h"
#include "u_sleep.h"
#include "mk_pca9555_drv.h"
#include "u_irq_helper.h"
#include "cons_cli.h"
#include "u_sema.h"
#include "u_factory.h"
#include "u_task.h"
#define DEV_PIN_PATH "/pin"
#define DEV_PCA9555_PATH "/pca9555"

static int pin_fd;
static int pca9555_fd;
emac_control_config_type mac_control_para;

typedef struct
{
  u32 length;
  u32 buffer;
  emac_dma_desc_type *descriptor;
  emac_dma_desc_type *rx_fs_desc;
  emac_dma_desc_type *rx_ls_desc;
  uint32_t g_seg_count;
} FrameTypeDef;
extern emac_dma_desc_type *dma_tx_desc_to_set;
extern emac_dma_desc_type *dma_rx_desc_to_get;
#define EMAC_RXBUFNB 6
#define EMAC_TXBUFNB 6

FrameTypeDef rx_frame;
uint8_t MACaddr[6];
emac_dma_desc_type DMARxDscrTab[EMAC_RXBUFNB], DMATxDscrTab[EMAC_TXBUFNB];                            /* Ethernet Rx & Tx DMA Descriptors */
uint8_t Rx_Buff[EMAC_RXBUFNB][EMAC_MAX_PACKET_LENGTH], Tx_Buff[EMAC_TXBUFNB][EMAC_MAX_PACKET_LENGTH]; /* Ethernet buffers */
#define EMAC_DMARxDesc_FrameLengthShift 16

/**
 * @brief  enable emac clock and gpio clock
 * @param  none
 * @retval success or error
 */
error_status emac_system_init(void)
{
  error_status status;

again:
  pca9555_fd = open(DEV_PCA9555_PATH, O_RDWR, 0777);
  if (pca9555_fd < 0)
  {
    u_sleep_ms(50);
    goto again;
  }

  emac_nvic_configuration();

  /* emac periph clock enable */
  crm_periph_clock_enable(CRM_EMAC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_EMACTX_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_EMACRX_PERIPH_CLOCK, TRUE);

  emac_pins_configuration();

  status = emac_layer2_configuration();
#if 0
  emac_tmr_init();
#endif

  /* Initialize Tx Descriptors list: Chain Mode */
  emac_dma_descriptor_list_address_set(EMAC_DMA_TRANSMIT, DMATxDscrTab, &Tx_Buff[0][0], EMAC_TXBUFNB);
  /* Initialize Rx Descriptors list: Chain Mode  */
  emac_dma_descriptor_list_address_set(EMAC_DMA_RECEIVE, DMARxDscrTab, &Rx_Buff[0][0], EMAC_RXBUFNB);

  /* Enable Ethernet Rx interrrupt */
  for (int index = 0; index < EMAC_RXBUFNB; index++)
  {
    emac_dma_rx_desc_interrupt_config(&DMARxDscrTab[index], TRUE);
  }

#ifdef CHECKSUM_BY_HARDWARE
  for (index = 0; index < EMAC_TXBUFNB; index++)
  {
    DMATxDscrTab[index].status |= EMAC_DMATXDESC_CIC_TUI_FULL;
  }
#endif

  rx_frame.g_seg_count = 0;

  uint8_t mac_addr[6] = {
      0x2,
      0x0,
      0x0,
      0x0,
      0x0,
      0x2,
  };
  emac_local_address_set(mac_addr);

  /* Enable MAC and DMA transmission and reception */
  emac_speed_config(mac_control_para.auto_nego, mac_control_para.duplex_mode, mac_control_para.fast_ethernet_speed);

  u_sleep_ms(300);
  /* enable mac and dma transmission and reception */
  emac_start();

  return status;
}

static obj_handler_t sem_obj;
obj_handler_t emac_get_sema(void)
{
  return sem_obj;
}
static obj_handler_t irq_obj;
#define STACK_SIZE 1024
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
static uint8_t irq_msg_buf[512];
static void EMAC_IRQHandler(void)
{
  while (1)
  {
    msg_tag_t tag = uirq_wait(irq_obj, 0);
    if (msg_tag_get_val(tag) >= 0)
    {
      flag_status status;

      /* packet receiption */
      if (emac_dma_flag_get(EMAC_DMA_RI_FLAG) == SET)
      {
        /* a frame has been received */
        emac_dma_flag_clear(EMAC_DMA_RI_FLAG);
      }
      /* packet transmission */
      if (emac_dma_flag_get(EMAC_DMA_TI_FLAG) == SET)
      {
        emac_dma_flag_clear(EMAC_DMA_TI_FLAG);
      }
      emac_dma_flag_clear(EMAC_DMA_NIS_FLAG);
      emac_dma_flag_clear(EMAC_DMA_TI_FLAG);
      emac_dma_flag_clear(EMAC_DMA_RI_FLAG);
      u_sema_up(sem_obj);

      uirq_ack(irq_obj, EMAC_IRQn);
    }
  }
}

/**
 * @brief  configures emac irq channel.
 * @param  none
 * @retval none
 */
void emac_nvic_configuration(void)
{
  /*
  nvic_irq_enable(EMAC_IRQn, 1, 0);
  */
  int ret;

  msg_tag_t tag;
  obj_handler_t sema_hd;

  sem_obj = handler_alloc();
  if (sem_obj == HANDLER_INVALID)
  {
    return;
  }
  tag = facotry_create_sema(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, sem_obj), 0, (umword_t)(100000000));
  if (msg_tag_get_val(tag) < 0)
  {
    return;
  }
  ret = u_irq_request(EMAC_IRQn, (void *)(stack0 + STACK_SIZE), irq_msg_buf, &irq_obj, EMAC_IRQHandler, u_irq_prio_create(0, 0));
  if (ret < 0)
  {
    printf("irq request error.\n");
  }
}

typedef struct pin_cfg
{
  int pin;
  gpio_mux_sel_type mux_type;
} pin_cfg_t;

static const pin_cfg_t emac_pins[] = {
    {(2 * 16 + 1), GPIO_MUX_11},
    {(0 * 16 + 2), GPIO_MUX_11},
    {(1 * 16 + 11), GPIO_MUX_11},
    {(6 * 16 + 13), GPIO_MUX_11},
    {(6 * 16 + 14), GPIO_MUX_11},
    {(0 * 16 + 7), GPIO_MUX_11},
    {(2 * 16 + 4), GPIO_MUX_11},
    {(2 * 16 + 5), GPIO_MUX_11},
    {(0 * 16 + 1), GPIO_MUX_11},
};
/**
 * @brief  configures emac required pins.
 * @param  none
 * @retval none
 */
int emac_pins_configuration(void)
{
  int ret;
#if 0
  gpio_init_type gpio_init_struct = {0};

  /* emac pins clock enable */
  crm_periph_clock_enable(ETH_MDC_GPIO_CLK, TRUE);
  crm_periph_clock_enable(ETH_MDIO_GPIO_CLK, TRUE);
  crm_periph_clock_enable(ETH_RMII_TX_EN_GPIO_CLK, TRUE);
  crm_periph_clock_enable(ETH_RMII_TXD0_GPIO_CLK, TRUE);
  crm_periph_clock_enable(ETH_RMII_TXD1_GPIO_CLK, TRUE);
  crm_periph_clock_enable(ETH_RMII_CRS_DV_GPIO_CLK, TRUE);
  crm_periph_clock_enable(ETH_RMII_RXD0_GPIO_CLK, TRUE);
  crm_periph_clock_enable(ETH_RMII_RXD1_GPIO_CLK, TRUE);
  crm_periph_clock_enable(ETH_RMII_REF_CLK_GPIO_CLK, TRUE);
#endif
  pin_fd = open(DEV_PIN_PATH, O_RDWR, 0777);

  if (pin_fd < 0)
  {
    return pin_fd;
  }

  for (int i = 0; i < ARRAY_SIZE(emac_pins); i++)
  {
    ret = ioctl(pin_fd, MK_PIN_SET_MODE, ((mk_pin_cfg_t){
                                              .mode = MK_PIN_MODE_MUX_OUTPUT,
                                              .pin = emac_pins[i].pin,
                                              .cfg = emac_pins[i].mux_type,
                                          })
                                             .cfg_raw);
    if (ret < 0)
    {
      return ret;
    }
  }
#if 0 
  gpio_pin_mux_config(ETH_MDC_GPIO_PORT, ETH_MDC_GPIO_PINS_SOURCE, ETH_MDC_GPIO_MUX);
  gpio_pin_mux_config(ETH_MDIO_GPIO_PORT, ETH_MDIO_GPIO_PINS_SOURCE, ETH_MDIO_GPIO_MUX);
  gpio_pin_mux_config(ETH_RMII_TX_EN_GPIO_PORT, ETH_RMII_TX_EN_GPIO_PINS_SOURCE, ETH_RMII_TX_EN_GPIO_MUX);
  gpio_pin_mux_config(ETH_RMII_TXD0_GPIO_PORT, ETH_RMII_TXD0_GPIO_PINS_SOURCE, ETH_RMII_TXD0_GPIO_MUX);
  gpio_pin_mux_config(ETH_RMII_TXD1_GPIO_PORT, ETH_RMII_TXD1_GPIO_PINS_SOURCE, ETH_RMII_TXD1_GPIO_MUX);
  gpio_pin_mux_config(ETH_RMII_CRS_DV_GPIO_PORT, ETH_RMII_CRS_DV_GPIO_PINS_SOURCE, ETH_RMII_CRS_DV_GPIO_MUX);
  gpio_pin_mux_config(ETH_RMII_RXD0_GPIO_PORT, ETH_RMII_RXD0_GPIO_PINS_SOURCE, ETH_RMII_RXD0_GPIO_MUX);
  gpio_pin_mux_config(ETH_RMII_RXD1_GPIO_PORT, ETH_RMII_RXD1_GPIO_PINS_SOURCE, ETH_RMII_RXD1_GPIO_MUX);
  gpio_pin_mux_config(ETH_RMII_REF_CLK_GPIO_PORT, ETH_RMII_REF_CLK_GPIO_PINS_SOURCE, ETH_RMII_REF_CLK_GPIO_MUX);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

  gpio_init_struct.gpio_pins = ETH_MDC_GPIO_PIN;
  gpio_init(ETH_MDC_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = ETH_MDIO_GPIO_PIN;
  gpio_init(ETH_MDIO_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = ETH_RMII_TX_EN_GPIO_PIN;
  gpio_init(ETH_RMII_TX_EN_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = ETH_RMII_TXD0_GPIO_PIN;
  gpio_init(ETH_RMII_TXD0_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = ETH_RMII_TXD1_GPIO_PIN;
  gpio_init(ETH_RMII_TXD1_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = ETH_RMII_CRS_DV_GPIO_PIN;
  gpio_init(ETH_RMII_CRS_DV_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = ETH_RMII_RXD0_GPIO_PIN;
  gpio_init(ETH_RMII_RXD0_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = ETH_RMII_RXD1_GPIO_PIN;
  gpio_init(ETH_RMII_RXD1_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = ETH_RMII_REF_CLK_GPIO_PIN;
  gpio_init(ETH_RMII_REF_CLK_GPIO_PORT, &gpio_init_struct);
#endif
#if !CRYSTAL_ON_PHY
  crm_periph_clock_enable(ETH_INPUT_CLK_GPIO_CLK, TRUE);
#if 0
  gpio_pin_mux_config(ETH_INPUT_CLK_GPIO_PORT, ETH_INPUT_CLK_GPIO_PINS_SOURCE, ETH_INPUT_CLK_GPIO_MUX);

  gpio_init_struct.gpio_pins = ETH_INPUT_CLK_GPIO_PIN;
  gpio_init(ETH_INPUT_CLK_GPIO_PORT, &gpio_init_struct);
#endif
  ret = ioctl(pin_fd, MK_PIN_SET_MODE, ((mk_pin_cfg_t){
                                            .mode = MK_PIN_MODE_MUX_OUTPUT,
                                            .pin = (0 * 16) + 8,
                                            .cfg = GPIO_MUX_0,
                                        })
                                           .cfg_raw);
  if (ret < 0)
  {
    return ret;
  }
#endif

  return 0;
}

/**
 * @brief  configures emac layer2
 * @param  none
 * @retval error or success
 */
error_status emac_layer2_configuration(void)
{
  emac_dma_config_type dma_control_para;
  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);

  scfg_emac_interface_set(SCFG_EMAC_SELECT_RMII);

  crm_clock_out1_set(CRM_CLKOUT1_PLL);
  crm_clkout_div_set(CRM_CLKOUT_INDEX_1, CRM_CLKOUT_DIV1_5, CRM_CLKOUT_DIV2_2);

  /* reset phy */
  reset_phy();

  /* reset emac ahb bus */
  emac_reset();

  /* software reset emac dma */
  emac_dma_software_reset_set();

  while (emac_dma_software_reset_get() == SET)
  {
    u_sleep_ms(10);
  }

  emac_control_para_init(&mac_control_para);

  mac_control_para.auto_nego = EMAC_AUTO_NEGOTIATION_ON;
#ifdef CHECKSUM_BY_HARDWARE
  mac_control_para.ipv4_checksum_offload = TRUE;
#else
  mac_control_para.ipv4_checksum_offload = FALSE;
#endif

  if (emac_phy_init(&mac_control_para) == ERROR)
  {
    return ERROR;
  }

  emac_dma_para_init(&dma_control_para);

  dma_control_para.rsf_enable = TRUE;
  dma_control_para.tsf_enable = TRUE;
  dma_control_para.osf_enable = TRUE;
  dma_control_para.aab_enable = TRUE;
  dma_control_para.usp_enable = TRUE;
  dma_control_para.fb_enable = TRUE;
  dma_control_para.flush_rx_disable = TRUE;
  dma_control_para.rx_dma_pal = EMAC_DMA_PBL_32;
  dma_control_para.tx_dma_pal = EMAC_DMA_PBL_32;
  dma_control_para.priority_ratio = EMAC_DMA_2_RX_1_TX;

  emac_dma_config(&dma_control_para);
  emac_dma_interrupt_enable(EMAC_DMA_INTERRUPT_NORMAL_SUMMARY, TRUE);
  emac_dma_interrupt_enable(EMAC_DMA_INTERRUPT_RX, TRUE);

  return SUCCESS;
}

/**
 * @brief  reset layer 1
 * @param  none
 * @retval none
 */
void static reset_phy(void)
{
  emac_phy_power_on();

  ETH_RESET_LOW();

  u_sleep_ms(2);

  ETH_RESET_HIGH();

  u_sleep_ms(2);
}

/**
 * @brief  reset phy register
 * @param  none
 * @retval SUCCESS or ERROR
 */
error_status emac_phy_register_reset(void)
{
  uint16_t data = 0;
  uint32_t timeout = 0;
  uint32_t i = 0;

  if (emac_phy_register_write(PHY_ADDRESS, PHY_CONTROL_REG, PHY_RESET_BIT) == ERROR)
  {
    return ERROR;
  }

  for (i = 0; i < 0x000FFFFF; i++)
    ;

  do
  {
    timeout++;
    if (emac_phy_register_read(PHY_ADDRESS, PHY_CONTROL_REG, &data) == ERROR)
    {
      return ERROR;
    }
  } while ((data & PHY_RESET_BIT) && (timeout < PHY_TIMEOUT));

  for (i = 0; i < 0x00FFFFF; i++)
    ;
  if (timeout == PHY_TIMEOUT)
  {
    return ERROR;
  }
  return SUCCESS;
}

/**
 * @brief  set mac speed related parameters
 * @param  nego: auto negotiation on or off.
 *         this parameter can be one of the following values:
 *         - EMAC_AUTO_NEGOTIATION_OFF
 *         - EMAC_AUTO_NEGOTIATION_ON.
 * @param  mode: half-duplex or full-duplex.
 *         this parameter can be one of the following values:
 *         - EMAC_HALF_DUPLEX
 *         - EMAC_FULL_DUPLEX.
 * @param  speed: 10 mbps or 100 mbps
 *         this parameter can be one of the following values:
 *         - EMAC_SPEED_10MBPS
 *         - EMAC_SPEED_100MBPS.
 * @retval none
 */
error_status emac_speed_config(emac_auto_negotiation_type nego, emac_duplex_type mode, emac_speed_type speed)
{
  uint16_t data = 0;
  uint32_t timeout = 0;
  if (nego == EMAC_AUTO_NEGOTIATION_ON)
  {
    do
    {
      timeout++;
      if (emac_phy_register_read(PHY_ADDRESS, PHY_STATUS_REG, &data) == ERROR)
      {
        return ERROR;
      }
      u_sleep_ms(10);
    } while (!(data & PHY_LINKED_STATUS_BIT) && (timeout < 100));

    if (timeout == PHY_TIMEOUT)
    {
      return ERROR;
    }

    timeout = 0;

    if (emac_phy_register_write(PHY_ADDRESS, PHY_CONTROL_REG, PHY_AUTO_NEGOTIATION_BIT) == ERROR)
    {
      return ERROR;
    }

    do
    {
      timeout++;
      if (emac_phy_register_read(PHY_ADDRESS, PHY_STATUS_REG, &data) == ERROR)
      {
        return ERROR;
      }
      u_sleep_ms(10);
    } while (!(data & PHY_NEGO_COMPLETE_BIT) && (timeout < 100));

    if (timeout == PHY_TIMEOUT)
    {
      return ERROR;
    }

    if (emac_phy_register_read(PHY_ADDRESS, PHY_SPECIFIED_CS_REG, &data) == ERROR)
    {
      return ERROR;
    }
#ifdef DM9162
    if (data & PHY_FULL_DUPLEX_100MBPS_BIT)
    {
      emac_fast_speed_set(EMAC_SPEED_100MBPS);
      emac_duplex_mode_set(EMAC_FULL_DUPLEX);
    }
    else if (data & PHY_HALF_DUPLEX_100MBPS_BIT)
    {
      emac_fast_speed_set(EMAC_SPEED_100MBPS);
      emac_duplex_mode_set(EMAC_HALF_DUPLEX);
    }
    else if (data & PHY_FULL_DUPLEX_10MBPS_BIT)
    {
      emac_fast_speed_set(EMAC_SPEED_10MBPS);
      emac_duplex_mode_set(EMAC_FULL_DUPLEX);
    }
    else if (data & PHY_HALF_DUPLEX_10MBPS_BIT)
    {
      emac_fast_speed_set(EMAC_SPEED_10MBPS);
      emac_duplex_mode_set(EMAC_HALF_DUPLEX);
    }
#else
    if (data & PHY_DUPLEX_MODE)
    {
      emac_duplex_mode_set(EMAC_FULL_DUPLEX);
    }
    else
    {
      emac_duplex_mode_set(EMAC_HALF_DUPLEX);
    }
    if (data & PHY_SPEED_MODE)
    {
      emac_fast_speed_set(EMAC_SPEED_10MBPS);
    }
    else
    {
      emac_fast_speed_set(EMAC_SPEED_100MBPS);
    }
#endif
  }
  else
  {
    if (emac_phy_register_write(PHY_ADDRESS, PHY_CONTROL_REG, (uint16_t)((mode << 8) | (speed << 13))) == ERROR)
    {
      return ERROR;
    }
    if (speed == EMAC_SPEED_100MBPS)
    {
      emac_fast_speed_set(EMAC_SPEED_100MBPS);
    }
    else
    {
      emac_fast_speed_set(EMAC_SPEED_10MBPS);
    }
    if (mode == EMAC_FULL_DUPLEX)
    {
      emac_duplex_mode_set(EMAC_FULL_DUPLEX);
    }
    else
    {
      emac_duplex_mode_set(EMAC_HALF_DUPLEX);
    }
  }

  return SUCCESS;
}

/**
 * @brief  initialize emac phy
 * @param  none
 * @retval SUCCESS or ERROR
 */
error_status emac_phy_init(emac_control_config_type *control_para)
{
  emac_clock_range_set();
  if (emac_phy_register_reset() == ERROR)
  {
    return ERROR;
  }

  emac_control_config(control_para);
  return SUCCESS;
}

/**
 * @brief  power on
 * @param  none
 * @retval none
 */
void emac_phy_power_on(void)
{
  // pca9555_out_mode_config(ETH_POWER_ON_PIN);

  // pca9555_bits_reset(ETH_POWER_ON_PIN);
  ioctl(pca9555_fd, PCA9555_SET_OUTPUT_MODE, ETH_POWER_ON_PIN);
  ioctl(pca9555_fd, PCA9555_IO_RESET, ETH_POWER_ON_PIN);
}

/**
 * @brief  power off
 * @param  none
 * @retval none
 */
void emac_phy_power_off(void)
{
  // pca9555_out_mode_config(ETH_POWER_ON_PIN);

  // pca9555_bits_set(ETH_POWER_ON_PIN);
  ioctl(pca9555_fd, PCA9555_SET_OUTPUT_MODE, ETH_POWER_ON_PIN);
  ioctl(pca9555_fd, PCA9555_IO_SET, ETH_POWER_ON_PIN);
}

/**
 * @brief  updates the link states
 * @param  none
 * @retval link state 0: disconnect, 1: connection
 */
uint16_t link_update(void)
{
  uint16_t link_data, link_state;
  if (emac_phy_register_read(PHY_ADDRESS, PHY_STATUS_REG, &link_data) == ERROR)
  {
    return ERROR;
  }

  link_state = (link_data & PHY_LINKED_STATUS_BIT) >> 2;
  return link_state;
}
#if 0
/**
 * @brief  this function sets the netif link status.
 * @param  netif: the network interface
 * @retval none
 */
void ethernetif_set_link(void const *argument)
{
  uint16_t regvalue = 0;
  struct netif *netif = (struct netif *)argument;

  /* read phy_bsr*/
  regvalue = link_update();

  /* check whether the netif link down and the phy link is up */
  if (!netif_is_link_up(netif) && (regvalue))
  {
    /* network cable is connected */
    netif_set_link_up(netif);
  }
  else if (netif_is_link_up(netif) && (!regvalue))
  {
    /* network cable is dis-connected */
    netif_set_link_down(netif);
  }
}
/**
 * @brief  this function notify user about link status changement.
 * @param  netif: the network interface
 * @retval none
 */
void ethernetif_notify_conn_changed(struct netif *netif)
{
  /* note : this is function could be implemented in user file
            when the callback is needed,
  */

  if (netif_is_link_up(netif))
  {
    netif_set_up(netif);

#if LWIP_DHCP
    /*  creates a new dhcp client for this interface on the first call.
    note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
    the predefined regular intervals after starting the client.
    you can peek in the netif->dhcp struct for the actual dhcp status.*/
    dhcp_start(netif);
#endif
  }
  else
    netif_set_down(netif);
}

/**
 * @brief  link callback function, this function is called on change of link status
 *         to update low level driver configuration.
 * @param  netif: the network interface
 * @retval none
 */
void ethernetif_update_config(struct netif *netif)
{
  if (netif_is_link_up(netif))
  {
    emac_speed_config(mac_control_para.auto_nego, mac_control_para.duplex_mode, mac_control_para.fast_ethernet_speed);

    u_sleep_ms(300);
    /* enable mac and dma transmission and reception */
    emac_start();
  }
  else
  {
    /* disable mac and dma transmission and reception */
    emac_stop();
  }

  ethernetif_notify_conn_changed(netif);
}
#endif

/*******************************************************************************
 * Function Name  : emac_txpkt_chainmode
 * Description    : Transmits a packet, from application buffer, pointed by ppkt.
 * Input          : - FrameLength: Tx Packet size.
 * Output         : None
 * Return         : ERROR: in case of Tx desc owned by DMA
 *                  SUCCESS: for correct transmission
 *******************************************************************************/
error_status emac_txpkt_chainmode(uint32_t FrameLength)
{
  uint32_t buf_cnt = 0, index = 0;

  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if ((dma_tx_desc_to_set->status & EMAC_DMATXDESC_OWN) != (u32)RESET)
  {
    /* Return ERROR: OWN bit set */
    return ERROR;
  }

  if (FrameLength == 0)
  {
    return ERROR;
  }

  if (FrameLength > EMAC_MAX_PACKET_LENGTH)
  {
    buf_cnt = FrameLength / EMAC_MAX_PACKET_LENGTH;
    if (FrameLength % EMAC_MAX_PACKET_LENGTH)
    {
      buf_cnt += 1;
    }
  }
  else
  {
    buf_cnt = 1;
  }

  if (buf_cnt == 1)
  {
    /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */
    dma_tx_desc_to_set->status |= EMAC_DMATXDESC_LS | EMAC_DMATXDESC_FS;

    /* Setting the Frame Length: bits[12:0] */
    dma_tx_desc_to_set->controlsize = (FrameLength & EMAC_DMATXDESC_TBS1);

    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    dma_tx_desc_to_set->status |= EMAC_DMATXDESC_OWN;

    /* Selects the next DMA Tx descriptor list for next buffer to send */
    dma_tx_desc_to_set = (emac_dma_desc_type *)(dma_tx_desc_to_set->buf2nextdescaddr);
  }
  else
  {
    for (index = 0; index < buf_cnt; index++)
    {
      /* clear first and last segments */
      dma_tx_desc_to_set->status &= ~(EMAC_DMATXDESC_LS | EMAC_DMATXDESC_FS);

      /* set first segments */
      if (index == 0)
      {
        dma_tx_desc_to_set->status |= EMAC_DMATXDESC_FS;
      }

      /* set size */
      dma_tx_desc_to_set->controlsize = (EMAC_MAX_PACKET_LENGTH & EMAC_DMATXDESC_TBS1);

      /* set last segments */
      if (index == (buf_cnt - 1))
      {
        dma_tx_desc_to_set->status |= EMAC_DMATXDESC_LS;
        dma_tx_desc_to_set->controlsize = ((FrameLength - ((buf_cnt - 1) * EMAC_MAX_PACKET_LENGTH)) & EMAC_DMATXDESC_TBS1);
      }

      /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
      dma_tx_desc_to_set->status |= EMAC_DMATXDESC_OWN;

      /* Selects the next DMA Tx descriptor list for next buffer to send */
      dma_tx_desc_to_set = (emac_dma_desc_type *)(dma_tx_desc_to_set->buf2nextdescaddr);
    }
  }

  /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
  if (emac_dma_flag_get(EMAC_DMA_TBU_FLAG))
  {
    /* Clear TBUS ETHERNET DMA flag */
    emac_dma_flag_clear(EMAC_DMA_TBU_FLAG);
    /* Resume DMA transmission*/
    EMAC_DMA->tpd_bit.tpd = 0;
  }

  return SUCCESS;
}
/*******************************************************************************
 * Function Name  : emac_getcurrenttxbuffer
 * Description    : Return the address of the buffer pointed by the current descritor.
 * Input          : None
 * Output         : None
 * Return         : Buffer address
 *******************************************************************************/
u32 emac_getcurrenttxbuffer(void)
{
  /* Return Buffer address */
  return (dma_tx_desc_to_set->buf1addr);
}

int emac_send_packet(void *data, int len)
{
  int err = 0;
  struct pbuf *q;
  emac_dma_desc_type *dma_tx_desc;
  uint8_t *buffer;
  uint32_t length = 0;

  dma_tx_desc = dma_tx_desc_to_set;
  buffer = (uint8_t *)emac_getcurrenttxbuffer();

  /* copy data to buffer */
  if ((dma_tx_desc->status & EMAC_DMATXDESC_OWN) != RESET)
  {
    err = -EIO;
    goto out_error;
  }
  memcpy(buffer, (uint8_t *)data, len);
  emac_txpkt_chainmode(len);
  err = len;
out_error:
  /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
  if (emac_dma_flag_get(EMAC_DMA_TBU_FLAG))
  {
    /* Clear TBUS ETHERNET DMA flag */
    emac_dma_flag_clear(EMAC_DMA_TBU_FLAG);
    /* Resume DMA transmission*/
    EMAC_DMA->tpd_bit.tpd = 0;
  }

  return err;
}
/*******************************************************************************
 * Function Name  : emac_rxpkt_chainmode
 * Description    : Receives a packet.
 * Input          : None
 * Output         : None
 * Return         : ERROR: in case of Tx desc owned by DMA
 *                  SUCCESS: for correct transmission
 *******************************************************************************/
error_status emac_rxpkt_chainmode(void)
{
  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if ((dma_rx_desc_to_get->status & EMAC_DMARXDESC_OWN) != (u32)RESET)
  {
    /* return error: own bit set */
    return ERROR;
  }
  if ((dma_rx_desc_to_get->status & EMAC_DMARXDESC_LS) != (u32)RESET)
  {
    rx_frame.g_seg_count++;
    if (rx_frame.g_seg_count == 1)
    {
      rx_frame.rx_fs_desc = dma_rx_desc_to_get;
    }
    rx_frame.rx_ls_desc = dma_rx_desc_to_get;
    rx_frame.length = ((dma_rx_desc_to_get->status & EMAC_DMARXDESC_FL) >> EMAC_DMARxDesc_FrameLengthShift) - 4;
    rx_frame.buffer = rx_frame.rx_fs_desc->buf1addr;

    /* Selects the next DMA Rx descriptor list for next buffer to read */
    dma_rx_desc_to_get = (emac_dma_desc_type *)(dma_rx_desc_to_get->buf2nextdescaddr);

    return SUCCESS;
  }
  else if ((dma_rx_desc_to_get->status & EMAC_DMARXDESC_FS) != (u32)RESET)
  {
    rx_frame.g_seg_count = 1;
    rx_frame.rx_fs_desc = dma_rx_desc_to_get;
    rx_frame.rx_ls_desc = NULL;
    dma_rx_desc_to_get = (emac_dma_desc_type *)(dma_rx_desc_to_get->buf2nextdescaddr);
  }
  else
  {
    rx_frame.g_seg_count++;
    dma_rx_desc_to_get = (emac_dma_desc_type *)(dma_rx_desc_to_get->buf2nextdescaddr);
  }

  return ERROR;
}

int emac_read_packet(void *data, int buf_len)
{
  uint32_t len = 0;
  emac_dma_desc_type *dma_rx_desc;
  uint8_t *buffer;

  if (emac_rxpkt_chainmode() != SUCCESS)
  {
    return -EIO;
  }

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  len = rx_frame.length;
  buffer = (uint8_t *)rx_frame.buffer;

  memcpy((uint8_t *)data, (uint8_t *)buffer, MIN(len, buf_len));

  dma_rx_desc = rx_frame.rx_fs_desc;
  for (int index = 0; index < rx_frame.g_seg_count; index++)
  {
    dma_rx_desc->status |= EMAC_DMARXDESC_OWN;
    dma_rx_desc = (emac_dma_desc_type *)(dma_rx_desc->buf2nextdescaddr);
  }

  rx_frame.g_seg_count = 0;

  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
  if (emac_dma_flag_get(EMAC_DMA_RBU_FLAG))
  {
    /* Clear RBUS ETHERNET DMA flag */
    emac_dma_flag_clear(EMAC_DMA_RBU_FLAG);
    /* Resume DMA reception */
    EMAC_DMA->rpd_bit.rpd = FALSE;
  }

  return MIN(len, buf_len);
}
/**
 * @brief  initialize tmr6 for emac
 * @param  none
 * @retval none
 */
void emac_tmr_init(void)
{
  crm_clocks_freq_type crm_clocks_freq_struct = {0};
  crm_periph_clock_enable(CRM_TMR6_PERIPH_CLOCK, TRUE);

  crm_clocks_freq_get(&crm_clocks_freq_struct);
  /* tmr1 configuration */
  /* time base configuration */
  /* systemclock/24000/100 = 100hz */
  tmr_base_init(TMR6, 99, (crm_clocks_freq_struct.ahb_freq / 10000) - 1);
  tmr_cnt_dir_set(TMR6, TMR_COUNT_UP);

  /* overflow interrupt enable */
  tmr_interrupt_enable(TMR6, TMR_OVF_INT, TRUE);

  /* tmr1 overflow interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR6_DAC_GLOBAL_IRQn, 0, 0);
  tmr_counter_enable(TMR6, TRUE);
}
