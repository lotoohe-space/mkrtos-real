#ifndef _ISR_H__
#define _ISR_H__

#include "type.h"

#define ISR_START_INX 15
//�жϴӵ�16��ʼ��ǰ����ϵͳ���쳣
typedef void (*isr_cb)(void);
struct isr_func{
	//�жϵĻص�����
	isr_cb isr_func;
	//0bit:isr�������ں˻����û�
	uint8_t isr_flag;
};

void extern_intr(void* sp);


//ע���жϺ���
void reg_isr_func(isr_cb isrCb,uint32_t isrNum,uint8_t inKernel);
//ȡ��ע���жϺ���
void unreg_isr_func(uint32_t isrNum);

#endif
