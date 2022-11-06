#ifndef _ISR_H__
#define _ISR_H__

#include "type.h"

#define ISR_START_INX 15
//�жϴӵ�16��ʼ��ǰ����ϵͳ���쳣
typedef void (*IsrCb)(void);
typedef struct IsrFunc{
	//�жϵĻص�����
	IsrCb isrFunc;
	//0bit:isr�������ں˻����û�
	uint8_t isrFlag;
}*PIsrFunc,IsrFunc;

//ע���жϺ���
void RegIsrFunc(IsrCb isrCb,uint32_t isrNum,uint8_t inKernel);
//ȡ��ע���жϺ���
void UnregIsrFunc(uint32_t isrNum);

#endif
