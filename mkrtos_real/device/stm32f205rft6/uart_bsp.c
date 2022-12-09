
#include <stm32f2xx.h>

#define CR3_CLEAR_MASK            ((uint16_t)(USART_CR3_RTSE | USART_CR3_CTSE))

void uart_set_word_len(USART_TypeDef* USARTx, uint16_t len)
{
	uint32_t tmpreg;

	tmpreg = USARTx->CR1;

	/* Clear M, PCE, PS, TE and RE bits */
	tmpreg &= (uint32_t)~((uint32_t)USART_CR1_M);

	/* Configure the USART Word Length, Parity and mode: 
		Set the M bits according to USART_WordLength value 
		Set PCE and PS bits according to USART_Parity value
		Set TE and RE bits according to USART_Mode value */
	tmpreg |= (uint32_t)len;

	/* Write to USART CR1 */
	USARTx->CR1 = (uint16_t)tmpreg;
}
void uart_set_baud(USART_TypeDef* USARTx, uint32_t baud)
{
	uint32_t tmpreg = 0x00, apbclock = 0x00;
	RCC_ClocksTypeDef RCC_ClocksStatus;

	tmpreg = USART1->BRR;
	uint32_t integerdivider = 0x00;
	uint32_t fractionaldivider = 0x00;

	RCC_GetClocksFreq(&RCC_ClocksStatus);

	if ((USARTx == USART1) || (USARTx == USART6))
	{
		apbclock = RCC_ClocksStatus.PCLK2_Frequency;
	}
	else
	{
		apbclock = RCC_ClocksStatus.PCLK1_Frequency;
	}

	/* Determine the integer part */
	if ((USARTx->CR1 & USART_CR1_OVER8) != 0)
	{
		/* Integer part computing in case Oversampling mode is 8 Samples */
		integerdivider = ((25 * apbclock) / (2 * (baud)));    
	}
	else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
	{
		/* Integer part computing in case Oversampling mode is 16 Samples */
		integerdivider = ((25 * apbclock) / (4 * (baud)));    
	}
	tmpreg = (integerdivider / 100) << 4;

	/* Determine the fractional part */
	fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

	/* Implement the fractional part in the register */
	if ((USARTx->CR1 & USART_CR1_OVER8) != 0)
	{
	tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
	}
	else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
	{
	tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
	}

	USARTx->BRR &= ((~0UL)<<16);
	USARTx->BRR |= (uint16_t)tmpreg;
}
void uart_set_stop_bit(USART_TypeDef* USARTx, uint16_t stop_bits)
{
	uint32_t tmpreg;

	tmpreg = USARTx->CR2;

	/* Clear STOP[13:12] bits */
	tmpreg &= (uint32_t)~((uint32_t)USART_CR2_STOP);

	/* Configure the USART Stop Bits, Clock, CPOL, CPHA and LastBit :
	Set STOP[13:12] bits according to USART_StopBits value */
	tmpreg |= (uint32_t)stop_bits;

	/* Write to USART CR2 */
	USARTx->CR2 = (uint16_t)tmpreg;
}
void uart_set_parity(USART_TypeDef* USARTx, uint16_t parity)
{
	uint32_t tmpreg;

	tmpreg = USARTx->CR1;

	/* Clear M, PCE, PS, TE and RE bits */
	tmpreg &= (uint32_t)~((uint32_t)USART_CR1_PCE | USART_CR1_PS);

	/* Configure the USART Word Length, Parity and mode: 
	Set the M bits according to USART_WordLength value 
	Set PCE and PS bits according to USART_Parity value
	Set TE and RE bits according to USART_Mode value */
	tmpreg |= (uint32_t)parity;

	/* Write to USART CR1 */
	USARTx->CR1 = (uint16_t)tmpreg;
}
void uart_hardware_flow_rts(USART_TypeDef* USARTx, uint16_t flow)
{
	uint32_t tmpreg;

	tmpreg = USARTx->CR3;

	/* Clear CTSE and RTSE bits */
	tmpreg &= (uint32_t)~((uint32_t)CR3_CLEAR_MASK);

	/* Configure the USART HFC : 
	Set CTSE and RTSE bits according to USART_HardwareFlowControl value */
	tmpreg |= flow;

	/* Write to USART CR3 */
	USARTx->CR3 = (uint16_t)tmpreg;
}