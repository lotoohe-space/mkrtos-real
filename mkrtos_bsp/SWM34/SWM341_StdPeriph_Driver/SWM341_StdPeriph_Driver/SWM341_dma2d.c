/******************************************************************************************************************************************
 * �ļ�����: SWM341_dma2d.c
 * ����˵��:	SWM341��Ƭ����DMA2D����������
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
#include "SWM341_dma2d.h"

#ifdef MKRTOS_DRV
#include <u_intr.h>
#include <assert.h>
obj_handler_t dma2d_irq_obj;
#endif
/******************************************************************************************************************************************
 * ��������:	DMA2D_Init()
 * ����˵��:	DMA2D��ʼ��
 * ��    ��: DMA2D_InitStructure * initStruct
 * ��    ��: ��
 * ע������: ��
 ******************************************************************************************************************************************/
void DMA2D_Init(DMA2D_InitStructure *initStruct)
{
	SYS->CLKEN0 |= (1 << SYS_CLKEN0_DMA2D_Pos);

	DMA2D->CR &= ~DMA2D_CR_WAIT_Msk;
	DMA2D->CR |= (initStruct->Interval << DMA2D_CR_WAIT_Pos);

	DMA2D->IF = 0xFF;
	DMA2D->IE = (initStruct->IntEOTEn << DMA2D_IE_DONE_Pos);

	if (initStruct->IntEOTEn)
	{
#ifdef MKRTOS_DRV
#define IRQ_THREAD_PRIO 16
#define STACK_SIZE (512 + 512)
		static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];
		static uint8_t msg_buf[128];
		extern void DMA2D_Handler(void);

		assert(u_intr_bind(DMA2D_IRQn, u_irq_prio_create(1, 0), IRQ_THREAD_PRIO,
						   stack0 + STACK_SIZE, msg_buf, DMA2D_Handler, &dma2d_irq_obj) >= 0);
#else
		NVIC_EnableIRQ(DMA2D_IRQn);
#endif
	}
}

/******************************************************************************************************************************************
 * ��������:	DMA2D_PixelFill()
 * ����˵��:	DMA2D��ָ���洢���������ָ����ɫ
 * ��    ��: DMA2D_LayerSetting * outLayer	Ҫ����λ�á���С����ɫ��ʽ��
 *			uint32_t color					Ҫ������ɫ
 * ��    ��: ��
 * ע������: ��
 ******************************************************************************************************************************************/
void DMA2D_PixelFill(DMA2D_LayerSetting *outLayer, uint32_t color)
{
	DMA2D->L[DMA2D_LAYER_OUT].COLOR = color;

	DMA2D->L[DMA2D_LAYER_OUT].MAR = outLayer->Address;
	DMA2D->L[DMA2D_LAYER_OUT].OR = outLayer->LineOffset;
	DMA2D->L[DMA2D_LAYER_OUT].PFCCR = (outLayer->ColorMode << DMA2D_PFCCR_CFMT_Pos);

	DMA2D->NLR = ((outLayer->LineCount - 1) << DMA2D_NLR_NLINE_Pos) |
				 ((outLayer->LinePixel - 1) << DMA2D_NLR_NPIXEL_Pos);

	DMA2D->CR &= ~DMA2D_CR_MODE_Msk;
	DMA2D->CR |= (3 << DMA2D_CR_MODE_Pos) |
				 (1 << DMA2D_CR_START_Pos);
}

/******************************************************************************************************************************************
 * ��������:	DMA2D_PixelMove()
 * ����˵��:	DMA2D�������ݰ���
 * ��    ��: DMA2D_LayerSetting * fgLayer
 *			DMA2D_LayerSetting * outLayer
 * ��    ��: ��
 * ע������: ��
 ******************************************************************************************************************************************/
void DMA2D_PixelMove(DMA2D_LayerSetting *fgLayer, DMA2D_LayerSetting *outLayer)
{
	DMA2D->L[DMA2D_LAYER_FG].MAR = fgLayer->Address;
	DMA2D->L[DMA2D_LAYER_FG].OR = fgLayer->LineOffset;
	DMA2D->L[DMA2D_LAYER_FG].PFCCR = (fgLayer->ColorMode << DMA2D_PFCCR_CFMT_Pos);

	DMA2D->L[DMA2D_LAYER_OUT].MAR = outLayer->Address;
	DMA2D->L[DMA2D_LAYER_OUT].OR = outLayer->LineOffset;

	DMA2D->NLR = ((outLayer->LineCount - 1) << DMA2D_NLR_NLINE_Pos) |
				 ((outLayer->LinePixel - 1) << DMA2D_NLR_NPIXEL_Pos);

	DMA2D->CR &= ~DMA2D_CR_MODE_Msk;
	DMA2D->CR |= (0 << DMA2D_CR_MODE_Pos) |
				 (1 << DMA2D_CR_START_Pos);
}

/******************************************************************************************************************************************
 * ��������:	DMA2D_PixelConvert()
 * ����˵��:	DMA2D����ת��
 * ��    ��: DMA2D_LayerSetting * fgLayer
 *			DMA2D_LayerSetting * outLayer
 * ��    ��: ��
 * ע������: ��
 ******************************************************************************************************************************************/
void DMA2D_PixelConvert(DMA2D_LayerSetting *fgLayer, DMA2D_LayerSetting *outLayer)
{
	DMA2D->L[DMA2D_LAYER_FG].MAR = fgLayer->Address;
	DMA2D->L[DMA2D_LAYER_FG].OR = fgLayer->LineOffset;
	DMA2D->L[DMA2D_LAYER_FG].PFCCR = (fgLayer->ColorMode << DMA2D_PFCCR_CFMT_Pos);

	DMA2D->L[DMA2D_LAYER_OUT].MAR = outLayer->Address;
	DMA2D->L[DMA2D_LAYER_OUT].OR = outLayer->LineOffset;
	DMA2D->L[DMA2D_LAYER_OUT].PFCCR = (outLayer->ColorMode << DMA2D_PFCCR_CFMT_Pos);

	DMA2D->NLR = ((outLayer->LineCount - 1) << DMA2D_NLR_NLINE_Pos) |
				 ((outLayer->LinePixel - 1) << DMA2D_NLR_NPIXEL_Pos);

	DMA2D->CR &= ~DMA2D_CR_MODE_Msk;
	DMA2D->CR |= (1 << DMA2D_CR_MODE_Pos) |
				 (1 << DMA2D_CR_START_Pos);
}

/******************************************************************************************************************************************
 * ��������:	DMA2D_PixelBlend()
 * ����˵��:	DMA2D���ػ��
 * ��    ��: DMA2D_LayerSetting * fgLayer
 *			DMA2D_LayerSetting * bgLayer
 *			DMA2D_LayerSetting * outLayer
 * ��    ��: ��
 * ע������: ��
 ******************************************************************************************************************************************/
void DMA2D_PixelBlend(DMA2D_LayerSetting *fgLayer, DMA2D_LayerSetting *bgLayer, DMA2D_LayerSetting *outLayer)
{
	DMA2D->L[DMA2D_LAYER_FG].MAR = fgLayer->Address;
	DMA2D->L[DMA2D_LAYER_FG].OR = fgLayer->LineOffset;
	DMA2D->L[DMA2D_LAYER_FG].PFCCR = (fgLayer->ColorMode << DMA2D_PFCCR_CFMT_Pos) |
									 (fgLayer->AlphaMode << DAM2D_PFCCR_AMODE_Pos) |
									 (fgLayer->Alpha << DMA2D_PFCCR_ALPHA_Pos);

	DMA2D->L[DMA2D_LAYER_BG].MAR = bgLayer->Address;
	DMA2D->L[DMA2D_LAYER_BG].OR = bgLayer->LineOffset;
	DMA2D->L[DMA2D_LAYER_BG].PFCCR = (bgLayer->ColorMode << DMA2D_PFCCR_CFMT_Pos) |
									 (bgLayer->AlphaMode << DAM2D_PFCCR_AMODE_Pos) |
									 (bgLayer->Alpha << DMA2D_PFCCR_ALPHA_Pos);

	DMA2D->L[DMA2D_LAYER_OUT].MAR = outLayer->Address;
	DMA2D->L[DMA2D_LAYER_OUT].OR = outLayer->LineOffset;
	DMA2D->L[DMA2D_LAYER_OUT].PFCCR = (outLayer->ColorMode << DMA2D_PFCCR_CFMT_Pos);

	DMA2D->NLR = ((outLayer->LineCount - 1) << DMA2D_NLR_NLINE_Pos) |
				 ((outLayer->LinePixel - 1) << DMA2D_NLR_NPIXEL_Pos);

	DMA2D->CR &= ~DMA2D_CR_MODE_Msk;
	DMA2D->CR |= (2 << DMA2D_CR_MODE_Pos) |
				 (1 << DMA2D_CR_START_Pos);
}

/******************************************************************************************************************************************
 * ��������:	DMA2D_IsBusy()
 * ����˵��:	DMA2Dæ��ѯ
 * ��    ��: ��
 * ��    ��: uint32_t				1 ���ڴ���   0 �������
 * ע������: ��
 ******************************************************************************************************************************************/
uint32_t DMA2D_IsBusy(void)
{
	return (DMA2D->CR & DMA2D_CR_START_Msk) ? 1 : 0;
}

/******************************************************************************************************************************************
 * ��������: DMA2D_INTEn()
 * ����˵��:	DMA2D�ж�ʹ�ܣ����ָ�����ȵ����ݴ���ʱ�����ж�
 * ��    ��: ��
 * ��    ��: ��
 * ע������: ��
 ******************************************************************************************************************************************/
void DMA2D_INTEn(void)
{
	DMA2D->IE = DMA2D_IE_DONE_Msk;
}

/******************************************************************************************************************************************
 * ��������: DMA2D_INTDis()
 * ����˵��:	DMA2D�жϽ�ֹ�����ָ�����ȵ����ݴ���ʱ�������ж�
 * ��    ��: ��
 * ��    ��: ��
 * ע������: ��
 ******************************************************************************************************************************************/
void DMA2D_INTDis(void)
{
	DMA2D->IE = 0;
}

/******************************************************************************************************************************************
 * ��������: DMA2D_INTClr()
 * ����˵��:	DMA2D�жϱ�־���
 * ��    ��: ��
 * ��    ��: ��
 * ע������: ��
 ******************************************************************************************************************************************/
void DMA2D_INTClr(void)
{
	DMA2D->IF = DMA2D_IF_DONE_Msk;
}

/******************************************************************************************************************************************
 * ��������: DMA2D_INTStat()
 * ����˵��:	DMA2D�ж�״̬��ѯ
 * ��    ��: ��
 * ��    ��: uint32_t			0 δ���ָ�����ȵ����ݴ���   1 ���ָ�����ȵ����ݴ���
 * ע������: ��
 ******************************************************************************************************************************************/
uint32_t DMA2D_INTStat(void)
{
	return (DMA2D->IF & DMA2D_IF_DONE_Msk) ? 1 : 0;
}
