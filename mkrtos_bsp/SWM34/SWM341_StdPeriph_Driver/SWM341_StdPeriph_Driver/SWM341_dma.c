/****************************************************************************************************************************************** 
* �ļ�����: SWM341_dma.c
* ����˵��:	SWM341��Ƭ����DMA����������
* ����֧��:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* ע������: 
* �汾����:	V1.0.0		2016��1��30��
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
#include "SWM341_dma.h"


/****************************************************************************************************************************************** 
* ��������: DMA_CH_Init()
* ����˵��:	DMAͨ����ʼ��
* ��    ��: uint32_t chn			ָ��Ҫ���õ�ͨ������Чֵ��DMA_CH0��DMA_CH1��DMA_CH2��DMA_CH3
*			DMA_InitStructure * initStruct    ����DMAͨ������趨ֵ�Ľṹ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void DMA_CH_Init(uint32_t chn, DMA_InitStructure * initStruct)
{
	DMA->EN = 1;			//ÿ��ͨ�������Լ������Ŀ��ؿ��ƣ������ܿ��ؿ�����һֱ������
	
	DMA_CH_Close(chn);		//�رպ�����
	
	DMA->CH[chn].CR = (initStruct->Mode << DMA_CR_AUTORE_Pos) | 
					 ((initStruct->Count ? initStruct->Count - 1 : 0) << DMA_CR_LEN_Pos);
	
	DMA->CH[chn].SRC = initStruct->SrcAddr;
	DMA->CH[chn].DST = initStruct->DstAddr;
	
	DMA->CH[chn].AM = (initStruct->SrcAddrInc << DMA_AM_SRCAM_Pos)  |
					  (initStruct->DstAddrInc << DMA_AM_DSTAM_Pos)  |
					  (initStruct->Unit       << DMA_AM_SRCBIT_Pos) |
					  (initStruct->Unit       << DMA_AM_DSTBIT_Pos);
	
	switch(initStruct->Handshake & DMA_HS_MSK)
	{
	case DMA_HS_NO:
		DMA->CH[chn].MUX = 0;
		break;
	
	case DMA_HS_SRC:
		DMA->CH[chn].MUX = ((initStruct->Handshake & 0xF) << DMA_MUX_SRCHSSIG_Pos) | (1 << DMA_MUX_SRCHSEN_Pos);
		break;
	
	case DMA_HS_DST:
		DMA->CH[chn].MUX = ((initStruct->Handshake & 0xF) << DMA_MUX_DSTHSSIG_Pos) | (1 << DMA_MUX_DSTHSEN_Pos);
		break;
	
	case DMA_HS_EXT:
		DMA->CH[chn].MUX = ((initStruct->Handshake & 0xF) << DMA_MUX_EXTHSSIG_Pos) | (1 << DMA_MUX_EXTHSEN_Pos);
		DMA->CH[chn].CR |= (1 << DMA_CR_STEPOP_Pos);
		break;
	
	default:
		break;
	}
	
	int totalBytes = initStruct->Count * (1 << initStruct->Unit);
	
	if(initStruct->DstAddrInc == 2)		// Destination Scatter-Gather Transfer
	{
		DMA->CH[chn].DSTSGADDR1 = initStruct->DstAddr + totalBytes / 4 * 1;
		DMA->CH[chn].DSTSGADDR2 = initStruct->DstAddr + totalBytes / 4 * 2;
		DMA->CH[chn].DSTSGADDR3 = initStruct->DstAddr + totalBytes / 4 * 3;
	}
	if(initStruct->SrcAddrInc == 2)		// Source      Scatter-Gather Transfer
	{
		DMA->CH[chn].SRCSGADDR1 = initStruct->SrcAddr + totalBytes / 4 * 1;
		DMA->CH[chn].SRCSGADDR2 = initStruct->SrcAddr + totalBytes / 4 * 2;
		DMA->CH[chn].SRCSGADDR3 = initStruct->SrcAddr + totalBytes / 4 * 3;
	}

	
	DMA->PRI &= ~(1 << chn);
	DMA->PRI |= (initStruct->Priority << chn);
	
	DMA_CH_INTClr(chn, initStruct->INTEn);
	DMA_CH_INTEn(chn, initStruct->INTEn);
	
	if(initStruct->INTEn) NVIC_EnableIRQ(DMA_IRQn);
}

/****************************************************************************************************************************************** 
* ��������: DMA_CH_Open()
* ����˵��:	DMAͨ�������������������ͨ�����������������䣻����Ӳ������ͨ������������ȳ��ִ����źź�ſ�ʼ����
* ��    ��: uint32_t chn			ָ��Ҫ���õ�ͨ������Чֵ��DMA_CH0��DMA_CH1��DMA_CH2��DMA_CH3
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void DMA_CH_Open(uint32_t chn)
{
	DMA->CH[chn].CR |= (1 << DMA_CR_RXEN_Pos);
}

/****************************************************************************************************************************************** 
* ��������: DMA_CH_Close()
* ����˵��:	DMAͨ���ر�
* ��    ��: uint32_t chn			ָ��Ҫ���õ�ͨ������Чֵ��DMA_CH0��DMA_CH1��DMA_CH2��DMA_CH3
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void DMA_CH_Close(uint32_t chn)
{
	DMA->CH[chn].CR &= ~(DMA_CR_TXEN_Msk | DMA_CR_RXEN_Msk);
}

/****************************************************************************************************************************************** 
* ��������: DMA_CH_SetCount()
* ����˵��: ���ô��� Unit ����
* ��    ��: uint32_t chn			ָ��Ҫ���õ�ͨ������Чֵ��DMA_CH0��DMA_CH1��DMA_CH2��DMA_CH3
*			uint32_t count			���� Unit ���������ȡֵ0x100000
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void DMA_CH_SetCount(uint32_t chn, uint32_t count)
{
	DMA->CH[chn].CR &= ~DMA_CR_LEN_Msk;
	DMA->CH[chn].CR |= ((count - 1) << DMA_CR_LEN_Pos);
}

/****************************************************************************************************************************************** 
* ��������: DMA_CH_GetRemaining()
* ����˵��: ��ѯʣ��Ĵ��� Unit ����
* ��    ��: uint32_t chn			ָ��Ҫ���õ�ͨ������Чֵ��DMA_CH0��DMA_CH1��DMA_CH2��DMA_CH3
* ��    ��: uint32_t				ʣ��Ĵ��� Unit ����
* ע������: ��
******************************************************************************************************************************************/
uint32_t DMA_CH_GetRemaining(uint32_t chn)
{
	return (DMA->CH[chn].DSTSR & DMA_DSTSR_LEN_Msk);
}

/****************************************************************************************************************************************** 
* ��������: DMA_CH_SetSrcAddress()
* ����˵��: ���ô���Դ��ַ
* ��    ��: uint32_t chn			ָ��Ҫ���õ�ͨ������Чֵ��DMA_CH0��DMA_CH1��DMA_CH2��DMA_CH3
*			uint32_t address		Դ��ַ
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void DMA_CH_SetSrcAddress(uint32_t chn, uint32_t address)
{
	DMA->CH[chn].SRC = address;
}

/****************************************************************************************************************************************** 
* ��������: DMA_CH_SetDstAddress()
* ����˵��: ���ô���Ŀ�ĵ�ַ
* ��    ��: uint32_t chn			ָ��Ҫ���õ�ͨ������Чֵ��DMA_CH0��DMA_CH1��DMA_CH2��DMA_CH3
*			uint32_t address		Ŀ�ĵ�ַ
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void DMA_CH_SetDstAddress(uint32_t chn, uint32_t address)
{
	DMA->CH[chn].DST = address;
}

/****************************************************************************************************************************************** 
* ��������: DMA_CH_INTEn()
* ����˵��:	DMA�ж�ʹ��
* ��    ��: uint32_t chn			ָ��Ҫ���õ�ͨ������Чֵ��DMA_CH0��DMA_CH1��DMA_CH2��DMA_CH3
*			uint32_t it				interrupt type����Чֵ�� DMA_IT_DONE��DMA_IT_DSTSG_HALF��DMA_IT_DSTSG_DONE��DMA_IT_SRCSG_HALF��
*									DMA_IT_SRCSG_DONE ���䡰��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void DMA_CH_INTEn(uint32_t chn, uint32_t it)
{
	DMA->IE |=  (it << chn);
	DMA->DSTSGIE |=  ((it >>  8) << (chn * 2));
	DMA->SRCSGIE |=  ((it >> 16) << (chn * 2));
	DMA->IM &= ~(it << chn);
	DMA->DSTSGIM &= ~((it >>  8) << (chn * 2));
	DMA->SRCSGIM &= ~((it >> 16) << (chn * 2));
}

/****************************************************************************************************************************************** 
* ��������: DMA_CH_INTDis()
* ����˵��:	DMA�жϽ�ֹ
* ��    ��: uint32_t chn			ָ��Ҫ���õ�ͨ������Чֵ��DMA_CH0��DMA_CH1��DMA_CH2��DMA_CH3
*			uint32_t it				interrupt type����Чֵ�� DMA_IT_DONE��DMA_IT_DSTSG_HALF��DMA_IT_DSTSG_DONE��DMA_IT_SRCSG_HALF��
*									DMA_IT_SRCSG_DONE ���䡰��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void DMA_CH_INTDis(uint32_t chn, uint32_t it)
{
	DMA->IE &= ~(it << chn);
	DMA->DSTSGIE &= ~((it >>  8) << (chn * 2));
	DMA->SRCSGIE &= ~((it >> 16) << (chn * 2));
	DMA->IM |=  (it << chn);
	DMA->DSTSGIM |=  ((it >>  8) << (chn * 2));
	DMA->SRCSGIM |=  ((it >> 16) << (chn * 2));
}

/****************************************************************************************************************************************** 
* ��������: DMA_CH_INTClr()
* ����˵��:	DMA�жϱ�־���
* ��    ��: uint32_t chn			ָ��Ҫ���õ�ͨ������Чֵ��DMA_CH0��DMA_CH1��DMA_CH2��DMA_CH3
*			uint32_t it				interrupt type����Чֵ�� DMA_IT_DONE��DMA_IT_DSTSG_HALF��DMA_IT_DSTSG_DONE��DMA_IT_SRCSG_HALF��
*									DMA_IT_SRCSG_DONE ���䡰��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void DMA_CH_INTClr(uint32_t chn, uint32_t it)
{
	DMA->IF = (it << chn);
	DMA->DSTSGIF = ((it >>  8) << (chn * 2));
	DMA->SRCSGIF = ((it >> 16) << (chn * 2));
}

/****************************************************************************************************************************************** 
* ��������: DMA_CH_INTStat()
* ����˵��:	DMA�ж�״̬��ѯ
* ��    ��: uint32_t chn			ָ��Ҫ���õ�ͨ������Чֵ��DMA_CH0��DMA_CH1��DMA_CH2��DMA_CH3
*			uint32_t it				interrupt type����Чֵ�� DMA_IT_DONE��DMA_IT_DSTSG_HALF��DMA_IT_DSTSG_DONE��DMA_IT_SRCSG_HALF��
*									DMA_IT_SRCSG_DONE ���䡰��
* ��    ��: uint32_t				1 ָ���ж��ѷ���    0 ָ���ж�δ����
* ע������: ��
******************************************************************************************************************************************/
uint32_t DMA_CH_INTStat(uint32_t chn, uint32_t it)
{
	return ((DMA->IF & (it << chn)) |
			(DMA->DSTSGIF & ((it >>  8) << (chn * 2))) |
			(DMA->SRCSGIF & ((it >> 16) << (chn * 2)))) ? 1 : 0;
}
