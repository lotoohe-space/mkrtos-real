/****************************************************************************************************************************************** 
* �ļ�����: SWM341_lcd.c
* ����˵��:	SWM341��Ƭ����LCD����������
* ����֧��:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* ע������: 
* �汾����:	V1.1.0		2017��10��25��
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
#include "SWM341_lcd.h"

#include <string.h>


/****************************************************************************************************************************************** 
* ��������:	LCD_Init()
* ����˵��:	LCD��ʼ��
* ��    ��: LCD_TypeDef * LCDx	ָ��Ҫ�����õ�LCD����Чֵ����LCD
*			LCD_InitStructure * initStruct    ����LCD����趨ֵ�Ľṹ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void LCD_Init(LCD_TypeDef * LCDx, LCD_InitStructure * initStruct)
{	
	switch((uint32_t)LCDx)
	{
	case ((uint32_t)LCD):
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_LCD_Pos);
		break;
	}
	
	LCDx->CR  = ((initStruct->ClkDiv - 2)  << LCD_CR_CLKDIV_Pos)   |
				(initStruct->SampleEdge    << LCD_CR_CLKINV_Pos)   |
				(1                         << LCD_CR_CLKALW_Pos)   |
				((initStruct->Format & 1)  << LCD_CR_FORMAT_Pos)   |
			    ((initStruct->Format >> 1) << LCD_CR_SEREN_Pos)    |
				(1                         << LCD_CR_BURSTEN_Pos)  |
				(1                         << LCD_CR_BURSTLEN_Pos) |
				((1-initStruct->IntEOTEn)  << LCD_CR_AUTORESTA_Pos);
	
	LCDx->CRH = ((initStruct->HsyncWidth - 1) << LCD_CRH_HSW_Pos) |
				((initStruct->Hbp - 1)        << LCD_CRH_HBP_Pos) |
				((initStruct->HnPixel - 1)    << LCD_CRH_PIX_Pos) |
				((initStruct->Hfp - 1)        << LCD_CRH_HFP_Pos);
					
	LCDx->CRV = ((initStruct->VsyncWidth - 1) << LCD_CRV_VSW_Pos) |
				((initStruct->Vbp - 1)        << LCD_CRV_VBP_Pos) |
				((initStruct->VnPixel - 1)    << LCD_CRV_PIX_Pos) |
				((initStruct->Vfp - 1)        << LCD_CRV_VFP_Pos);
	
	LCDx->BGC = initStruct->Background;
	
	LCDx->L[0].LCR = (1    << LCD_LCR_EN_Pos)   |
					 (0    << LCD_LCR_CKEN_Pos) |
					 (0xFF << LCD_LCR_ALPHA_Pos);
	
	LCDx->L[0].WHP = (0                         << LCD_WHP_STA_Pos) |
					 ((initStruct->HnPixel - 1) << LCD_WHP_STP_Pos);
	
	LCDx->L[0].WVP = (0                         << LCD_WVP_STA_Pos) |
					 ((initStruct->VnPixel - 1) << LCD_WVP_STP_Pos);
	
	LCDx->L[0].ADDR = initStruct->DataSource;
	
	LCDx->L[0].LLEN = (initStruct->HnPixel - 1);
	
	LCDx->IF = 1;	//�����־
	LCDx->IE = initStruct->IntEOTEn;
	
	switch((uint32_t)LCDx)
	{
	case ((uint32_t)LCD):		
		if(initStruct->IntEOTEn) NVIC_EnableIRQ(LCD_IRQn);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������:	LCD_LayerInit()
* ����˵��:	LCD���ʼ��
* ��    ��: LCD_TypeDef * LCDx	ָ��Ҫ�����õ�LCD����Чֵ����LCD
*			uint32_t layerx		Ҫ��ʼ����LCD�㣬ȡֵLCD_LAYER_1��LCD_LAYER_2
*			LCD_LayerInitStructure * initStruct    ����LCD������趨ֵ�Ľṹ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void LCD_LayerInit(LCD_TypeDef * LCDx, uint32_t layerx, LCD_LayerInitStructure * initStruct)
{
	LCDx->L[layerx].LCR = (1                 << LCD_LCR_EN_Pos)   |
					      (0                 << LCD_LCR_CKEN_Pos) |
					      (initStruct->Alpha << LCD_LCR_ALPHA_Pos);
	
	LCDx->L[layerx].WHP = (initStruct->HStart << LCD_WHP_STA_Pos) |
					      (initStruct->HStop  << LCD_WHP_STP_Pos);
	
	LCDx->L[layerx].WVP = (initStruct->VStart << LCD_WVP_STA_Pos) |
					      (initStruct->VStop  << LCD_WVP_STP_Pos);
	
	LCDx->L[layerx].ADDR = initStruct->DataSource;
	
	LCDx->L[layerx].LLEN = (initStruct->HStop - initStruct->HStart);
	
	LCD->CR |= (1 << LCD_CR_VBPRELOAD_Pos);
}

/****************************************************************************************************************************************** 
* ��������:	LCD_SetLayerPos()
* ����˵��:	LCD��λ���趨
* ��    ��: LCD_TypeDef * LCDx	ָ��Ҫ�����õ�LCD����Чֵ����LCD
*			uint32_t layerx		Ҫ���õ�LCD�㣬ȡֵLCD_LAYER_1��LCD_LAYER_2
*			uint16_t hstart, uint16_t hstop, uint16_t vstart, uint16_t vstop����ϸ˵���� LCD_LayerInitStructure
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void LCD_SetLayerPos(LCD_TypeDef * LCDx, uint32_t layerx, uint16_t hstart, uint16_t hstop, uint16_t vstart, uint16_t vstop)
{
	LCDx->L[layerx].WHP = (hstart << LCD_WHP_STA_Pos) |
					      (hstop  << LCD_WHP_STP_Pos);
	
	LCDx->L[layerx].WVP = (vstart << LCD_WVP_STA_Pos) |
					      (vstop  << LCD_WVP_STP_Pos);
	
	LCD->CR |= (1 << LCD_CR_VBPRELOAD_Pos);
}

/****************************************************************************************************************************************** 
* ��������: LCD_Start()
* ����˵��:	����һ�����ݴ���
* ��    ��: LCD_TypeDef * LCDx	ָ��Ҫ�����õ�LCD����Чֵ����LCD
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void LCD_Start(LCD_TypeDef * LCDx)
{
	LCDx->START |= (1 << LCD_START_GO_Pos);
}

/****************************************************************************************************************************************** 
* ��������: LCD_IsBusy()
* ����˵��:	�Ƿ����ڽ������ݴ���
* ��    ��: LCD_TypeDef * LCDx	ָ��Ҫ�����õ�LCD����Чֵ����LCD
* ��    ��: uint32_t			1 ���ڴ�������    0 ���ݴ��������
* ע������: ��
******************************************************************************************************************************************/
uint32_t LCD_IsBusy(LCD_TypeDef * LCDx)
{
	return (LCDx->START & LCD_START_GO_Msk) ? 1 : 0;
}

/****************************************************************************************************************************************** 
* ��������: LCD_INTEn()
* ����˵��:	LCD�ж�ʹ�ܣ����ָ�����ȵ����ݴ���ʱ�����ж�
* ��    ��: LCD_TypeDef * LCDx	ָ��Ҫ�����õ�LCD����Чֵ����LCD
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void LCD_INTEn(LCD_TypeDef * LCDx)
{
	LCDx->IE = 1;
}

/****************************************************************************************************************************************** 
* ��������: LCD_INTDis()
* ����˵��:	LCD�жϽ�ֹ�����ָ�����ȵ����ݴ���ʱ�������ж�
* ��    ��: LCD_TypeDef * LCDx	ָ��Ҫ�����õ�LCD����Чֵ����LCD
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void LCD_INTDis(LCD_TypeDef * LCDx)
{
	LCDx->IE = 0;
}

/****************************************************************************************************************************************** 
* ��������: LCD_INTClr()
* ����˵��:	LCD�жϱ�־���
* ��    ��: LCD_TypeDef * LCDx	ָ��Ҫ�����õ�LCD����Чֵ����LCD
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void LCD_INTClr(LCD_TypeDef * LCDx)
{
	LCDx->IF = 1;
}

/****************************************************************************************************************************************** 
* ��������: LCD_INTStat()
* ����˵��:	LCD�ж�״̬��ѯ
* ��    ��: LCD_TypeDef * LCDx	ָ��Ҫ�����õ�LCD����Чֵ����LCD
* ��    ��: uint32_t			0 δ���ָ�����ȵ����ݴ���   ��0 ���ָ�����ȵ����ݴ���    
* ע������: ��
******************************************************************************************************************************************/
uint32_t LCD_INTStat(LCD_TypeDef * LCDx)
{
	return (LCDx->IF & 1);
}


/****************************************************************************************************************************************** 
* ��������:	MPULCD_Init()
* ����˵��:	MPU LCD��ʼ��
* ��    ��: LCD_TypeDef * LCDx	ָ��Ҫ�����õ�LCD����Чֵ����LCD
*			MPULCD_InitStructure * initStruct    ����MPU LCD����趨ֵ�Ľṹ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void MPULCD_Init(LCD_TypeDef * LCDx, MPULCD_InitStructure * initStruct)
{
	switch((uint32_t)LCDx)
	{
	case ((uint32_t)LCD):
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_LCD_Pos);
		__NOP();__NOP();__NOP();
		break;
	}
	
	LCDx->CR  = (1              << LCD_CR_MPUEN_Pos) |
				(LCD_FMT_RGB565 << LCD_CR_FORMAT_Pos);	//MPUģʽֻ֧��RGB565
	
	LCDx->L[0].LCR |= (1 << LCD_LCR_EN_Pos);			//MPUģʽ����Ҫʹ��Layer1
	
	LCDx->MPUCR = ((initStruct->RDHoldTime    - 1) << LCD_MPUCR_RDHOLD_Pos) |
				  ((initStruct->WRHoldTime    - 1) << LCD_MPUCR_WRHOLD_Pos) |
				  ((initStruct->CSFall_WRFall - 1) << LCD_MPUCR_CS0WR0_Pos) |
				  ((initStruct->WRRise_CSRise - 1) << LCD_MPUCR_WR1CS1_Pos) |
				  ((initStruct->RDCSRise_Fall - 1) << LCD_MPUCR_RCS1_0_Pos) |
				  ((initStruct->WRCSRise_Fall - 1) << LCD_MPUCR_WCS1_0_Pos);
}

void LCD_WR_REG(LCD_TypeDef * LCDx, uint16_t reg)
{
	LCDx->MPUIR = reg;
	while(LCD_IsBusy(LCDx)) __NOP();
}

void LCD_WR_DATA(LCD_TypeDef * LCDx, uint16_t val)
{
	LCDx->MPUDR = val;
	while(LCD_IsBusy(LCDx)) __NOP();
}

void LCD_WriteReg(LCD_TypeDef * LCDx, uint16_t reg, uint16_t val)
{
	LCDx->MPUIR = reg;
	while(LCD_IsBusy(LCDx)) __NOP();
	
	LCDx->MPUDR = val;
	while(LCD_IsBusy(LCDx)) __NOP();
}

uint16_t LCD_ReadReg(LCD_TypeDef * LCDx, uint16_t reg)
{
	LCDx->MPUIR = reg;
	while(LCD_IsBusy(LCDx)) __NOP();
	
	return LCDx->MPUDR;
}

/****************************************************************************************************************************************** 
* ��������:	MPULCD_DMAStart()
* ����˵��:	MPU LCD DMA������ʾ����
* ��    ��: LCD_TypeDef * LCDx	ָ��Ҫ�����õ�LCD����Чֵ����LCD
*			uint32_t * buff		Ҫ���˵���ʾ����
*			uint16_t hpix		��������������ÿ����������������ż��
*			uint16_t vpix		������������������
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void MPULCD_DMAStart(LCD_TypeDef * LCDx, uint32_t * buff, uint16_t hpix, uint16_t vpix)
{
	LCDx->MPUAR = (uint32_t)buff;
	
	LCDx->MPULEN = ((vpix - 1) << LCD_MPULEN_VPIX_Pos) |
				   ((hpix - 1) << LCD_MPULEN_HPIX_Pos);
	
	LCDx->START |= (1 << LCD_START_GO_Pos);
}

/****************************************************************************************************************************************** 
* ��������:	MPULCD_DMABusy()
* ����˵��:	MPU LCD DMA������ʾ����æ��ѯ
* ��    ��: LCD_TypeDef * LCDx	ָ��Ҫ�����õ�LCD����Чֵ����LCD
* ��    ��: uint32_t			1 ������   0 �������
* ע������: ��
******************************************************************************************************************************************/
uint32_t MPULCD_DMABusy(LCD_TypeDef * LCDx)
{
	return (LCDx->START & LCD_START_GO_Msk) ? 1 : 0;
}
