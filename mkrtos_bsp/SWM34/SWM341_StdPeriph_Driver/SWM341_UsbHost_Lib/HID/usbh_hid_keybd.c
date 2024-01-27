/****************************************************************************************************************************************** 
* �ļ�����: usbh_hid_keybd.c
* ����˵��:	
* ����֧��:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* ע������: 
* �汾����:	V1.1.0		2020��11��3��
* ������¼:  
*
*
*******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION 
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE 
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/
#include "SWM341.h"
#include "usbh_hid_core.h"
#include "usbh_hid_keybd.h"


USBH_HID_cb_t USBH_HID_KeyBD_cb = 
{
	USBH_HID_KeyBD_Init,
	USBH_HID_KeyBd_Decode
};




void USBH_HID_KeyBD_Init(void)
{
}


void USBH_HID_KeyBd_Decode(uint8_t *pbuf)
{
}
