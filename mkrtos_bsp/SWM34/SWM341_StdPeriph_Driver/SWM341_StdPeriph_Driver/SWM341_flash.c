/****************************************************************************************************************************************** 
* �ļ�����:	SWM341_flash.c
* ����˵��:	ʹ��оƬ��IAP���ܽ�Ƭ��Flashģ���EEPROM���������ݣ�����󲻶�ʧ
* ����֧��:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* ע������:
* �汾����: V1.0.0		2016��1��30��
* ������¼: 
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
#include "SWM341_flash.h"


const IAP_Cache_Reset_t IAP_Cache_Reset = (IAP_Cache_Reset_t)0x11000401;
const IAP_Flash_Param_t IAP_Flash_Param = (IAP_Flash_Param_t)0x11000431;
const IAP_Flash_Erase_t IAP_Flash_Erase = (IAP_Flash_Erase_t)0x11000471;
const IAP_Flash_Write_t IAP_Flash_Write = (IAP_Flash_Write_t)0x110004C1;


/****************************************************************************************************************************************** 
* ��������:	FLASH_Erase()
* ����˵��:	FLASH����������ÿ������4K�ֽ�
* ��    ��: uint32_t addr		Ҫ���������ĵ�ַ������4K���룬��addr%4096 == 0
* ��    ��: uint32_t			FLASH_RES_OK��FLASH_RES_TO��FLASH_RES_ERR
* ע������: ��
******************************************************************************************************************************************/
uint32_t FLASH_Erase(uint32_t addr)
{
	if(addr % 4096 != 0) return FLASH_RES_ERR;
	
	__disable_irq();
	
	IAP_Flash_Erase(addr/4096, 0x0B11FFAC);
	
	IAP_Cache_Reset((FMC->CACHE | FMC_CACHE_CCLR_Msk), 0x0B11FFAC);	// Cache Clear
	
	__enable_irq();
	
	return FLASH_RES_OK;
}


/****************************************************************************************************************************************** 
* ��������:	FLASH_Write()
* ����˵��:	FLASH����д��
* ��    ��: uint32_t addr		����Ҫд�뵽Flash�еĵ�ַ�����ֶ���
*			uint32_t buff[]		Ҫд��Flash�е�����
*			uint32_t count		Ҫд�����ݵĸ���������Ϊ��λ���ұ�����4����������������д��4����
* ��    ��: uint32_t			FLASH_RES_OK��FLASH_RES_TO��FLASH_RES_ERR
* ע������: ��
******************************************************************************************************************************************/
uint32_t FLASH_Write(uint32_t addr, uint32_t buff[], uint32_t count)
{
	if(addr % 16 != 0) return FLASH_RES_ERR;
	if(count % 4 != 0) return FLASH_RES_ERR;
	
	__disable_irq();
	
	IAP_Flash_Write(addr, (uint32_t)buff, count/4, 0x0B11FFAC);
	
	IAP_Cache_Reset((FMC->CACHE | FMC_CACHE_CCLR_Msk), 0x0B11FFAC);	// Cache Clear
	
	__enable_irq();
	
	return FLASH_RES_OK;
}


/****************************************************************************************************************************************** 
* ��������: Flash_Param_at_xMHz()
* ����˵��:	��Flash�������ó�xMHz��Ƶ������ʱ����Ĳ���
* ��    ��: uint32_t x		ȡֵ30��40��80��120��150
* ��    ��: ��
* ע������: ϵͳ��ƵԽ�ߣ�Flash���ʵȴ�����Խ��
******************************************************************************************************************************************/
void Flash_Param_at_xMHz(uint32_t x)
{
	__disable_irq();
	switch(x)
	{
		case 30:
			IAP_Flash_Param(0x8949, 0x0FA1, 0x0B11FFAC);
			break;
		
		case 40:
			IAP_Flash_Param(0x8A49, 0x14A9, 0x0B11FFAC);
			break;
		
		case 80:
			IAP_Flash_Param(0xD349, 0x294A, 0x0B11FFAC);
			break;
		
		case 120:
			IAP_Flash_Param(0x11C89, 0x3D6B, 0x0B11FFAC);
			break;
		
		case 150:
		default:
			IAP_Flash_Param(0x16589, 0x4C74, 0x0B11FFAC);
			break;
	}
	__enable_irq();
}
