/****************************************************************************************************************************************** 
* �ļ�����: usbh_hid_mouse.c
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
#include "usbh_hid_mouse.h"


USBH_HID_cb_t USBH_HID_Mouse_cb =
{
	USBH_HID_Mouse_Init,
	USBH_HID_Mouse_Decode,
};


USBH_HID_Mouse_Data_t USBH_HID_Mouse_Data;


void USBH_HID_Mouse_Init(void)
{
}


void USBH_HID_Mouse_Decode(uint8_t *data)
{
	USBH_HID_Mouse_Data.button = data[0];
	USBH_HID_Mouse_Data.x      = data[1];
	USBH_HID_Mouse_Data.y      = data[2];
}
