/****************************************************************************************************************************************** 
* �ļ�����: SWM341_timr.c
* ����˵��:	SWM341��Ƭ���ļ�����/��ʱ������������
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
#include "SWM341_timr.h"


/****************************************************************************************************************************************** 
* ��������: TIMR_Init()
* ����˵��:	TIMR��ʱ��/��������ʼ��
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������Чֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
*			uint32_t mode			����TIMR0~4: TIMR_MODE_TIMER��TIMR_MODE_COUNTER��TIMR_MODE_OC��TIMR_MODE_IC
*									����BTIMR0~11: TIMR_MODE_TIMER��TIMR_MODE_OC
*			uint16_t prediv			Ԥ��Ƶ��ȡֵ1-256
*			uint32_t period			��ʱ/�������ڣ�ȡֵ1-16777216
*			uint32_t int_en			�ж�ʹ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_Init(TIMR_TypeDef * TIMRx, uint32_t mode, uint16_t prediv, uint32_t period, uint32_t int_en)
{
	if((TIMRx == TIMR0) || (TIMRx == TIMR1) || (TIMRx == TIMR2) || (TIMRx == TIMR3) || (TIMRx == TIMR4))
	{
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_TIMR_Pos);
	}
	else
	{
		SYS->CLKEN1 |= (0x01 << SYS_CLKEN1_BTIMR_Pos);
	}
	
	TIMR_Stop(TIMRx);	//һЩ�ؼ��Ĵ���ֻ���ڶ�ʱ��ֹͣʱ����
	
	TIMRx->CR &= ~(TIMR_CR_MODE_Msk | TIMR_CR_CLKSRC_Msk);
	TIMRx->CR |= (mode << TIMR_CR_CLKSRC_Pos);
	
	TIMRx->PREDIV = prediv - 1;
	
	TIMRx->LOAD = period - 1;
	
	TIMRx->IF = (1 << TIMR_IF_TO_Pos);		//����жϱ�־
	if(int_en) TIMRx->IE |=  (1 << TIMR_IE_TO_Pos);
	else	   TIMRx->IE &= ~(1 << TIMR_IE_TO_Pos);
	
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		if(int_en) NVIC_EnableIRQ(TIMR0_IRQn);
		break;
	
	case ((uint32_t)TIMR1):
		if(int_en) NVIC_EnableIRQ(TIMR1_IRQn);
		break;
	
	case ((uint32_t)TIMR2):
		if(int_en) NVIC_EnableIRQ(TIMR2_IRQn);
		break;
	
	case ((uint32_t)TIMR3):
		if(int_en) NVIC_EnableIRQ(TIMR3_IRQn);
		break;
	
	case ((uint32_t)TIMR4):
		if(int_en) NVIC_EnableIRQ(TIMR4_IRQn);
		break;
	
	case ((uint32_t)BTIMR0):
		if(int_en) NVIC_EnableIRQ(BTIMR0_IRQn);
		break;
	
	case ((uint32_t)BTIMR1):
		if(int_en) NVIC_EnableIRQ(BTIMR1_IRQn);
		break;
	
	case ((uint32_t)BTIMR2):
		if(int_en) NVIC_EnableIRQ(BTIMR2_IRQn);
		break;
	
	case ((uint32_t)BTIMR3):
		if(int_en) NVIC_EnableIRQ(BTIMR3_IRQn);
		break;
	
	case ((uint32_t)BTIMR4):
		if(int_en) NVIC_EnableIRQ(BTIMR4_IRQn);
		break;
	
	case ((uint32_t)BTIMR5):
		if(int_en) NVIC_EnableIRQ(BTIMR5_IRQn);
		break;
	
	case ((uint32_t)BTIMR6):
		if(int_en) NVIC_EnableIRQ(BTIMR6_IRQn);
		break;
	
	case ((uint32_t)BTIMR7):
		if(int_en) NVIC_EnableIRQ(BTIMR7_IRQn);
		break;
	
	case ((uint32_t)BTIMR8):
		if(int_en) NVIC_EnableIRQ(BTIMR8_IRQn);
		break;
	
	case ((uint32_t)BTIMR9):
		if(int_en) NVIC_EnableIRQ(BTIMR9_IRQn);
		break;
	
	case ((uint32_t)BTIMR10):
		if(int_en) NVIC_EnableIRQ(BTIMR10_IRQn);
		break;
	
	case ((uint32_t)BTIMR11):
		if(int_en) NVIC_EnableIRQ(BTIMR11_IRQn);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_Start()
* ����˵��:	������ʱ�����ӳ�ʼֵ��ʼ��ʱ/����
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_Start(TIMR_TypeDef * TIMRx)
{
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		TIMRG->EN |= (1 << TIMRG_EN_TIMR0_Pos);
		break;
	
	case ((uint32_t)TIMR1):
		TIMRG->EN |= (1 << TIMRG_EN_TIMR1_Pos);
		break;
	
	case ((uint32_t)TIMR2):
		TIMRG->EN |= (1 << TIMRG_EN_TIMR2_Pos);
		break;
	
	case ((uint32_t)TIMR3):
		TIMRG->EN |= (1 << TIMRG_EN_TIMR3_Pos);
		break;
	
	case ((uint32_t)TIMR4):
		TIMRG->EN |= (1 << TIMRG_EN_TIMR4_Pos);
		break;
	
	case ((uint32_t)BTIMR0):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR0_Pos);
		break;
	
	case ((uint32_t)BTIMR1):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR1_Pos);
		break;
	
	case ((uint32_t)BTIMR2):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR2_Pos);
		break;
	
	case ((uint32_t)BTIMR3):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR3_Pos);
		break;
	
	case ((uint32_t)BTIMR4):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR4_Pos);
		break;
	
	case ((uint32_t)BTIMR5):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR5_Pos);
		break;
	
	case ((uint32_t)BTIMR6):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR6_Pos);
		break;
	
	case ((uint32_t)BTIMR7):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR7_Pos);
		break;
	
	case ((uint32_t)BTIMR8):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR8_Pos);
		break;
	
	case ((uint32_t)BTIMR9):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR9_Pos);
		break;
	
	case ((uint32_t)BTIMR10):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR10_Pos);
		break;
	
	case ((uint32_t)BTIMR11):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR11_Pos);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_Stop()
* ����˵��:	ֹͣ��ʱ��
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_Stop(TIMR_TypeDef * TIMRx)
{
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		TIMRG->EN &= ~(1 << TIMRG_EN_TIMR0_Pos);
		break;
	
	case ((uint32_t)TIMR1):
		TIMRG->EN &= ~(1 << TIMRG_EN_TIMR1_Pos);
		break;
	
	case ((uint32_t)TIMR2):
		TIMRG->EN &= ~(1 << TIMRG_EN_TIMR2_Pos);
		break;
	
	case ((uint32_t)TIMR3):
		TIMRG->EN &= ~(1 << TIMRG_EN_TIMR3_Pos);
		break;
	
	case ((uint32_t)TIMR4):
		TIMRG->EN &= ~(1 << TIMRG_EN_TIMR4_Pos);
		break;
	
	case ((uint32_t)BTIMR0):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR0_Pos);
		break;
	
	case ((uint32_t)BTIMR1):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR1_Pos);
		break;
	
	case ((uint32_t)BTIMR2):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR2_Pos);
		break;
	
	case ((uint32_t)BTIMR3):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR3_Pos);
		break;
	
	case ((uint32_t)BTIMR4):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR4_Pos);
		break;
	
	case ((uint32_t)BTIMR5):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR5_Pos);
		break;
	
	case ((uint32_t)BTIMR6):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR6_Pos);
		break;
	
	case ((uint32_t)BTIMR7):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR7_Pos);
		break;
	
	case ((uint32_t)BTIMR8):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR8_Pos);
		break;
	
	case ((uint32_t)BTIMR9):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR9_Pos);
		break;
	
	case ((uint32_t)BTIMR10):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR10_Pos);
		break;
	
	case ((uint32_t)BTIMR11):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR11_Pos);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_Halt()
* ����˵��:	��ͣ��ʱ��������ֵ���ֲ���
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_Halt(TIMR_TypeDef * TIMRx)
{
	TIMRx->HALT = 1;
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_Resume()
* ����˵��:	�ָ���ʱ��������ͣ����������
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_Resume(TIMR_TypeDef * TIMRx)
{
	TIMRx->HALT = 0;
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_GetCurValue()
* ����˵��:	��ȡ��ǰ����ֵ
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: uint32_t				��ǰ����ֵ
* ע������: ��
******************************************************************************************************************************************/
uint32_t TIMR_GetCurValue(TIMR_TypeDef * TIMRx)
{
	return TIMRx->VALUE;
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_INTEn()
* ����˵��:	ʹ���ж�
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_INTEn(TIMR_TypeDef * TIMRx)
{
	TIMRx->IE |= (1 << TIMR_IE_TO_Pos);
}

/****************************************************************************************************************************************** 
* ��������: TIMR_INTDis()
* ����˵��:	�����ж�
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_INTDis(TIMR_TypeDef * TIMRx)
{
	TIMRx->IE &= ~(1 << TIMR_IE_TO_Pos);
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_INTClr()
* ����˵��:	����жϱ�־
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_INTClr(TIMR_TypeDef * TIMRx)
{
	TIMRx->IF = (1 << TIMR_IF_TO_Pos);
}

/****************************************************************************************************************************************** 
* ��������: TIMR_INTStat()
* ����˵��:	��ȡ�ж�״̬
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: uint32_t 				0 TIMRxδ�����ж�    1 TIMRx�������ж�
* ע������: ��
******************************************************************************************************************************************/
uint32_t TIMR_INTStat(TIMR_TypeDef * TIMRx)
{
	return (TIMRx->IF & TIMR_IF_TO_Msk) ? 1 : 0;
}


/****************************************************************************************************************************************** 
* ��������: TIMR_OC_Init()
* ����˵��:	����ȽϹ��ܳ�ʼ��
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
*			uint32_t match			����������ֵ�ݼ���matchʱ���������ƽ��ת
*			uint32_t match_int_en	����������ֵ�ݼ���matchʱ�Ƿ�����ж�
*			uint32_t init_lvl		��ʼ�����ƽ
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_OC_Init(TIMR_TypeDef * TIMRx, uint32_t match, uint32_t match_int_en, uint32_t init_lvl)
{
	TIMRx->OCMAT = match;
	if(init_lvl) TIMRx->OCCR |=  (1 << TIMR_OCCR_INITLVL_Pos);
	else         TIMRx->OCCR &= ~(1 << TIMR_OCCR_INITLVL_Pos);
	
	TIMRx->IF = (1 << TIMR_IF_OC0_Pos);		//����жϱ�־
	if(match_int_en) TIMRx->IE |=  (1 << TIMR_IE_OC0_Pos);
	else             TIMRx->IE &= ~(1 << TIMR_IE_OC0_Pos);
	
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		if(match_int_en) NVIC_EnableIRQ(TIMR0_IRQn);
		break;
	
	case ((uint32_t)TIMR1):
		if(match_int_en) NVIC_EnableIRQ(TIMR1_IRQn);
		break;
	
	case ((uint32_t)TIMR2):
		if(match_int_en) NVIC_EnableIRQ(TIMR2_IRQn);
		break;
	
	case ((uint32_t)TIMR3):
		if(match_int_en) NVIC_EnableIRQ(TIMR3_IRQn);
		break;
	
	case ((uint32_t)TIMR4):
		if(match_int_en) NVIC_EnableIRQ(TIMR4_IRQn);
		break;
	
	case ((uint32_t)BTIMR0):
		if(match_int_en) NVIC_EnableIRQ(BTIMR0_IRQn);
		break;
	
	case ((uint32_t)BTIMR1):
		if(match_int_en) NVIC_EnableIRQ(BTIMR1_IRQn);
		break;
	
	case ((uint32_t)BTIMR2):
		if(match_int_en) NVIC_EnableIRQ(BTIMR2_IRQn);
		break;
	
	case ((uint32_t)BTIMR3):
		if(match_int_en) NVIC_EnableIRQ(BTIMR3_IRQn);
		break;
	
	case ((uint32_t)BTIMR4):
		if(match_int_en) NVIC_EnableIRQ(BTIMR4_IRQn);
		break;
	
	case ((uint32_t)BTIMR5):
		if(match_int_en) NVIC_EnableIRQ(BTIMR5_IRQn);
		break;
	
	case ((uint32_t)BTIMR6):
		if(match_int_en) NVIC_EnableIRQ(BTIMR6_IRQn);
		break;
	
	case ((uint32_t)BTIMR7):
		if(match_int_en) NVIC_EnableIRQ(BTIMR7_IRQn);
		break;
	
	case ((uint32_t)BTIMR8):
		if(match_int_en) NVIC_EnableIRQ(BTIMR8_IRQn);
		break;
	
	case ((uint32_t)BTIMR9):
		if(match_int_en) NVIC_EnableIRQ(BTIMR9_IRQn);
		break;
	
	case ((uint32_t)BTIMR10):
		if(match_int_en) NVIC_EnableIRQ(BTIMR10_IRQn);
		break;
	
	case ((uint32_t)BTIMR11):
		if(match_int_en) NVIC_EnableIRQ(BTIMR11_IRQn);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_OC_OutputEn()
* ����˵��:	ʹ������ȽϹ��ܵĲ������
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_OC_OutputEn(TIMR_TypeDef * TIMRx)
{
	TIMRx->OCCR &= ~(TIMR_OCCR_FORCEEN_Pos);
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_OC_OutputDis()
* ����˵��:	��ֹ����ȽϹ��ܵĲ����������������ȽϹ������ű���level��ƽ
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
*			uint32_t level			��ֹ������κ��������ϱ��ֵĵ�ƽ
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_OC_OutputDis(TIMR_TypeDef * TIMRx, uint32_t level)
{
	if(level) TIMRx->OCCR |=  (1 << TIMR_OCCR_FORCELVL_Pos);
	else      TIMRx->OCCR &= ~(1 << TIMR_OCCR_FORCELVL_Pos);
	
	TIMRx->OCCR |= (TIMR_OCCR_FORCEEN_Pos);
}

/****************************************************************************************************************************************** 
* ��������: TIMR_OC_SetMatch()
* ����˵��:	��������ȽϹ��ܵıȽ�ֵ
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
*			uint32_t match			����ȽϹ��ܵıȽ�ֵ
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_OC_SetMatch(TIMR_TypeDef * TIMRx, uint32_t match)
{
	TIMRx->OCMAT = match;
}

/****************************************************************************************************************************************** 
* ��������: TIMR_OC_GetMatch()
* ����˵��:	��ȡ����ȽϹ��ܵıȽ�ֵ
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: uint32_t				����ȽϹ��ܵıȽ�ֵ
* ע������: ��
******************************************************************************************************************************************/
uint32_t TIMR_OC_GetMatch(TIMR_TypeDef * TIMRx)
{	
	return TIMRx->OCMAT;
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_OC_INTEn()
* ����˵��:	ʹ������Ƚ��ж�
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_OC_INTEn(TIMR_TypeDef * TIMRx)
{
	TIMRx->IE |= (1 << TIMR_IE_OC0_Pos);
}

/****************************************************************************************************************************************** 
* ��������: TIMR_OC_INTDis()
* ����˵��:	��������Ƚ��ж�
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_OC_INTDis(TIMR_TypeDef * TIMRx)
{
	TIMRx->IE &= ~(1 << TIMR_IE_OC0_Pos);
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_OC_INTClr()
* ����˵��:	�������Ƚ��жϱ�־
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_OC_INTClr(TIMR_TypeDef * TIMRx)
{
	TIMRx->IF = (1 << TIMR_IF_OC0_Pos);
}

/****************************************************************************************************************************************** 
* ��������: TIMR_OC_INTStat()
* ����˵��:	��ȡ����Ƚ��ж�״̬
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4��BTIMR0��BTIMR1��...��BTIMR11
* ��    ��: uint32_t 				0 ����Ƚ�matchδ����   1 ����Ƚ�match����
* ע������: ��
******************************************************************************************************************************************/
uint32_t TIMR_OC_INTStat(TIMR_TypeDef * TIMRx)
{
	return (TIMRx->IF & TIMR_IF_OC0_Msk) ? 1 : 0;
}

/****************************************************************************************************************************************** 
* ��������: TIMR_IC_Init()
* ����˵��:	���벶���ܳ�ʼ��
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
*			uint32_t captureH_int_en	�����ߵ�ƽ��������ж�ʹ��
*			uint32_t captureL_int_en	�����͵�ƽ��������ж�ʹ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_IC_Init(TIMR_TypeDef * TIMRx, uint32_t captureH_int_en, uint32_t captureL_int_en)
{
	TIMRx->IF = (TIMR_IF_ICR_Msk | TIMR_IF_ICF_Msk);
	if(captureH_int_en) TIMRx->IE |=  (1 << TIMR_IE_ICF_Pos);
	else                TIMRx->IE &= ~(1 << TIMR_IE_ICF_Pos);
	if(captureL_int_en) TIMRx->IE |=  (1 << TIMR_IE_ICR_Pos);
	else                TIMRx->IE &= ~(1 << TIMR_IE_ICR_Pos);
	
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		if(captureH_int_en | captureL_int_en) NVIC_EnableIRQ(TIMR0_IRQn);
		break;
	
	case ((uint32_t)TIMR1):
		if(captureH_int_en | captureL_int_en) NVIC_EnableIRQ(TIMR1_IRQn);
		break;
	
	case ((uint32_t)TIMR2):
		if(captureH_int_en | captureL_int_en) NVIC_EnableIRQ(TIMR2_IRQn);
		break;
	
	case ((uint32_t)TIMR3):
		if(captureH_int_en | captureL_int_en) NVIC_EnableIRQ(TIMR3_IRQn);
		break;
	
	case ((uint32_t)TIMR4):
		if(captureH_int_en | captureL_int_en) NVIC_EnableIRQ(TIMR4_IRQn);
		break;
	}
}

/****************************************************************************************************************************************** 
* ��������: TIMR_IC_GetCaptureH()
* ����˵��:	��ȡ�ߵ�ƽ���Ȳ������
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
* ��    ��: uint32_t				�ߵ�ƽ���Ȳ������
* ע������: ��
******************************************************************************************************************************************/
uint32_t TIMR_IC_GetCaptureH(TIMR_TypeDef * TIMRx)
{
	return TIMRx->ICHIGH;
}

/****************************************************************************************************************************************** 
* ��������: TIMR_IC_GetCaptureL()
* ����˵��:	��ȡ�͵�ƽ���Ȳ������
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
* ��    ��: uint32_t				�͵�ƽ���Ȳ������
* ע������: ��
******************************************************************************************************************************************/
uint32_t TIMR_IC_GetCaptureL(TIMR_TypeDef * TIMRx)
{
	return TIMRx->ICLOW;
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_IC_CaptureH_INTEn()
* ����˵��:	ʹ�����벶��ߵ�ƽ���Ȳ�������ж�
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_IC_CaptureH_INTEn(TIMR_TypeDef * TIMRx)
{
	TIMRx->IE |= (1 << TIMR_IE_ICF_Pos);
}

/****************************************************************************************************************************************** 
* ��������: TIMR_IC_CaptureH_INTDis()
* ����˵��:	�������벶��ߵ�ƽ���Ȳ�������ж�
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_IC_CaptureH_INTDis(TIMR_TypeDef * TIMRx)
{
	TIMRx->IE &= ~(1 << TIMR_IE_ICF_Pos);
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_IC_CaptureH_INTClr()
* ����˵��:	������벶��ߵ�ƽ���Ȳ�������жϱ�־
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_IC_CaptureH_INTClr(TIMR_TypeDef * TIMRx)
{
	TIMRx->IF = (1 << TIMR_IF_ICF_Pos);
}

/****************************************************************************************************************************************** 
* ��������: TIMR_IC_CaptureH_INTStat()
* ����˵��:	��ȡ���벶��ߵ�ƽ���Ȳ�������ж�״̬
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
* ��    ��: uint32_t 				0 �ߵ�ƽ���Ȳ���δ���    1 �ߵ�ƽ���Ȳ������
* ע������: ��
******************************************************************************************************************************************/
uint32_t TIMR_IC_CaptureH_INTStat(TIMR_TypeDef * TIMRx)
{
	return (TIMRx->IF & TIMR_IF_ICF_Msk) ? 1 : 0;
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_IC_CaptureL_INTEn()
* ����˵��:	ʹ�����벶��͵�ƽ���Ȳ�������ж�
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_IC_CaptureL_INTEn(TIMR_TypeDef * TIMRx)
{
	TIMRx->IE |= (1 << TIMR_IE_ICR_Pos);
}

/****************************************************************************************************************************************** 
* ��������: TIMR_IC_CaptureL_INTDis()
* ����˵��:	�������벶��͵�ƽ���Ȳ�������ж�
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_IC_CaptureL_INTDis(TIMR_TypeDef * TIMRx)
{
	TIMRx->IE &= ~(1 << TIMR_IE_ICR_Pos);
}

/****************************************************************************************************************************************** 
* ��������:	TIMR_IC_CaptureL_INTClr()
* ����˵��:	������벶��͵�ƽ���Ȳ�������жϱ�־
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TIMR_IC_CaptureL_INTClr(TIMR_TypeDef * TIMRx)
{
	TIMRx->IF = (1 << TIMR_IF_ICR_Pos);
}

/****************************************************************************************************************************************** 
* ��������: TIMR_IC_CaptureL_INTStat()
* ����˵��:	��ȡ���벶��͵�ƽ���Ȳ�������ж�״̬
* ��    ��: TIMR_TypeDef * TIMRx	ָ��Ҫ�����õĶ�ʱ������ȡֵ����TIMR0��TIMR1��TIMR2��TIMR3��TIMR4
* ��    ��: uint32_t 				0 �͵�ƽ���Ȳ���δ���    1 �͵�ƽ���Ȳ������
* ע������: ��
******************************************************************************************************************************************/
uint32_t TIMR_IC_CaptureL_INTStat(TIMR_TypeDef * TIMRx)
{
	return (TIMRx->IF & TIMR_IF_ICR_Msk) ? 1 : 0;
}
