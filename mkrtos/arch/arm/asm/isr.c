
#include "arch/isr.h"
#include "type.h"
#include "arch/arch.h"
#include "mkrtos/task.h"
#include "mkrtos/msg_knl.h"
//ֻ�ܹ�����Ȩģʽ����
//��ȡ�жϺ�
uint32_t GetISRNum(void){
    uint32_t num;
    __asm__ __volatile__(
        "mrs %0,IPSR"
        :"=r"(num)
        :
        :
    );
	return num;
}

//�жϺ����б�
IsrFunc isrFuncList[59]={NULL};


/**
 * @berief ���е��ⲿ�ж϶�������,
 * @param �������ջָ��
 */
void ExternInter(void* sp){
	uint32_t isrNum;
	isrNum = GetISRNum();

		//�ں���ֱ�ӵ����ж�
    if(isrFuncList[isrNum-ISR_START_INX].isrFunc!=NULL){
        isrFuncList[isrNum-ISR_START_INX].isrFunc();
    }else{
        printk("unreg isr %d.\r\n",isrNum);
    }


//    extern void do_signal_isr(void* sp);
//    do_signal_isr(sp);

   // msg_check();
}

//ע���жϺ���
void RegIsrFunc(IsrCb isrCb,uint32_t isrNum,uint8_t inKernel){
	uint32_t t;
	t=DisCpuInter();
	isrFuncList[isrNum].isrFunc=isrCb;
	isrFuncList[isrNum].isrFlag|=inKernel;
	RestoreCpuInter(t);
}
//ȡ��ע���жϺ���
void UnregIsrFunc(uint32_t isrNum){
	uint32_t t;
	t=DisCpuInter();
	isrFuncList[isrNum].isrFunc=NULL;
	RestoreCpuInter(t);
}
