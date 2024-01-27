/****************************************************************************************************************************************** 
* �ļ�����: SWM341_port.c
* ����˵��:	SWM341��Ƭ���Ķ˿����Ź���ѡ��⺯��
* ����֧��:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* ע������:
* �汾����: V1.0.0		2016��1��30��
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
#include "SWM341_port.h"


/****************************************************************************************************************************************** 
* ��������: PORT_Init()
* ����˵��:	�˿����Ź���ѡ��
* ��    ��: PORT_TypeDef * PORTx	ָ��PORT�˿ڣ���Чֵ����PORTA��PORTB��PORTC��PORTD��PORTE��PORTM��PORTN
*			uint32_t n		   		ָ��PORT���ţ���Чֵ����PIN0��PIN1��PIN2��... ... PIN14��PIN15
*			uint32_t func	   		ָ���˿�����Ҫ�趨�Ĺ��ܣ����ȡֵ��"SWM201_port.h"�ļ�
*			uint32_t digit_in_en   	��������ʹ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void PORT_Init(PORT_TypeDef * PORTx, uint32_t n, uint32_t func, uint32_t digit_in_en)
{
	if(n < PIN8)
	{
		PORTx->FUNC0 &= ~(0x0F << (n*4));
		PORTx->FUNC0 |=  (func << (n*4));
	}
	else
	{
		PORTx->FUNC1 &= ~(0x0F << ((n-8)*4));
		PORTx->FUNC1 |=  (func << ((n-8)*4));
	}
	
	if(digit_in_en)  PORTx->INEN |=  (1 << n);
	else             PORTx->INEN &= ~(1 << n);
}
