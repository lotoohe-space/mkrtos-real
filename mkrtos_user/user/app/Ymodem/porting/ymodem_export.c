/**
 ******************************************************************************
 * @file    STM32F0xx_IAP/src/common.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    29-May-2012
 * @brief   Main program body
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
// #include "ymodem_port.h"
// #include "ff.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <u_types.h>
#include <unistd.h>
/** @addtogroup STM32F0xx_IAP
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/********************************************************************************
 *                     Porting APIs
 ********************************************************************************/
typedef enum {
    TRANS_FILE = 0,
    IAP_IMAGE = 1,
} YMODEM_ACTION_E;

typedef struct {
    int ymodem_tty_fd;
    YMODEM_ACTION_E ymodem_action;
    uint32_t ymodem_receive_supported_maxsize;
    uint32_t ymodem_transmit_size;

    bool_t ymodem_file_inited;
    uint32_t ymodem_file_total_size;
    uint32_t ymodem_file_handled_size;
    int ymodem_file_handle;
    // uint8_t ymodem_file_tmppath[128];
} YMODEM_DATA_T;

static YMODEM_DATA_T s_ymodem_data;
static void tty_init(void)
{
    struct termios old_settings;
    struct termios new_settings;
    tcgetattr(STDIN_FILENO, &old_settings);
    new_settings = old_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO); // 禁用规范模式和回显
    new_settings.c_cc[VMIN] = 1;              // 读取的最小字符数
    new_settings.c_cc[VTIME] = 0;             // 读取的超时时间（以10ms为单位）
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}
void ymodem_init(void)
{
    tty_init();
    memset(&s_ymodem_data, 0, sizeof(YMODEM_DATA_T));
    s_ymodem_data.ymodem_tty_fd = STDIN_FILENO;

    s_ymodem_data.ymodem_action = TRANS_FILE;

    switch (s_ymodem_data.ymodem_action) {
    case TRANS_FILE:
        s_ymodem_data.ymodem_receive_supported_maxsize = 1024 * 1024 * 5;
        s_ymodem_data.ymodem_transmit_size = 0;
        break;
#if 0
    case IAP_IMAGE:
#warning "MUST implement GOT transmit SIZE!"
        s_ymodem_data.ymodem_receive_supported_maxsize = 1024 * 1024 * 5;
        s_ymodem_data.ymodem_transmit_size = 0;
        break;
#endif
    default:
        // Not supported
        break;
    }
}
uint32_t ymodem_get_receive_maxsize(void)
{
    return s_ymodem_data.ymodem_receive_supported_maxsize;
}

uint32_t ymodem_get_transmit_size(void)
{
    return s_ymodem_data.ymodem_transmit_size;
}
#include <fcntl.h>
// 0 - success ; -1 - fail
int ymodem_recv_start_cb(const char *filename, const uint32_t filesize)
{
    // FRESULT fres;
    int ret;

    switch (s_ymodem_data.ymodem_action) {
    case TRANS_FILE:
        ret = open(filename, O_RDWR | O_CREAT, 0777);
        if (ret < 0) {
            //logd("Ymodem Start: create file(%s) fail(%d)...\n", filename, ret);
            return -1;
        } else {
            //logd("Ymodem Start: create file(%s) ok(%d)...\n", filename, ret);

            s_ymodem_data.ymodem_file_inited = TRUE;
            s_ymodem_data.ymodem_file_total_size = filesize;
            s_ymodem_data.ymodem_file_handled_size = 0;
            s_ymodem_data.ymodem_file_handle = ret;
            return 0;
        }
        // break;

    case IAP_IMAGE:
        /* erase user application area */
        // FLASH_If_Erase(APPLICATION_ADDRESS);

        break;

    default:
        // Not supported
        break;
    }
    return -1;
}

int ymodem_recv_processing_cb(const uint8_t *buffer, const uint32_t buff_size)
{
    int ret;
    uint32_t to_write_size = 0;
    uint32_t writted_size = 0;

    switch (s_ymodem_data.ymodem_action) {
    case TRANS_FILE:
        to_write_size = s_ymodem_data.ymodem_file_total_size - s_ymodem_data.ymodem_file_handled_size;
        to_write_size = (buff_size > to_write_size) ? to_write_size : buff_size;

        ret = write(s_ymodem_data.ymodem_file_handle,  buffer, to_write_size);
        if (ret != to_write_size) {
            //loge("Ymodem process: write file(%d - %d) fail(%d)...\n", to_write_size, writted_size, ret);
            return -1;
        } else {
            s_ymodem_data.ymodem_file_handled_size += to_write_size;
           // logi("Ymodem process: write file(%d/%d) ok(%d)...\n", s_ymodem_data.ymodem_file_handled_size,
           //      s_ymodem_data.ymodem_file_total_size, ret);
            return 0;
        }
        // break;

    case IAP_IMAGE:
        // if (FLASH_If_Write(&flashdestination, (uint32_t*) ramsource, (uint16_t) packet_length/4)  == 0)
        break;

    default:
        // Not supported
        break;
    }
    return -1;
}

int ymodem_recv_end_cb(void)
{
    int fres;
    // FILINFO fno;

    switch (s_ymodem_data.ymodem_action) {
    case TRANS_FILE:
        if (TRUE != s_ymodem_data.ymodem_file_inited)
            return -1;

        fres = close(s_ymodem_data.ymodem_file_handle);
        //logd("Ymodem End: close file res(%d)...\n", fres);
#if 0
        fres = stat((const TCHAR *)s_ymodem_data.ymodem_file_tmppath, &fno);
        if (fres != RES_OK) {
            logw("Get File Status Fail(%d)...\n", fres);
        } else {
            logi("Get File Status ok(%d), file size(%d) Bytes...\n", fres, fno.fsize);
        }
#endif
        s_ymodem_data.ymodem_file_handle = -1;
        s_ymodem_data.ymodem_file_total_size = 0;
        s_ymodem_data.ymodem_file_handled_size = 0;
        s_ymodem_data.ymodem_file_inited = FALSE;
        return 0;
        // break;

    case IAP_IMAGE:
        /* erase user application area */
        // FLASH_If_Erase(APPLICATION_ADDRESS);

        break;

    default:
        // Not supported
        break;
    }
    return -1;
}
/**
 * @brief  Test to see if a key has been pressed on the HyperTerminal
 * @param  key: The key pressed
 * @retval 1: Correct
 *         0: Error
 */
uint32_t SerialKeyPressed(uint8_t *key)
{
    int ret;

    ret = read(STDIN_FILENO, key, 1);
    if (ret < 0) {
        return 0;
    }
    return 1;
}
// fail - 0xff; success -other value
uint8_t SerialReadByte(void)
{
    int ret;
    uint8_t byte;

    ret = read(STDIN_FILENO, &byte, 1);
    if (ret < 0) {
        return 0xff;
    }
    return byte;
}

/**
 * @brief  Print a character on the HyperTerminal
 * @param  c: The character to be printed
 * @retval None
 */
void SerialPutChar(uint8_t c)
{
    write(STDIN_FILENO, &c, 1);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
