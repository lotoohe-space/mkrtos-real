/**
  ******************************************************************************
  * @file    STM32F0xx_IAP/inc/common.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    29-May-2012
  * @brief   Header for main.c module
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

#ifndef __YMODEM_EXPORT_H__
#define __YMODEM_EXPORT_H__

/********************************************************************************
 *                   The  Public APIs                                           *
 ********************************************************************************/
void Ymodem_Main_Entrance(void);



/********************************************************************************
 *                   The  Porting APIs Must Be Implemented                      *
 ********************************************************************************/
extern unsigned int     SerialKeyPressed(unsigned char *key);
extern void             SerialPutChar(unsigned char c);
extern unsigned char    SerialReadByte(void);

extern void             ymodem_init(void);
extern unsigned int     ymodem_get_receive_maxsize(void);
extern unsigned int     ymodem_get_transmit_size(void);

//0 - success ; -1 - fail
extern int              ymodem_recv_start_cb(const char * filename, const unsigned int filesize);
extern int              ymodem_recv_processing_cb(const unsigned char * buffer, const unsigned int buff_size);
extern int              ymodem_recv_end_cb(void);

#endif  /* __YMODEM_EXPORT_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
