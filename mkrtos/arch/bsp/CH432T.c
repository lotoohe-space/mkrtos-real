#include "spi2.h"
#include "CH432T.h"
#include "delay.h"
#include "arch/atomic.h"
#include <termios.h>
#define CH432T_EN_PIN PBout(12)
#define CH432_BPS   9600    /* ����CH432����0ͨѶ������ */
#define CH432_BPS1  115200    /* ����CH432����1ͨѶ������ */
#define Fpclk    1843200    /* �����ڲ�ʱ��Ƶ��    */

//static Atomic_t lock={0};
//data:Ҫд�������
//����ֵ:����������
uint8_t Ch432_SPI_ReadWriteByte(uint8_t data)
{
    uint8_t r ;
    again:
//    if(atomic_test_set(&lock,1)) {
        //uint32_t t=DisCpuInter();
        r= SPI2_ReadWriteByte(data);
        //RestoreCpuInter(t);
//        atomic_set(&lock,0);
//    }else{
//        goto again;
//    }
	return r;
}	  
void WriteCH432Block( uint32_t mAddr, uint8_t mLen, uint8_t *mBuf )    /* ��ָ����ʼ��ַд�����ݿ� */
{
    while ( mLen -- ) WriteCH432Data( mAddr, *mBuf++ );
}

void WriteCH432Data( uint8_t mAddr, uint8_t mData )
{
	CH432T_EN_PIN=0;

	mAddr=((mAddr<<2)|0x02);
	Ch432_SPI_ReadWriteByte(mAddr);
	Ch432_SPI_ReadWriteByte(mData);

	CH432T_EN_PIN=1;
}
uint8_t ReadCH432Data( uint8_t mAddr )
{
	uint8_t dat; 
	CH432T_EN_PIN=0;

	mAddr<<=2; 
	mAddr&=0xfd; 
	Ch432_SPI_ReadWriteByte(mAddr);
	dat=Ch432_SPI_ReadWriteByte(0xff); 

	CH432T_EN_PIN=1;
return dat; 
}

//MUTEX* ch432t_mutex=0;

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 0
#include <stdio.h>
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{     
//	mutex_lock(ch432t_mutex,MUTEX_WAIT_ENDLESS);
	u32 lev;
	lev=enter_critical_zone();
	UART1_SendByte(ch);
	exit_critical_zone(lev);
//	mutex_unlock(ch432t_mutex);
	return ch;
}
#endif 
void Ch432_hw_init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //ʹ�ܸ��ù���ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //CS����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	CH432T_EN_PIN=1;
	SPI2_Init();
	
}
void ch432t_flow_ctrl(int32_t inx,int32_t status){
    switch(inx){
        case 0:
            if(status) {
                WriteCH432Data(CH432_MCR_PORT, BIT_MCR_RTS|BIT_MCR_DTR|BIT_MCR_OUT2);    /* �����ж������DTR,RTSΪ1 */
            }else{
                WriteCH432Data(CH432_MCR_PORT, BIT_MCR_OUT2);    /* �����ж������DTR,RTSΪ1 */
            }
            break;
        case 1:
            if(status) {
                WriteCH432Data(CH432_MCR1_PORT, BIT_MCR_RTS|BIT_MCR_DTR|BIT_MCR_OUT2);    /* �����ж������DTR,RTSΪ1 */
            }else{
                WriteCH432Data(CH432_MCR1_PORT, BIT_MCR_OUT2);    /* �����ж������DTR,RTSΪ1 */
            }
            break;
    }
}
void ch432t_set_baud(int32_t inx,int32_t baud){
    uint8_t DLM,DLL;
    uint32_t div;
    div = ( Fpclk >> 4 ) / baud;
    DLM = div >> 8;
    DLL = div & 0xff;
    switch(inx){
        case 0:
            WriteCH432Data( CH432_LCR_PORT, BIT_LCR_DLAB );    /* ����DLABΪ1 */
            WriteCH432Data( CH432_DLL_PORT, DLL );    /* ���ò����� */
            WriteCH432Data( CH432_DLM_PORT, DLM );
            break;
        case 1:
            WriteCH432Data( CH432_LCR1_PORT, BIT_LCR_DLAB );    /* ����DLABΪ1 */
            WriteCH432Data( CH432_DLL1_PORT, DLL );    /* ���ò����� */
            WriteCH432Data( CH432_DLM1_PORT, DLM );
            break;
    }
}
void ch4324_set_par(int32_t inx,int32_t csize,int32_t cstopb,int32_t par){
    uint32_t w_tmp=0;
    switch (csize) {
        case CS5:
            w_tmp&=~BIT_LCR_WORDSZ0;
            w_tmp&=~BIT_LCR_WORDSZ1;
            break;
        case CS6:
            w_tmp|=BIT_LCR_WORDSZ0;
            break;
        case CS7:
            w_tmp|=BIT_LCR_WORDSZ1;
            break;
        case CS8:
            w_tmp|=BIT_LCR_WORDSZ0|BIT_LCR_WORDSZ1;
            break;
    }
    switch(cstopb){
        case 0:
            w_tmp&=~BIT_LCR_STOPBIT;
            break;
        case 1:
            w_tmp|=BIT_LCR_STOPBIT;
            break;
    }
    switch(par){
        case 0:
            w_tmp&=~BIT_LCR_PAREN;
            break;
        case 1:
            w_tmp|=BIT_LCR_PAREN;
            w_tmp&=~BIT_LCR_PARMODE0;
            w_tmp&=~BIT_LCR_PARMODE1;
            break;
        case 2:
            w_tmp|=BIT_LCR_PAREN;
            w_tmp|=BIT_LCR_PARMODE0;
            w_tmp&=~BIT_LCR_PARMODE1;
            break;
    }
    switch(inx) {
        case 0:
            WriteCH432Data(CH432_LCR_PORT, w_tmp);    /* �ֳ�8λ��1λֹͣλ����У�� */
            break;
        case 1:
            WriteCH432Data(CH432_LCR1_PORT, w_tmp);    /* �ֳ�8λ��1λֹͣλ����У�� */
            break;
    }
}
void ch432t_reset(int inx){
    switch(inx){
        case 0:
            WriteCH432Data( CH432_IER_PORT, BIT_IER_RESET );    /* ����DLABΪ1 */
            break;
        case 1:
            WriteCH432Data( CH432_IER1_PORT, BIT_IER_RESET );    /* ����DLABΪ1 */
         break;
    }
    delay_ms(20);
}
void ch432t_init_intr(int inx){
    switch(inx){
        case 0:
            WriteCH432Data( CH432_FCR_PORT,BIT_FCR_FIFOEN );    /* ����FIFOģʽ��������Ϊ8 */
            WriteCH432Data( CH432_IER_PORT,BIT_IER_IERECV );    /* ʹ���ж� */
            WriteCH432Data( CH432_MCR_PORT, BIT_MCR_OUT2  );    /* �����ж������DTR,RTSΪ1 */
            delay_ms(20);
            ReadCH432Data(CH432_IIR_PORT );	/* ��������·���Ϊ01H */
            break;
        case 1:
            WriteCH432Data( CH432_FCR1_PORT,BIT_FCR_FIFOEN );    /* ����FIFOģʽ��������Ϊ8 */
            WriteCH432Data( CH432_IER1_PORT,BIT_IER_IERECV );    /* ʹ���ж� */
            WriteCH432Data( CH432_MCR1_PORT, BIT_MCR_OUT2  );    /* �����ж������DTR,RTSΪ1 */
            delay_ms(20);
            ReadCH432Data( CH432_IIR1_PORT );	/* ��������·���Ϊ01H */
            break;
    }

}
//SPIӲ�����ʼ��
int Ch432_SPI_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	uint16_t div;
	uint8_t DLL, DLM;
	delay_ms(20);
	Ch432_hw_init();
	
	WriteCH432Data( CH432_IER_PORT, BIT_IER_RESET );    /* ����DLABΪ1 */
	delay_ms(20);

/**************************************************************************
          ����CH432����0�ļĴ���
**************************************************************************/
    div = ( Fpclk >> 4 ) / CH432_BPS;
    DLM = div >> 8;
    DLL = div & 0xff;

    WriteCH432Data( CH432_LCR_PORT, BIT_LCR_DLAB );    /* ����DLABΪ1 */
    WriteCH432Data( CH432_DLL_PORT, DLL );    /* ���ò����� */
    WriteCH432Data( CH432_DLM_PORT, DLM );
    WriteCH432Data( CH432_FCR_PORT, BIT_FCR_FIFOEN );    /* ����FIFOģʽ��������Ϊ8 */
    WriteCH432Data( CH432_LCR_PORT, BIT_LCR_WORDSZ1| BIT_LCR_WORDSZ0 );    /* �ֳ�8λ��1λֹͣλ����У�� */
    WriteCH432Data( CH432_IER_PORT,BIT_IER_IERECV );    /* ʹ���ж� */
    WriteCH432Data( CH432_MCR_PORT, BIT_MCR_OUT2 );    /* �����ж����,DTR,RTSΪ1 */

/**************************************************************************
          ����CH432����1�ļĴ���
**************************************************************************/
    WriteCH432Data( CH432_IER1_PORT, BIT_IER_RESET );    /* ����DLABΪ1 */
    delay_ms(20);
    div = ( Fpclk >> 4 ) / CH432_BPS1;
    DLM = div >> 8;
    DLL = div & 0xff;
    WriteCH432Data( CH432_LCR1_PORT, BIT_LCR_DLAB );    /* ����DLABΪ1 */
    WriteCH432Data( CH432_DLL1_PORT, DLL );    /* ���ò����� */
    WriteCH432Data( CH432_DLM1_PORT, DLM );
    WriteCH432Data( CH432_FCR1_PORT,BIT_FCR_FIFOEN );    /* ����FIFOģʽ��������Ϊ8 */
    WriteCH432Data( CH432_LCR1_PORT,BIT_LCR_WORDSZ1| BIT_LCR_WORDSZ0 );    /* �ֳ�8λ��1λֹͣλ */
    WriteCH432Data( CH432_IER1_PORT,BIT_IER_IERECV );    /* ʹ���ж� */
    WriteCH432Data( CH432_MCR1_PORT, BIT_MCR_OUT2  );    /* �����ж������DTR,RTSΪ1 */
    delay_ms(20);

	ReadCH432Data(CH432_IIR_PORT );	/* ��������·���Ϊ01H */ 
	ReadCH432Data( CH432_IIR1_PORT );	/* ��������·���Ϊ01H */ 
	
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//
	GPIO_SetBits(GPIOC,GPIO_Pin_6); 
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource6);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);  
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;            
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;    
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;                 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                            
	NVIC_Init(&NVIC_InitStructure);   
	return 1;
}
void UART0_SendByte( uint8_t dat )    /* CH432����0����һ�ֽ��ӳ��� */
{
    while( ( ReadCH432Data( CH432_LSR_PORT ) & BIT_LSR_THRE ) == 0 );     /* �ȴ����ݷ������ */
    WriteCH432Data( CH432_THR_PORT, dat );
	
}
uint8_t UART0_RcvByte()    /* CH432����0����һ�ֽ��ӳ��� */
{
    uint8_t Rcvdat;
    if( !( ReadCH432Data( CH432_LSR_PORT ) & ( BIT_LSR_BREAKINT | BIT_LSR_FRAMEERR | BIT_LSR_PARERR | BIT_LSR_OVERR ) ) )    /*  b1-b4�޴��� */
    {
        while( ( ReadCH432Data( CH432_LSR_PORT ) & BIT_LSR_DATARDY ) == 0 );    /* �ȴ�����׼���� */
        Rcvdat = ReadCH432Data( CH432_RBR_PORT );    /* �ӽ��ջ���Ĵ����������� */
        return( Rcvdat );
    }
    else return ReadCH432Data( CH432_RBR_PORT );    /* �д������ */
}

void UART1_SendByte( uint8_t dat )    /* CH432����1����һ�ֽ��ӳ��� */
{
    while( ( ReadCH432Data( CH432_LSR1_PORT ) & BIT_LSR_THRE ) == 0 );     /* �ȴ����ݷ������ */
    WriteCH432Data( CH432_THR1_PORT, dat );
}
uint8_t UART1_RcvByte()    /* CH432����1����һ�ֽ��ӳ��� */
{
    uint8_t Rcvdat;
    if( !( ReadCH432Data( CH432_LSR1_PORT ) & (BIT_LSR_BREAKINT | BIT_LSR_FRAMEERR | BIT_LSR_PARERR | BIT_LSR_OVERR ) ) )    /* b1-b4�޴��� */
    {
        while( ( ReadCH432Data( CH432_LSR1_PORT ) & BIT_LSR_DATARDY ) == 0 );    /* �ȴ�����׼���� */
        Rcvdat = ReadCH432Data( CH432_RBR1_PORT );    /* �ӽ��ջ���Ĵ����������� */
        return( Rcvdat );
    }
    else return ReadCH432Data( CH432_RBR1_PORT );    /* b1-b4�д������ */
}
void  CH432Seril0Send( u8 *Data, u8 Num )    /* ����FIFO,CH432����0���Ͷ��ֽ��ӳ��� */
{
	if(Num==0){return ;}
    do
    {
        while( ( ReadCH432Data( CH432_LSR_PORT ) & BIT_LSR_THRE ) == 0 );    /* �ȴ����ݷ������ */
        WriteCH432Data( CH432_THR_PORT, *Data++ );
    }
    while( --Num );
}

void  CH432Seril1Send( u8 *Data, u8 Num )    /* ����FIFO,CH432����1���Ͷ��ֽ��ӳ��� */
{
	if(Num==0){return ;}
    do
    {
        while( ( ReadCH432Data( CH432_LSR1_PORT ) & BIT_LSR_THRE ) == 0 );    /* �ȴ����ݷ������ */
        WriteCH432Data( CH432_THR1_PORT, *Data++ );
    }
    while( --Num );
}

u8  CH432Seril0Rcv( u8 *buf )    /* ����FIFO,CH432����0���ն��ֽ��ӳ��� */
{
    u8 RcvNum = 0;
    if( !( ReadCH432Data( CH432_LSR_PORT ) & ( BIT_LSR_BREAKINT | BIT_LSR_FRAMEERR | BIT_LSR_PARERR | BIT_LSR_OVERR ) ) )    /* b1-b4�޴��� */
    {
        while( ( ReadCH432Data( CH432_LSR_PORT ) & BIT_LSR_DATARDY ) == 0 );    /* �ȴ�����׼���� */
        do
        {
            *buf++ = ReadCH432Data( CH432_RBR_PORT );    /* �ӽ��ջ���Ĵ����������� */
            RcvNum++;
        }
        while( ( ReadCH432Data( CH432_LSR_PORT ) & BIT_LSR_DATARDY ) == 0x01 );
    }
    else ReadCH432Data( CH432_RBR_PORT );
    return( RcvNum );
}

u8 CH432Seril1Rcv( u8 *buf )    /* ����FIFO,CH432����1���ն��ֽ��ӳ���*/
{
    u8 RcvNum = 0;
    if( !( ReadCH432Data( CH432_LSR1_PORT ) & ( BIT_LSR_BREAKINT | BIT_LSR_FRAMEERR | BIT_LSR_PARERR | BIT_LSR_OVERR ) ) )    /* b1-b4�޴��� */
    {
         while( ( ReadCH432Data( CH432_LSR1_PORT ) & BIT_LSR_DATARDY ) == 0 );    /* �ȴ�����׼���� */
         do
         {
              *buf++ = ReadCH432Data( CH432_RBR1_PORT );    /* �ӽ��ջ���Ĵ����������� */
              RcvNum++;
         }
         while( ( ReadCH432Data( CH432_LSR1_PORT ) & BIT_LSR_DATARDY ) == 0x01 );
    }
    else ReadCH432Data( CH432_RBR1_PORT );
    return( RcvNum );
}

void  CH432UART0Send( u8 *Data, u8 Num )    /* ����FIFO,һ�����16�ֽڣ�CH432����0���Ͷ��ֽ��ӳ��� */
{
    while( 1 )
    {
        while( ( ReadCH432Data( CH432_LSR_PORT ) & BIT_LSR_TEMT ) == 0 );    /* �ȴ����ݷ�����ϣ�THR,TSRȫ�� */
        if( Num<= 16 )
        {
            WriteCH432Block( CH432_THR_PORT, Num, Data );
            break;
        }
        else
        {
            WriteCH432Block( CH432_THR_PORT, 16, Data );
            Num -= 16;
            Data += 16;
        }
    }
}

void CH432UART1Send( u8 *Data, u8 Num )    /* ����FIFO,һ�����16�ֽڣ�CH432����1���Ͷ��ֽ��ӳ��� */
{
    while(1)
    {
        while( ( ReadCH432Data( CH432_LSR1_PORT ) & BIT_LSR_TEMT ) == 0 );    /* �ȴ����ݷ�����ϣ�THR,TSRȫ�� */
        if( Num<= 16 )
        {
            WriteCH432Block( CH432_THR1_PORT, Num, Data );
            break;
        }
        else
        {
             WriteCH432Block( CH432_THR1_PORT, 16, Data );
             Num -= 16;
             Data += 16;
        }
    }
}

#define CH432_UART0_RECV_BUF_LEN 32
#define CH432_UART1_RECV_BUF_LEN 32

CH432T_recv_data_call_back CH432T_recv_0_data_fun=0x00;
CH432T_recv_data_call_back CH432T_recv_1_data_fun=0x00;

u8 ch432_uart0_recv_buf[CH432_UART0_RECV_BUF_LEN]={0};
//u16 ch432_uart0_recv_len=0;

u8 ch432_uart1_recv_buf[CH432_UART1_RECV_BUF_LEN]={0};
u16 ch432_uart1_recv_len=0;

void CH432Interrupt(void)/* �жϷ�ʽ���� */
{
	uint8_t InterruptStatus;
	uint8_t RcvNum = 0, s;
	uint8_t ModemStatus;
	InterruptStatus = ReadCH432Data( CH432_IIR_PORT ) & ( ~ CH432_IIR_FIFOS_ENABLED );
	if(( InterruptStatus & 0x01 )!=INT_NOINT)    /* û���ж�ת������1 */
	{ 
			switch( InterruptStatus )
			{
					case INT_MODEM_CHANGE:
							ModemStatus = ReadCH432Data( CH432_MSR_PORT );
							WriteCH432Data( CH432_THR_PORT, ModemStatus );
							//UART0_SendStr( SEND_STRING1 );
							if ( ModemStatus == 0x30 )    /* Modem�źŷ������ݱ仯 */
							{
							//		UART0_SendStr( SEND_STRING1 );
							}
							else if ( ModemStatus == 0x11 )    /* Modem �źŽ������ݱ仯 */
							{
									WriteCH432Data( CH432_THR_PORT, 0xAA );
							}
							else if ( ModemStatus == 0x22 )    /* Modem �źŽ������ݱ仯 */
							{
									s = ReadCH432Data( CH432_RBR_PORT );
									WriteCH432Data( CH432_THR_PORT, s );
							}
							break;
					case INT_NOINT:    /* û���ж� */
									break;
					case INT_THR_EMPTY:    /* ���ͱ��ּĴ������ж� */
									//UART0_SendStr( SEND_STRING );
									break;
					case INT_RCV_SUCCESS:    /* ���ڽ��տ��������ж� */
									RcvNum = CH432Seril0Rcv( ch432_uart0_recv_buf);			
									if(CH432T_recv_0_data_fun!=0x00){
										CH432T_recv_0_data_fun(0,ch432_uart0_recv_buf,RcvNum);
									}
									break;
					case INT_RCV_LINES:    /* ������·״̬�ж� */
									ReadCH432Data( CH432_LSR_PORT);
									break;
					case INT_RCV_OVERTIME:    /* �������ݳ�ʱ�ж� */
									RcvNum = CH432Seril0Rcv( ch432_uart0_recv_buf );					
									if(CH432T_recv_0_data_fun != 0x00){
										CH432T_recv_0_data_fun(0,ch432_uart0_recv_buf,RcvNum);
									}
									break;
					default:    /* �����ܷ������ж� */
									break;
			}
	}

	InterruptStatus = ReadCH432Data( CH432_IIR1_PORT ) & ( ~ CH432_IIR_FIFOS_ENABLED );    /* ������1���ж�״̬ */
	if( ( InterruptStatus & 0x01)!=INT_NOINT ) {
		switch( InterruptStatus )
		{
				case INT_NOINT:    /* û���ж� */
						break;
				case INT_THR_EMPTY:    /* ���ͱ��ּĴ������ж� */
						//ReadCH432Data( CH432_IIR_PORT );//����ж�
						break;
				case INT_RCV_SUCCESS:    /* ���ڽ��տ��������ж� */
						RcvNum = CH432Seril1Rcv( ch432_uart1_recv_buf );
						if(CH432T_recv_1_data_fun!=0x00){
							CH432T_recv_1_data_fun(1,ch432_uart1_recv_buf,RcvNum);
						}
						break;
				case INT_RCV_LINES:    /* ������·״̬�ж� */
						ReadCH432Data( CH432_LSR1_PORT);
						break;
				case INT_RCV_OVERTIME:    /* �������ݳ�ʱ�ж� */
						RcvNum = CH432Seril1Rcv( ch432_uart1_recv_buf );
						if(CH432T_recv_1_data_fun!=0x00){
							CH432T_recv_1_data_fun(1,ch432_uart1_recv_buf,RcvNum);
						}
						break;
				default:    /* �����ܷ������ж� */
						break;
			}
	}
}
void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
	
		while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)==0){
			//�յ�������
			CH432Interrupt();
		}
		//����жϱ�־λ
		EXTI_ClearITPendingBit(EXTI_Line6); 
	}
}
