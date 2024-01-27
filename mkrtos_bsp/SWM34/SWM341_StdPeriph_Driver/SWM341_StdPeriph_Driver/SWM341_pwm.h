#ifndef __SWM341_PWM_H__
#define	__SWM341_PWM_H__


typedef struct {
	uint8_t  Mode;			//PWM_EDGE_ALIGNED��PWM_CENTER_ALIGNED��PWM_ASYM_CENTER_ALIGNED

	uint16_t Clkdiv;		//1--1024

	uint16_t Period;		//����
	
	uint16_t HdutyA;		//ռ�ձȣ��ߵ�ƽʱ����PWM_ASYM_CENTER_ALIGNED ģʽ���趨ǰ�����ڵĸߵ�ƽʱ��
	uint16_t HdutyA2;		//ֻ�� PWM_ASYM_CENTER_ALIGNED ģʽ��ʹ�ã������趨������ڵĸߵ�ƽʱ��
	uint16_t DeadzoneA;		//����ʱ����ȡֵ0--1023
	uint8_t  IdleLevelA;	//ֹͣ״̬��PWMxA�����ƽ
	uint8_t  IdleLevelAN;	//ֹͣ״̬��PWMxAN�����ƽ
	uint8_t  OutputInvA;	//PWMxA���ȡ��
	uint8_t  OutputInvAN;	//PWMxAN���ȡ��
	
	uint16_t HdutyB;
	uint16_t HdutyB2;
	uint16_t DeadzoneB;
	uint8_t  IdleLevelB;
	uint8_t  IdleLevelBN;
	uint8_t  OutputInvB;
	uint8_t  OutputInvBN;
	
	uint8_t  UpOvfIE;		//�����ؼ���������ж�ʹ�ܣ�PWM_EDGE_ALIGNED ģʽ�����ϼ��������ֻ��������
	uint8_t  DownOvfIE;		//�½��ؼ���������ж�ʹ�ܣ���������ģʽ�������ϼ����������¼�������������ǰ�����ڡ��½����Ǻ������
	uint8_t  UpCmpAIE;		//�����رȽ���Aƥ���ж�ʹ��
	uint8_t  DownCmpAIE;	//�½��رȽ���Aƥ���ж�ʹ��
	uint8_t  UpCmpBIE;		//�����رȽ���Bƥ���ж�ʹ��
	uint8_t  DownCmpBIE;	//�½��رȽ���Bƥ���ж�ʹ��
} PWM_InitStructure;


#define PWM_EDGE_ALIGNED		  0
#define PWM_CENTER_ALIGNED		  1
#define PWM_ASYM_CENTER_ALIGNED   2		// �ǶԳ����Ķ���ģʽ�������غ��½��ؾ��в�ͬ�ķ�ת��Ƚ�ֵ


#define PWM_CH_A	0
#define PWM_CH_B	1

#define PWM0_MSK	(1 << 0)
#define PWM1_MSK	(1 << 1)
#define PWM2_MSK	(1 << 2)
#define PWM3_MSK	(1 << 3)
#define PWM4_MSK	(1 << 4)

#define PWM_BRK0	(1 << 0)
#define PWM_BRK1	(1 << 1)
#define PWM_BRK2	(1 << 2)

#define PWM_DIR_UP		0
#define PWM_DIR_DOWN	1

#define PWM_TRG_0		0		//�������������ָ��ֵ���ʱ PWM ��������ź�
#define PWM_TRG_1		1
#define PWM_TRG_2		2
#define PWM_TRG_3		3
#define PWM_TRG_4		4
#define PWM_TRG_5		5
#define PWM_TRG_6		6
#define PWM_TRG_7		7

#define PWM_EVT_DIS		0		//�ⲿ�¼��ź�Ϊ��ʱ PWM ����������ֹͣ��������ͣ�������������
#define PWM_EVT_0		1
#define PWM_EVT_1		2
#define PWM_EVT_2		3
#define PWM_EVT_3		4
#define PWM_EVT_4		5
#define PWM_EVT_TIMR0	6
#define PWM_EVT_TIMR1	7

/* Interrupt Type */
#define PWM_IT_OVF_UP	   (1 << 0)		//���ϼ���ʱ�����������PWM_EDGE_ALIGNED ģʽ�����ϼ��������ֻ��������
#define PWM_IT_OVF_DOWN	   (1 << 1)		//���¼���ʱ�������������������ģʽ�������ϼ����������¼�������������ǰ�����ڡ��½����Ǻ������
#define PWM_IT_CMPA_UP	   (1 << 2)		//���ϼ���ʱ������ֵ��CMPA���
#define PWM_IT_CMPB_UP	   (1 << 3)		//���ϼ���ʱ������ֵ��CMPB���
#define PWM_IT_CMPA_DOWN   (1 << 4)		//���¼���ʱ������ֵ��CMPA���
#define PWM_IT_CMPB_DOWN   (1 << 5)		//���¼���ʱ������ֵ��CMPB���

#define PWM_BRKIT_BRK0	   (1 << 0)		//PWM_BRK0�����ϳ���ɲ���ź�
#define PWM_BRKIT_BRK1	   (1 << 1)
#define PWM_BRKIT_BRK2	   (1 << 2)



void PWM_Init(PWM_TypeDef * PWMx, PWM_InitStructure * initStruct);			//PWM��ʼ��
void PWM_Start(uint32_t pwm);												//����PWM����ʼPWM���
void PWM_Stop(uint32_t pwm);												//�ر�PWM��ֹͣPWM���
void PWM_Restart(uint32_t pwm);

void PWM_ReloadEn(uint32_t pwm);
void PWM_ReloadDis(uint32_t pwm);

void PWM_BrkInPolarity(uint32_t brk, uint32_t level);
void PWM_BrkConfig(PWM_TypeDef * PWMx, uint32_t chn, uint32_t brk, uint32_t out, uint32_t outN, uint32_t outHold, uint32_t stpCount);

void PWM_OvfTrigger(PWM_TypeDef * PWMx, uint32_t en_up, uint32_t en_down, uint32_t trig_chn);
void PWM_CmpTrigger(PWM_TypeDef * PWMx, uint16_t match, uint32_t dir, uint32_t width, uint32_t trig_chn, uint32_t adc_trig_pos);

void PWM_OutMask(PWM_TypeDef * PWMx, uint32_t chn, uint32_t evt, uint32_t out, uint32_t evt_n, uint32_t out_n);

void PWM_SetPeriod(PWM_TypeDef * PWMx, uint16_t period);					//��������
uint16_t PWM_GetPeriod(PWM_TypeDef * PWMx);	   								//��ȡ����
void PWM_SetHDuty(PWM_TypeDef * PWMx, uint32_t chn, uint16_t hduty);		//���øߵ�ƽʱ��
uint16_t PWM_GetHDuty(PWM_TypeDef * PWMx, uint32_t chn);					//��ȡ�ߵ�ƽʱ��
void PWM_SetHDuty2(PWM_TypeDef * PWMx, uint32_t chn, uint16_t hduty, uint16_t hduty2);		//���øߵ�ƽʱ�������ڷǶԳ����Ķ���ģʽ
void PWM_GetHDuty2(PWM_TypeDef * PWMx, uint32_t chn, uint16_t *hduty, uint16_t *hduty2);	//��ȡ�ߵ�ƽʱ�������ڷǶԳ����Ķ���ģʽ
void PWM_SetDeadzone(PWM_TypeDef * PWMx, uint32_t chn, uint16_t deadzone);	//��������ʱ��
uint16_t PWM_GetDeadzone(PWM_TypeDef * PWMx, uint32_t chn);		 			//��ȡ����ʱ��

void PWM_IntEn(PWM_TypeDef * PWMx, uint32_t it);   							//�ж�ʹ��
void PWM_IntDis(PWM_TypeDef * PWMx, uint32_t it);  							//�жϽ���
void PWM_IntClr(PWM_TypeDef * PWMx, uint32_t it);  							//�жϱ�־���
uint32_t PWM_IntStat(PWM_TypeDef * PWMx, uint32_t it); 						//�жϱ�־��ѯ
void PWM_BrkIntEn(uint32_t brkit);   										//ɲ���ж�ʹ��
void PWM_BrkIntDis(uint32_t brkit);  										//ɲ���жϽ���
void PWM_BrkIntClr(uint32_t brkit);  										//ɲ���жϱ�־���
uint32_t PWM_BrkIntStat(uint32_t brkit); 									//ɲ���жϱ�־��ѯ


#endif //__SWM341_PWM_H__
