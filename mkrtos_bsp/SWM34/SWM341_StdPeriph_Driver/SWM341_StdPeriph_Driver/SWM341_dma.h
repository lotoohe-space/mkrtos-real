#ifndef __SWM341_DMA_H__
#define __SWM341_DMA_H__


typedef struct {
	uint8_t  Mode;			//DMA_MODE_SINGLE��DMA_MODE_CIRCLE
	
	uint8_t  Unit;			//DMA_UNIT_BYTE��DMA_UNIT_HALFWORD��DMA_UNIT_WORD
	
	uint32_t Count;			//���� Unit ���������ȡֵ0x100000
	
	uint32_t SrcAddr;
	
	uint32_t DstAddr;
	
	uint8_t  SrcAddrInc;	//0 ��ַ�̶�    1 ��ַ����
	
	uint8_t  DstAddrInc;
	
	uint8_t  Handshake;		//���������źţ�DMA_HS_NO��DMA_CH0_UART0TX��DMA_CH0_SPI0TX��... ...
	
	uint8_t  Priority;		//DMA_PRI_LOW��DMA_PRI_HIGH
	
	uint32_t  INTEn;		//�ж�ʹ�ܣ���Чֵ�� DMA_IT_DONE��DMA_IT_DSTSG_HALF��DMA_IT_DSTSG_DONE��DMA_IT_SRCSG_HALF��DMA_IT_SRCSG_DONE ���䡰��
} DMA_InitStructure;


#define DMA_CH0		0
#define DMA_CH1		1
#define DMA_CH2		2
#define DMA_CH3		3

#define DMA_MODE_SINGLE			0		// ����ģʽ��������ɺ�ֹͣ
#define DMA_MODE_CIRCLE			1		// ����ģʽ��������ɺ��ͷִ����һ�ִ���

#define DMA_UNIT_BYTE		0
#define DMA_UNIT_HALFWORD	1
#define DMA_UNIT_WORD		2

#define DMA_PRI_LOW			0
#define DMA_PRI_HIGH		1


#define DMA_HS_NO			(0 << 4)	// �����֣�Handshake���źţ����������ȫ������
#define DMA_HS_SRC			(1 << 4)	// Դ  �������źţ����������Դ  ������һ�����ݣ�DMA����һ��Unit
#define DMA_HS_DST			(2 << 4)	// Ŀ��������źţ����������Ŀ�����ȡһ�����ݣ�DMA����һ��Unit
#define DMA_HS_EXT			(4 << 4)	// �ⲿ  �����źţ�����������ⲿ������һ�����壬DMA����һ��Unit
#define DMA_HS_MSK			(7 << 4)

#define DMA_DIR_RX			(0 << 7)	// SRC --> DST
#define DMA_DIR_TX			(1 << 7)	// DST --> SRC
#define DMA_DIR_MSK			(1 << 7)

// Դ������
#define DMA_CH0_UART1RX		(0 | DMA_HS_SRC | DMA_DIR_RX)
#define DMA_CH0_SPI1RX		(1 | DMA_HS_SRC | DMA_DIR_RX)
#define DMA_CH0_UART2RX		(2 | DMA_HS_SRC | DMA_DIR_RX)
#define DMA_CH0_ADC0		(3 | DMA_HS_SRC | DMA_DIR_RX)

#define DMA_CH1_UART0RX		(0 | DMA_HS_SRC | DMA_DIR_RX)
#define DMA_CH1_SPI0RX		(1 | DMA_HS_SRC | DMA_DIR_RX)
#define DMA_CH1_UART3RX		(2 | DMA_HS_SRC | DMA_DIR_RX)

#define DMA_CH2_UART3RX		(0 | DMA_HS_SRC | DMA_DIR_RX)
#define DMA_CH2_SPI0RX		(1 | DMA_HS_SRC | DMA_DIR_RX)
#define DMA_CH2_UART0RX		(2 | DMA_HS_SRC | DMA_DIR_RX)

#define DMA_CH3_UART2RX		(0 | DMA_HS_SRC | DMA_DIR_RX)
#define DMA_CH3_SPI1RX		(1 | DMA_HS_SRC | DMA_DIR_RX)
#define DMA_CH3_UART1RX		(2 | DMA_HS_SRC | DMA_DIR_RX)
#define DMA_CH3_ADC1		(3 | DMA_HS_SRC | DMA_DIR_RX)

// Ŀ�������
#define DMA_CH0_UART0TX		(0 | DMA_HS_DST | DMA_DIR_RX)
#define DMA_CH0_SPI0TX		(1 | DMA_HS_DST | DMA_DIR_RX)
#define DMA_CH0_UART3TX		(2 | DMA_HS_DST | DMA_DIR_RX)

#define DMA_CH1_UART1TX		(0 | DMA_HS_DST | DMA_DIR_RX)
#define DMA_CH1_SPI1TX		(1 | DMA_HS_DST | DMA_DIR_RX)
#define DMA_CH1_UART2TX		(2 | DMA_HS_DST | DMA_DIR_RX)

#define DMA_CH2_UART2TX		(0 | DMA_HS_DST | DMA_DIR_RX)
#define DMA_CH2_SPI1TX		(1 | DMA_HS_DST | DMA_DIR_RX)
#define DMA_CH2_UART1TX		(2 | DMA_HS_DST | DMA_DIR_RX)

#define DMA_CH3_UART3TX		(0 | DMA_HS_DST | DMA_DIR_RX)
#define DMA_CH3_SPI0TX		(1 | DMA_HS_DST | DMA_DIR_RX)
#define DMA_CH3_UART0TX		(2 | DMA_HS_DST | DMA_DIR_RX)

// �ⲿ�����ź�
#define DMA_EXHS_TIMR0		(0 | DMA_HS_EXT | DMA_DIR_RX)
#define DMA_EXHS_TIMR1		(1 | DMA_HS_EXT | DMA_DIR_RX)
#define DMA_EXHS_TIMR2		(2 | DMA_HS_EXT | DMA_DIR_RX)
#define DMA_EXHS_TIMR3		(3 | DMA_HS_EXT | DMA_DIR_RX)
#define DMA_EXHS_TIMR4		(4 | DMA_HS_EXT | DMA_DIR_RX)
#define DMA_EXHS_TRIG0		(5 | DMA_HS_EXT | DMA_DIR_RX)	// DMA_TRIG0����
#define DMA_EXHS_TRIG1		(6 | DMA_HS_EXT | DMA_DIR_RX)	// DMA_TRIG1����


/* Interrupt Type */
#define DMA_IT_DONE			(1 <<  0)	//Transfer Done
#define DMA_IT_DSTSG_HALF	(1 <<  8)	//Destination Scatter-Gather Transfer Half
#define DMA_IT_DSTSG_DONE	(1 <<  9)	//Destination Scatter-Gather Transfer Done
#define DMA_IT_SRCSG_HALF	(1 << 16)	//Source      Scatter-Gather Transfer Half
#define DMA_IT_SRCSG_DONE	(1 << 17)	//Source      Scatter-Gather Transfer Done



void DMA_CH_Init(uint32_t chn, DMA_InitStructure * initStruct);	//DMAͨ������
void DMA_CH_Open(uint32_t chn);
void DMA_CH_Close(uint32_t chn);

void DMA_CH_SetCount(uint32_t chn, uint32_t count);
void DMA_CH_SetSrcAddress(uint32_t chn, uint32_t address);
void DMA_CH_SetDstAddress(uint32_t chn, uint32_t address);
uint32_t DMA_CH_GetRemaining(uint32_t chn);

void DMA_CH_INTEn(uint32_t chn, uint32_t it);					//DMA�ж�ʹ��
void DMA_CH_INTDis(uint32_t chn, uint32_t it);					//DMA�жϽ�ֹ
void DMA_CH_INTClr(uint32_t chn, uint32_t it);					//DMA�жϱ�־���
uint32_t DMA_CH_INTStat(uint32_t chn, uint32_t it);				//DMA�ж�״̬��ѯ


#endif //__SWM341_DMA_H__
