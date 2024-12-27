/**
 **************************************************************************
 * @file     at_surf_f437_board_audio.c
 * @brief     the driver library of the audio.
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

#include "at_surf_f437_board_audio.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "mk_pca9555_drv.h"
#include "u_irq_helper.h"
#include "mk_i2c_drv.h"
#include "u_sleep.h"
#include "u_sys.h"
#include "mk_pin_drv.h"

/**
 * @brief  wm8988 register default value
 */
uint16_t reg_addr_data[] =
    {
        (WM8988_R0_LEFT_INPUT_VOLUME << 9) | 0x012F,  /*Left Input Channel Volume*/
        (WM8988_R1_RIGHT_INPUT_VOLUME << 9) | 0x012F, /*Right Input Channel Volume*/
        (WM8988_R2_LOUT1_VOLUME << 9) | 0x0179,       /*Left Output Channel Volume*/
        (WM8988_R3_ROUT1_VOLUME << 9) | 0x0179,       /*Right Output Channel Volume*/
        (WM8988_R5_ADC_DAC_CONTROL << 9) | 0x0006,    /*De-emphasis Control and Digital soft mute*/
        (WM8988_REG_BITW16),
        (WM8988_REG_FREQ_48_K),
        (WM8988_R10_LEFT_DAC_VOLUME << 9) | 0x01FF,      /*Left Digital DAC Volume Control*/
        (WM8988_R11_RIGHT_DAC_VOLUME << 9) | 0x01FF,     /*Right Digital DAC Volume Control*/
        (WM8988_R12_BASS_CONTROL << 9) | 0x000F,         /*Bass Control*/
        (WM8988_R13_TREBLE_CONTROL << 9) | 0x000F,       /*Treble Control*/
        (WM8988_R16_3D_CONTROL << 9) | 0x0000,           /*3D stereo Enhancment*/
        (WM8988_R21_LEFT_ADC_VOLUME << 9) | 0x01C3,      /*Left ADC Digital Volume*/
        (WM8988_R22_RIGHT_ADC_VOLUME << 9) | 0x01C3,     /*Right ADC Digital Volume*/
        (WM8988_R23_ADDITIONAL_1_CONTROL << 9) | 0x00C2, /*Additional Control 1*/
        (WM8988_R24_ADDITIONAL_2_CONTROL << 9) | 0x0000, /*Additional Control 2*/
        (WM8988_R27_ADDITIONAL_3_CONTROL << 9) | 0x0000, /*Additional Control 3*/
        (WM8988_R31_ADC_INPUT_MODE << 9) | 0x0000,       /*ADC input mode*/
                                                         //  (WM8988_R32_ADC_L_SIGNAL_PATH << 9) |         0x0040,                /*ADC Signal Path Control left LINPUT2*/
                                                         //  (WM8988_R33_ADC_R_SIGNAL_PATH << 9) |         0x0040,                /*ADC Signal Path Control right RINPUT2*/
        (WM8988_R32_ADC_L_SIGNAL_PATH << 9) | 0x0000,    /*ADC Signal Path Control left  LINPUT1 */
        (WM8988_R33_ADC_R_SIGNAL_PATH << 9) | 0x0000,    /*ADC Signal Path Control right RINPUT1*/
        (WM8988_R34_LEFT_OUT_MIX_1 << 9) | 0x0152,       /*Left DAC mixer Control*/
        (WM8988_R35_LEFT_OUT_MIX_2 << 9) | 0x0050,       /*Left DAC mixer Control*/
        (WM8988_R36_RIGHT_OUT_MIX_1 << 9) | 0x0052,      /*Right DAC mixer Control*/
        (WM8988_R37_RIGHT_OUT_MIX_2 << 9) | 0x0150,      /*Right DAC mixer Control*/
        (WM8988_R40_LOUT2_VOLUME << 9) | 0x01FF,         /*Output left channel volume*/
        (WM8988_R41_ROUT2_VOLUME << 9) | 0x01FF,         /*Output left channel volume*/
        (WM8988_R43_LOW_POWER_PALYBACK << 9) | 0x0008,   /*Output left channel volume*/
        (WM8988_R25_PWR_1_MGMT << 9) | 0x017C,           /*Power Management1*/
        (WM8988_R26_PWR_2_MGMT << 9) | 0x01F8,           /*Power Management2*/
};
static int pca9555_fd;
static int i2c_fd;
static int pin_fd;
#define DEV_PIN_PATH "/pin"
#define DEV_PCA9555_PATH "/pca9555"
#define DEV_I2C2_PATH "/i2c2"
#define WAV_DECODE_SIZE (367) /* single channel */
int16_t music_output_buffer[DMA_BUFFER_SIZE];
audio_codec_type audio_codec;
uint16_t spk_dma_buffer[DMA_BUFFER_SIZE];
uint16_t mic_dma_buffer[DMA_BUFFER_SIZE];
void DMA1_Channel3_IRQHandler(void);
void DMA1_Channel4_IRQHandler(void);
volatile uint32_t i2s_dma_tx_end;
volatile uint32_t i2s_dma_rx_end;

void audio_output_cp(void *src, int len)
{
  audio_wait_data_tx_end();
  memcpy(spk_dma_buffer, src, len);
  dma_channel_enable(I2S_DMA_TX_CHANNEL, FALSE);
  dma_data_number_set(I2S_DMA_TX_CHANNEL, len >> 1);
  dma_channel_enable(I2S_DMA_TX_CHANNEL, TRUE);
}

/**
 * @brief  mclk clock conifg
 * @param  none
 * @retval none
 */
void mclk_timer_init(void)
{
  int ret;
#if 0
  gpio_init_type gpio_init_struct;
#endif
  tmr_output_config_type tmr_oc_init_structure;
  uint16_t prescaler_value = (uint16_t)(sys_read_clk() / 24000000) - 1;

  crm_periph_clock_enable(CRM_TMR8_PERIPH_CLOCK, TRUE);
#if 0
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  gpio_pin_mux_config(GPIOC, GPIO_PINS_SOURCE6, GPIO_MUX_3);

  gpio_init_struct.gpio_pins = GPIO_PINS_6;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOC, &gpio_init_struct);
#endif
again:
  pin_fd = open(DEV_PIN_PATH, O_RDWR, 0777);

  if (pin_fd < 0)
  {
    u_sleep_ms(50);
    goto again;
  }
  ret = ioctl(pin_fd, MK_PIN_SET_MODE, ((mk_pin_cfg_t){
                                            .mode = MK_PIN_MODE_MUX_OUTPUT,
                                            .pin = (2 * 16) + 6,
                                            .cfg = GPIO_MUX_3,
                                        })
                                           .cfg_raw);
  if (ret < 0)
  {
    printf("gpio init failed.\n");
    while (1)
      ;
  }

  tmr_base_init(TMR8, 1, prescaler_value);
  tmr_cnt_dir_set(TMR8, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR8, TMR_CLOCK_DIV1);

  tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = TRUE;
  tmr_output_channel_config(TMR8, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR8, TMR_SELECT_CHANNEL_1, 1);
  tmr_output_channel_buffer_enable(TMR8, TMR_SELECT_CHANNEL_1, TRUE);

  /* tmr enable counter */
  tmr_counter_enable(TMR8, TRUE);
  tmr_output_enable(TMR8, TRUE);
}

/**
 * @brief  audio codec i2s reset
 * @param  none
 * @retval none
 */
void codec_i2s_reset(void)
{
  i2s_enable(SPI1, FALSE);
  i2s_enable(I2S_SPIx, FALSE);
  dma_channel_enable(I2S_DMA_TX_CHANNEL, FALSE);
  dma_channel_enable(I2S_DMA_RX_CHANNEL, FALSE);
}
typedef struct pin_cfg
{
  int pin;
  gpio_mux_sel_type mux_type;
  int mode;
} pin_cfg_t;

static const pin_cfg_t audio_pins[] = {
    {(1 * 16 + 13), GPIO_MUX_5, MK_PIN_MODE_MUX_OUTPUT_DOWN},
    {(1 * 16 + 12), GPIO_MUX_5, MK_PIN_MODE_MUX_OUTPUT_UP},
    {(1 * 16 + 15), GPIO_MUX_5, MK_PIN_MODE_MUX_OUTPUT_DOWN},
    {(1 * 16 + 14), GPIO_MUX_6, MK_PIN_MODE_MUX_OUTPUT_UP},
};
static uint8_t i2s_dma_tx_intr_stack[1024];
static obj_handler_t i2s_dma_tx_intr_obj;
static uint8_t i2s_dma_rx_intr_stack[1024];
static obj_handler_t i2s_dma_rx_intr_obj;
/**
 * @brief  audio codec i2s init
 * @param  freq: audio sampling freq
 * @param  bitw_format: bit width
 * @retval error status
 */
void codec_i2s_init(audio_codec_type *param)
{
  int ret;
  dma_init_type dma_init_struct;
  i2s_init_type i2s_init_struct;

  crm_periph_clock_enable(I2S_CK_GPIO_CLK, TRUE);
  crm_periph_clock_enable(I2S_WS_GPIO_CLK, TRUE);
  crm_periph_clock_enable(I2S_SD_OUT_GPIO_CLK, TRUE);
  crm_periph_clock_enable(I2S_SD_IN_GPIO_CLK, TRUE);

  crm_periph_clock_enable(I2S_DMAx_CLK, TRUE);
  crm_periph_clock_enable(I2S_SPIx_CLK, TRUE);

  param->spk_tx_size = DMA_BUFFER_SIZE;
  param->mic_rx_size = DMA_BUFFER_SIZE;

  for (int i = 0; i < ARRAY_SIZE(audio_pins); i++)
  {
    ret = ioctl(pin_fd, MK_PIN_SET_MODE, ((mk_pin_cfg_t){
                                              .mode = audio_pins[i].mode,
                                              .pin = audio_pins[i].pin,
                                              .cfg = audio_pins[i].mux_type,
                                          })
                                             .cfg_raw);
    if (ret < 0)
    {
      printf("audio pin set error.\n");
      while (1)
        ;
    }
  }
#if 0
  gpio_init_type gpio_init_struct;
  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;

  /* i2s2 ws pins */
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_pins = I2S_WS_GPIO_PIN;
  gpio_init(I2S_WS_GPIO_PORT, &gpio_init_struct);
  gpio_pin_mux_config(I2S_WS_GPIO_PORT, I2S_WS_GPIO_PINS_SOURCE, I2S_WS_GPIO_MUX);

  /* i2s2 ck pins */
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_init_struct.gpio_pins = I2S_CK_GPIO_PIN;
  gpio_init(I2S_CK_GPIO_PORT, &gpio_init_struct);
  gpio_pin_mux_config(I2S_CK_GPIO_PORT, I2S_CK_GPIO_PINS_SOURCE, I2S_CK_GPIO_MUX);

  /* i2s2 SD pins */
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_init_struct.gpio_pins = I2S_SD_OUT_GPIO_PIN;
  gpio_init(I2S_SD_OUT_GPIO_PORT, &gpio_init_struct);
  gpio_pin_mux_config(I2S_SD_OUT_GPIO_PORT, I2S_SD_OUT_GPIO_PINS_SOURCE, I2S_SD_OUT_GPIO_MUX);

  /* i2s2_EXT pins slave rx */
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_pins = I2S_SD_IN_GPIO_PIN;
  gpio_init(I2S_SD_IN_GPIO_PORT, &gpio_init_struct);
  gpio_pin_mux_config(I2S_SD_IN_GPIO_PORT, I2S_SD_IN_GPIO_PINS_SOURCE, I2S_SD_IN_GPIO_MUX);
#endif
  /* dma config */
  dma_reset(I2S_DMA_TX_CHANNEL);
  dma_reset(I2S_DMA_RX_CHANNEL);

  /* dma1 channel3: speaker i2s1 tx */
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = param->spk_tx_size;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)spk_dma_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)I2S_DT_ADDRESS;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(I2S_DMA_TX_CHANNEL, &dma_init_struct);
  dma_interrupt_enable(I2S_DMA_TX_CHANNEL, DMA_FDT_INT, TRUE);
  // dma_interrupt_enable(I2S_DMA_TX_CHANNEL, DMA_HDT_INT, TRUE);
  // nvic_irq_enable(I2S_DMA_TX_IRQn, 1, 0);

  ret = u_irq_request(I2S_DMA_TX_IRQn, (void *)(i2s_dma_tx_intr_stack + sizeof(i2s_dma_tx_intr_stack)),
                      NULL, &i2s_dma_tx_intr_obj, DMA1_Channel3_IRQHandler, u_irq_prio_create(1, 0));
  if (ret < 0)
  {
    printf("irq request error.\n");
    while (1)
      ;
  }

  /* dma1 channel4: microphone i2s2 rx */
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = param->mic_rx_size;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)mic_dma_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)(&(I2S_EXTx->dt));
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(I2S_DMA_RX_CHANNEL, &dma_init_struct);
  dma_interrupt_enable(I2S_DMA_RX_CHANNEL, DMA_FDT_INT, TRUE);
  // dma_interrupt_enable(I2S_DMA_RX_CHANNEL, DMA_HDT_INT, TRUE);
  // nvic_irq_enable(I2S_DMA_RX_IRQn, 2, 0);

  ret = u_irq_request(I2S_DMA_RX_IRQn, (void *)(i2s_dma_rx_intr_stack + sizeof(i2s_dma_rx_intr_stack)),
                      NULL, &i2s_dma_rx_intr_obj, DMA1_Channel4_IRQHandler, u_irq_prio_create(2, 0));
  if (ret < 0)
  {
    printf("irq request error.\n");
    while (1)
      ;
  }
  /* i2s2 rx init */
  /* i2s2 Tx init */
  spi_i2s_reset(I2S_SPIx);
  i2s_default_para_init(&i2s_init_struct);
  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_PHILLIPS;
  i2s_init_struct.data_channel_format = I2S_DATA_16BIT_CHANNEL_16BIT;
  i2s_init_struct.mclk_output_enable = TRUE;
  i2s_init_struct.audio_sampling_freq = I2S_AUDIO_FREQUENCY_48K;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode = I2S_MODE_SLAVE_TX;
  i2s_init(I2S_SPIx, &i2s_init_struct);
  /* i2s2EXT Rx init */
  i2s_init_struct.operation_mode = I2S_MODE_SLAVE_RX;
  i2s_init(I2S_EXTx, &i2s_init_struct);

  spi_i2s_dma_transmitter_enable(I2S_SPIx, TRUE);
  spi_i2s_dma_receiver_enable(I2S_EXTx, TRUE);

  dmamux_enable(I2S_DMAx, TRUE);
  dmamux_init(I2S_DMA_TX_DMAMUX_CHANNEL, I2S_DMA_TX_DMAREQ);
  dmamux_init(I2S_DMA_RX_DMAMUX_CHANNEL, I2S_DMA_RX_DMAREQ);
  dma_channel_enable(I2S_DMA_TX_CHANNEL, TRUE);
  dma_channel_enable(I2S_DMA_RX_CHANNEL, TRUE);

  i2s_enable(I2S_EXTx, TRUE);
  i2s_enable(I2S_SPIx, TRUE);
}

/**
 * @brief  power on
 * @param  none
 * @retval none
 */
void audio_pa_power_on(void)
{
#if 0
  pca9555_out_mode_config(AUDIO_PA_ON_PIN);

  pca9555_bits_reset(AUDIO_PA_ON_PIN);
#endif
  ioctl(pca9555_fd, PCA9555_SET_OUTPUT_MODE, AUDIO_PA_ON_PIN);
  ioctl(pca9555_fd, PCA9555_IO_RESET, AUDIO_PA_ON_PIN);
}

/**
 * @brief  power off
 * @param  none
 * @retval none
 */
void audio_pa_power_off(void)
{
#if 0
  pca9555_out_mode_config(AUDIO_PA_ON_PIN);

  pca9555_bits_set(AUDIO_PA_ON_PIN);
#endif
  ioctl(pca9555_fd, PCA9555_SET_OUTPUT_MODE, AUDIO_PA_ON_PIN);
  ioctl(pca9555_fd, PCA9555_IO_SET, AUDIO_PA_ON_PIN);
}

/**
 * @brief  audio codec init
 * @param  none
 * @retval error status
 */
error_status audio_init(void)
{
  uint32_t reg_len = sizeof(reg_addr_data) / sizeof(uint16_t);
  uint32_t i = 0;
  uint8_t i2c_cmd[2];
  int ret;

again:
  pca9555_fd = open(DEV_PCA9555_PATH, O_RDWR, 0777);
  if (pca9555_fd < 0)
  {
    u_sleep_ms(50);
    goto again;
  }
again2:
  i2c_fd = open(DEV_I2C2_PATH, O_RDWR, 0777);
  if (i2c_fd < 0)
  {
    u_sleep_ms(50);
    goto again2;
  }

  audio_pa_power_on();
  ret = lseek(i2c_fd, (0 << 16) | (WM8988_I2C_ADDR_CSB_LOW), SEEK_SET);
  if (ret < 0)
  {
    printf("%s:%d ret:%d\n", __func__, __LINE__, ret);
    return ret;
  }
  for (i = 0; i < reg_len; i++)
  {
    i2c_cmd[0] = (uint8_t)(reg_addr_data[i] >> 8);

    i2c_cmd[1] = (uint8_t)reg_addr_data[i] & 0xFF;

    ret = write(i2c_fd, i2c_cmd, 2);
    if (ret != 2)
    {
      printf("error:%s:%d ret:%d\n", __func__, __LINE__, ret);
      return -1;
    }
  }

  /* timer init */
  mclk_timer_init();

  codec_i2s_init(&audio_codec);

  audio_codec_modify_freq(WM8988_REG_FREQ_22_05K);

  audio_dma_counter_set(WAV_DECODE_SIZE);

  // audio_wait_data_tx_end();

  audio_spk_volume_set(220, 220);

#if 1
  for (int i = 0; i < DMA_BUFFER_SIZE; i++)
  {
    music_output_buffer[i] = 0;
  }
#endif
  return SUCCESS;
}

/**
 * @brief  set audio volume
 * @param  left_volume
 * @param  right_volume
 * @retval error_status
 */
error_status audio_spk_volume_set(uint8_t left_volume, uint8_t right_volume)
{

  uint16_t audio_left;
  uint16_t audio_right;
  int ret;

  uint8_t i2c_cmd[2];

  audio_left = WM8988_R10_LEFT_DAC_VOLUME << 9 | 0x100 | left_volume;
  audio_right = WM8988_R11_RIGHT_DAC_VOLUME << 9 | 0x100 | right_volume;

  i2c_cmd[0] = (uint8_t)(audio_left >> 8);
  i2c_cmd[1] = (uint8_t)audio_left & 0xFF;

  ret = write(i2c_fd, i2c_cmd, 2);
  if (ret != 2)
  {
    printf("error:%s:%d ret:%d\n", __func__, __LINE__, ret);
    return ERROR;
  }

  i2c_cmd[0] = (uint8_t)(audio_right >> 8);
  i2c_cmd[1] = (uint8_t)audio_right & 0xFF;

  ret = write(i2c_fd, i2c_cmd, 2);
  if (ret != 2)
  {
    printf("error:%s:%d ret:%d\n", __func__, __LINE__, ret);
    return ERROR;
  }
  return SUCCESS;
}

/**
  * @brief  audio codec modify freq
  * @param  freq: freq (wm8988 microphone and speaker must as same freq)
            WM8988_REG_FREQ_8_K
            WM8988_REG_FREQ_8_0214_K
            WM8988_REG_FREQ_11_025_K
            WM8988_REG_FREQ_12_K
            WM8988_REG_FREQ_16K
            WM8988_REG_FREQ_22_05K
            WM8988_REG_FREQ_24K
            WM8988_REG_FREQ_32_K
            WM8988_REG_FREQ_44_1_K
            WM8988_REG_FREQ_48_K
            WM8988_REG_FREQ_88_235K
            WM8988_REG_FREQ_96_K
  * @retval none
  */
void audio_codec_modify_freq(uint16_t freq)
{
  int ret;
  uint8_t i2c_cmd[2];

  i2c_cmd[0] = (uint8_t)(freq >> 8);

  i2c_cmd[1] = (uint8_t)freq & 0xFF;

  ret = write(i2c_fd, i2c_cmd, 2);
  if (ret != 2)
  {
    printf("error:%s:%d ret:%d\n", __func__, __LINE__, ret);
    while (1)
      ;
  }
}

/**
  * @brief  audio codec modify freq
  * @param  freq: freq (wm8988 microphone and speaker must as same freq)
            WM8988_REG_FREQ_8_K
            WM8988_REG_FREQ_8_0214_K
            WM8988_REG_FREQ_11_025_K
            WM8988_REG_FREQ_12_K
            WM8988_REG_FREQ_16K
            WM8988_REG_FREQ_22_05K
            WM8988_REG_FREQ_24K
            WM8988_REG_FREQ_32_K
            WM8988_REG_FREQ_44_1_K
            WM8988_REG_FREQ_48_K
            WM8988_REG_FREQ_88_235K
            WM8988_REG_FREQ_96_K
  * @retval none
  */
void audio_dma_counter_set(uint16_t counter)
{
  audio_codec.spk_tx_size = counter * 2;
}

/**
 * @brief  wait data transmit end
 * @param  none
 * @retval none
 */
void audio_wait_data_tx_end(void)
{
  while (i2s_dma_tx_end == 0)
    ;

  i2s_dma_tx_end = 0;
}

/**
 * @brief  audio codec modify freq
 * @param  counter: freq (wm8988 microphone and speaker must as same freq)
 * @retval none
 */
void modify_dma_tx_counter(uint16_t counter)
{
  static uint16_t counter_last = 0xFFFF;

  /* the frequency value changes */
  if (counter_last != counter)
  {
    dma_channel_enable(I2S_DMA_TX_CHANNEL, FALSE);

    dma_data_number_set(I2S_DMA_TX_CHANNEL, counter);

    dma_channel_enable(I2S_DMA_TX_CHANNEL, TRUE);

    audio_codec.spk_tx_size = counter;

    counter_last = counter;
  }
}

/**
 * @brief  this function handles dma1 channel3 interrupt.
 * @param  none
 * @retval none
 */
void DMA1_Channel3_IRQHandler(void)
{
  while (1)
  {
    msg_tag_t tag = uirq_wait(i2s_dma_tx_intr_obj, 0);
    if (msg_tag_get_val(tag) >= 0)
    {
      if (dma_interrupt_flag_get(I2S_DMA_TX_HDT_FLAG) == SET)
      {
        dma_flag_clear(I2S_DMA_TX_HDT_FLAG);
      }
      else if (dma_interrupt_flag_get(I2S_DMA_TX_FDT_FLAG) == SET)
      {
        // memcpy(spk_dma_buffer, (uint16_t *)music_output_buffer, DMA_BUFFER_SIZE);
        // modify_dma_tx_counter(audio_codec.spk_tx_size);
        i2s_dma_tx_end = 1;
        dma_flag_clear(I2S_DMA_TX_FDT_FLAG);
      }
      uirq_ack(i2s_dma_tx_intr_obj, I2S_DMA_TX_IRQn);
    }
  }
}

/**
 * @brief  this function handles dma1 channel4 interrupt.
 * @param  none
 * @retval none
 */
void DMA1_Channel4_IRQHandler(void)
{
  while (1)
  {
    msg_tag_t tag = uirq_wait(i2s_dma_rx_intr_obj, 0);
    if (msg_tag_get_val(tag) >= 0)
    {
      if (dma_interrupt_flag_get(I2S_DMA_RX_HDT_FLAG) == SET)
      {
        dma_flag_clear(I2S_DMA_RX_HDT_FLAG);
      }
      else if (dma_interrupt_flag_get(I2S_DMA_RX_FDT_FLAG) == SET)
      {
        dma_flag_clear(I2S_DMA_RX_FDT_FLAG);
      }
      uirq_ack(i2s_dma_rx_intr_obj, I2S_DMA_RX_IRQn);
    }
  }
}
