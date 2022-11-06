#ifndef _CH432T_H__
#define _CH432T_H__
#include "sys.h"

#define REG_RBR_ADDR        0x00      /* ����0���ջ���Ĵ�����ַ */
#define REG_THR_ADDR        0x00      /* ����0���ͱ��ּĴ�����ַ */
#define REG_IER_ADDR        0x01      /* ����0�ж�ʹ�ܼĴ�����ַ */
#define REG_IIR_ADDR        0x02      /* ����0�ж�ʶ��Ĵ�����ַ */
#define REG_FCR_ADDR        0x02      /* ����0FIFO���ƼĴ�����ַ */
#define REG_LCR_ADDR        0x03      /* ����0��·���ƼĴ�����ַ */
#define REG_MCR_ADDR        0x04      /* ����0MODEM���ƼĴ�����ַ */
#define REG_LSR_ADDR        0x05      /* ����0��·״̬�Ĵ�����ַ */
#define REG_MSR_ADDR        0x06      /* ����0MODEM״̬�Ĵ�����ַ */
#define REG_SCR_ADDR        0x07      /* ����0�û��ɶ���Ĵ�����ַ */
#define REG_DLL_ADDR        0x00      /* �����ʳ�����������8λ�ֽڵ�ַ */
#define REG_DLM_ADDR        0x01      /* �����ʳ�����������8λ�ֽڵ�ַ */

/* CH432����1�Ĵ�����ַ */

#define REG_RBR1_ADDR       0x08      /* ����1���ջ���Ĵ�����ַ */
#define REG_THR1_ADDR       0x08      /* ����1���ͱ��ּĴ�����ַ */
#define REG_IER1_ADDR       0x09      /* ����1�ж�ʹ�ܼĴ�����ַ */
#define REG_IIR1_ADDR       0x0A      /* ����1�ж�ʶ��Ĵ�����ַ */
#define REG_FCR1_ADDR       0x0A      /* ����1FIFO���ƼĴ�����ַ */
#define REG_LCR1_ADDR       0x0B      /* ����1��·���ƼĴ�����ַ */
#define REG_MCR1_ADDR       0x0C      /* ����1MODEM���ƼĴ�����ַ */
#define REG_LSR1_ADDR       0x0D      /* ����1��·״̬�Ĵ�����ַ */
#define REG_MSR1_ADDR       0x0E      /* ����1MODEM״̬�Ĵ�����ַ */
#define REG_SCR1_ADDR       0x0F      /* ����1�û��ɶ���Ĵ�����ַ */
#define REG_DLL1_ADDR       0x08      /* �����ʳ�����������8λ�ֽڵ�ַ */
#define REG_DLM1_ADDR       0x09      /* �����ʳ�����������8λ�ֽڵ�ַ */

/* IER�Ĵ�����λ */

#define BIT_IER_RESET       0x80      /* ��λ��1����λ�ô��� */
#define BIT_IER_LOWPOWER    0x40      /* ��λΪ1��رոô��ڵ��ڲ���׼ʱ�� */
#define BIT_IER_SLP         0x20      /* ����0��SLP,Ϊ1��ر�ʱ������ */
#define BIT_IER1_CK2X       0x20      /* ����1��CK2X,Ϊ1��ǿ�ƽ��ⲿʱ���ź�2��Ƶ����Ϊ�ڲ���׼ʱ�� */
#define BIT_IER_IEMODEM     0x08      /* ��λΪ1����MODEM����״̬�仯�ж� */
#define BIT_IER_IELINES     0x04      /* ��λΪ1���������·״̬�ж� */
#define BIT_IER_IETHRE      0x02      /* ��λΪ1�����ͱ��ּĴ������ж� */
#define BIT_IER_IERECV      0x01      /* ��λΪ1������յ������ж� */

/* IIR�Ĵ�����λ */

#define BIT_IIR_FIFOENS1    0x80
#define BIT_IIR_FIFOENS0    0x40      /* ��2λΪ1��ʾ����FIFO */

/* �ж����ͣ�0001û���жϣ�0110������·״̬�жϣ�0100�������ݿ����жϣ�
1100�������ݳ�ʱ�жϣ�0010THR�Ĵ������жϣ�0000MODEM����仯�ж� */
#define BIT_IIR_IID3        0x08
#define BIT_IIR_IID2        0x04
#define BIT_IIR_IID1        0x02
#define BIT_IIR_NOINT       0x01

/* FCR�Ĵ�����λ */

/* �����㣺 00��Ӧ1���ֽڣ�01��Ӧ4���ֽڣ�10��Ӧ8���ֽڣ�11��Ӧ14���ֽ� */
#define BIT_FCR_RECVTG1     0x80      /* ����FIFO���жϺ��Զ�Ӳ�������ƵĴ����� */
#define BIT_FCR_RECVTG0     0x40      /* ����FIFO���жϺ��Զ�Ӳ�������ƵĴ����� */

#define BIT_FCR_TFIFORST    0x04      /* ��λ��1����շ���FIFO�е����� */
#define BIT_FCR_RFIFORST    0x02      /* ��λ��1����ս���FIFO�е����� */
#define BIT_FCR_FIFOEN      0x01      /* ��λ��1������FIFO,Ϊ0�����FIFO */

/* LCR�Ĵ�����λ */

#define BIT_LCR_DLAB        0x80      /* Ϊ1���ܴ�ȡDLL��DLM��Ϊ0���ܴ�ȡRBR/THR/IER */
#define BIT_LCR_BREAKEN     0x40      /* Ϊ1��ǿ�Ʋ���BREAK��·���*/

/* ����У���ʽ����PARENΪ1ʱ��00��У�飬01żУ�飬10��־λ��MARK����1)��11�հ�λ��SPACE����0) */
#define BIT_LCR_PARMODE1    0x20      /* ������żУ��λ��ʽ */
#define BIT_LCR_PARMODE0    0x10      /* ������żУ��λ��ʽ */

#define BIT_LCR_PAREN       0x08      /* Ϊ1��������ʱ�����ͽ���У����żУ��λ */
#define BIT_LCR_STOPBIT     0x04      /* Ϊ1������ֹͣλ,Ϊ0һ��ֹͣλ */

/* �����ֳ��ȣ�00��5������λ��01��6������λ��10��7������λ��11��8������λ */
#define BIT_LCR_WORDSZ1     0x02      /* �����ֳ����� */
#define BIT_LCR_WORDSZ0     0x01

/* MCR�Ĵ�����λ */

#define BIT_MCR_AFE         0x20      /* Ϊ1����CTS��RTSӲ���Զ������� */
#define BIT_MCR_LOOP        0x10      /* Ϊ1ʹ���ڲ���·�Ĳ���ģʽ */
#define BIT_MCR_OUT2        0x08      /* Ϊ1����ô��ڵ��ж�������� */
#define BIT_MCR_OUT1        0x04      /* Ϊ�û������MODEM����λ */
#define BIT_MCR_RTS         0x02      /* ��λΪ1��RTS���������Ч */
#define BIT_MCR_DTR         0x01      /* ��λΪ1��DTR���������Ч */

/* LSR�Ĵ�����λ */

#define BIT_LSR_RFIFOERR    0x80      /* Ϊ1��ʾ�ڽ���FIFO�д�������һ������ */
#define BIT_LSR_TEMT        0x40      /* Ϊ1��ʾTHR��TSRȫ�� */
#define BIT_LSR_THRE        0x20      /* Ϊ1��ʾTHR��*/
#define BIT_LSR_BREAKINT    0x10      /* ��λΪ1��ʾ��⵽BREAK��·��� */
#define BIT_LSR_FRAMEERR    0x08      /* ��λΪ1��ʾ��ȡ����֡���� */
#define BIT_LSR_PARERR      0x04      /* ��λΪ1��ʾ��żУ����� */
#define BIT_LSR_OVERR       0x02      /* Ϊ1��ʾ����FIFO��������� */
#define BIT_LSR_DATARDY     0x01      /* ��λΪ1��ʾ����FIFO���н��յ������� */

/* MSR�Ĵ�����λ */

#define BIT_MSR_DCD         0x80      /* ��λΪ1��ʾDCD������Ч */
#define BIT_MSR_RI          0x40      /* ��λΪ1��ʾRI������Ч */
#define BIT_MSR_DSR         0x20      /* ��λΪ1��ʾDSR������Ч */
#define BIT_MSR_CTS         0x10      /* ��λΪ1��ʾCTS������Ч */
#define BIT_MSR_DDCD        0x08      /* ��λΪ1��ʾDCD��������״̬�����仯�� */
#define BIT_MSR_TERI        0x04      /* ��λΪ1��ʾRI��������״̬�����仯�� */
#define BIT_MSR_DDSR        0x02      /* ��λΪ1��ʾDSR��������״̬�����仯�� */
#define BIT_MSR_DCTS        0x01      /* ��λΪ1��ʾCTS��������״̬�����仯�� */

/* �ж�״̬�� */

#define INT_NOINT           0x01      /* û���ж� */
#define INT_THR_EMPTY       0x02      /* THR���ж� */
#define INT_RCV_OVERTIME    0x0C      /* ���ճ�ʱ�ж� */
#define INT_RCV_SUCCESS     0x04      /* �������ݿ����ж� */
#define INT_RCV_LINES       0x06      /* ������·״̬�ж� */
#define INT_MODEM_CHANGE    0x00      /* MODEM����仯�ж� */
/**********************************************************************
        ����CH432����0�ļĴ�����ַ
***********************************************************************/

#define CH432_RBR_PORT     REG_RBR_ADDR     /* �ٶ�CH432���ջ���Ĵ���0��I/O��ַ */
#define CH432_THR_PORT     REG_THR_ADDR     /* �ٶ�CH432���ͱ��ּĴ���0��I/O��ַ */
#define CH432_IER_PORT     REG_IER_ADDR     /* �ٶ�CH432�ж�ʹ�ܼĴ���0��I/O��ַ */
#define CH432_IIR_PORT     REG_IIR_ADDR     /* �ٶ�CH432�ж�ʶ��Ĵ���0��I/O��ַ */
#define CH432_FCR_PORT     REG_FCR_ADDR     /* �ٶ�CH432FIFO���ƼĴ���0��I/O��ַ */
#define CH432_LCR_PORT     REG_LCR_ADDR     /* �ٶ�CH432��·���ƼĴ���0��I/O��ַ */
#define CH432_MCR_PORT     REG_MCR_ADDR     /* �ٶ�CH432MODEM���ƼĴ���0��I/O��ַ */
#define CH432_LSR_PORT     REG_LSR_ADDR     /* �ٶ�CH432��·״̬�Ĵ���0��I/O��ַ */
#define CH432_MSR_PORT     REG_MSR_ADDR     /* �ٶ�CH432MODEM״̬�Ĵ���0��I/O��ַ */
#define CH432_SCR_PORT     REG_SCR_ADDR     /* �ٶ�CH432�û��ɶ���Ĵ���0��I/O��ַ */
#define CH432_DLL_PORT     REG_DLL_ADDR
#define CH432_DLM_PORT     REG_DLM_ADDR

/**************************************************************************
        ����CH432����1�ļĴ�����ַ
**************************************************************************/

#define CH432_RBR1_PORT    REG_RBR1_ADDR    /* �ٶ�CH432���ջ���Ĵ���1��I/O��ַ */
#define CH432_THR1_PORT    REG_THR1_ADDR    /* �ٶ�CH432���ͱ��ּĴ���1��I/O��ַ */
#define CH432_IER1_PORT    REG_IER1_ADDR    /* �ٶ�CH432�ж�ʹ�ܼĴ���1��I/O��ַ */
#define CH432_IIR1_PORT    REG_IIR1_ADDR    /* �ٶ�CH432�ж�ʶ��Ĵ���1��I/O��ַ */
#define CH432_FCR1_PORT    REG_FCR1_ADDR    /* �ٶ�CH432FIFO���ƼĴ���1��I/O��ַ */
#define CH432_LCR1_PORT    REG_LCR1_ADDR    /* �ٶ�CH432��·���ƼĴ���1��I/O��ַ */
#define CH432_MCR1_PORT    REG_MCR1_ADDR    /* �ٶ�CH432MODEM���ƼĴ���1��I/O��ַ */
#define CH432_LSR1_PORT    REG_LSR1_ADDR    /* �ٶ�CH432��·״̬�Ĵ���1��I/O��ַ */
#define CH432_MSR1_PORT    REG_MSR1_ADDR    /* �ٶ�CH432MODEM״̬�Ĵ���1��I/O��ַ */
#define CH432_SCR1_PORT    REG_SCR1_ADDR    /* �ٶ�CH432�û��ɶ���Ĵ���1��I/O��ַ */
#define CH432_DLL1_PORT    REG_DLL1_ADDR
#define CH432_DLM1_PORT    REG_DLM1_ADDR



#define CH432_IIR_FIFOS_ENABLED 0xC0  /* ����FIFO */


typedef void (*CH432T_recv_data_call_back)(u8 port,u8 * data,u16 len);
extern CH432T_recv_data_call_back CH432T_recv_0_data_fun;
extern CH432T_recv_data_call_back CH432T_recv_1_data_fun;

uint8_t Ch432_SPI_ReadWriteByte(uint8_t data);
void Ch432_SPI_SpeedLow(void);
void Ch432_SPI_SpeedHigh(void);
void WriteCH432Data( uint8_t mAddr, uint8_t mData );
uint8_t ReadCH432Data( uint8_t mAddr );
int Ch432_SPI_Init(void);
void UART0_SendByte( uint8_t dat );
void ch432t_reset(int inx);
void ch432t_init_intr(int inx);
void UART1_SendByte( uint8_t dat ) ;
uint8_t UART1_RcvByte(void) ;

void  CH432Seril0Send( u8 *Data, u8 Num );    /* ����FIFO,CH432����0���Ͷ��ֽ��ӳ��� */
void  CH432Seril1Send( u8 *Data, u8 Num );    /* ����FIFO,CH432����1���Ͷ��ֽ��ӳ��� */

void ch432t_set_baud(int32_t inx,int32_t baud);
void ch4324_set_par(int32_t inx,int32_t csize,int32_t cstopb,int32_t par);
void ch432t_flow_ctrl(int32_t inx,int32_t status);

#endif
