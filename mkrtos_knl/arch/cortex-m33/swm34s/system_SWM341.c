/****************************************************************************************************************************************** 
* ???????:	system_SWM341.c
* ???????:	SWM341??????????????
* ???????:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* ???????:
* ?��????: V1.1.0		2017??10??25??
* ???????: 
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
#include <stdint.h>
#include "SWM341.h"


/******************************************************************************************************************************************
 * ??????څ
 *****************************************************************************************************************************************/
#define SYS_CLK_20MHz		0	 	//0 ??????20MHz  RC????
#define SYS_CLK_2M5Hz		1		//1 ??????2.5MHz RC????
#define SYS_CLK_40MHz		2		//2 ??????40MHz  RC????
#define SYS_CLK_5MHz		3		//3 ?????? 5MHz  RC????
#define SYS_CLK_XTAL		4		//4 ????????????4-32MHz??
#define SYS_CLK_XTAL_DIV8	5		//5 ????????????4-32MHz?? 8???
#define SYS_CLK_PLL			6		//6 ???????
#define SYS_CLK_PLL_DIV8	7		//7 ??????? 8???
#define SYS_CLK_32KHz		8		//8 ??????32KHz RC  ????
#define SYS_CLK_XTAL_32K	9		//9 ?????32KHz ????????

#define SYS_CLK   SYS_CLK_PLL


#define __HSI		(20000000UL)		//??????????
#define __LSI		(   32000UL)		//??????????
#define __HSE		(12000000UL)		//?????????
#define __LSE		(   32768UL)		//?????????


/********************************** PLL ?څ **********************************************
 * VCO?????? = PLL??????? / INDIV * 4 * FBDIV
 * PLL?????? = PLL??????? / INDIV * 4 * FBDIV / OUTDIV = VCO?????? / OUTDIV
 * ???VCO??????????? [600MHz, 1400MHz] ???
 *****************************************************************************************/ 
#define SYS_PLL_SRC   	SYS_CLK_XTAL	//????SYS_CLK_20MHz??SYS_CLK_XTAL

#define PLL_IN_DIV		3

#define PLL_FB_DIV		72


#define PLL_OUT_DIV8	0
#define PLL_OUT_DIV4	1
#define PLL_OUT_DIV2	2

#define PLL_OUT_DIV		PLL_OUT_DIV8



uint32_t SystemCoreClock  = __HSI;   				//System Clock Frequency (Core Clock)
uint32_t CyclesPerUs      = (__HSI / 1000000); 		//Cycles per micro second


/****************************************************************************************************************************************** 
* ????????: 
* ???????: This function is used to update the variable SystemCoreClock and must be called whenever the core clock is changed
* ??    ??: 
* ??    ??: 
* ???????: 
******************************************************************************************************************************************/
void SystemCoreClockUpdate(void)    
{
	if(SYS->CLKSEL & SYS_CLKSEL_SYS_Msk)			//SYS  <= HRC
	{
		if(SYS->HRCCR & SYS_HRCCR_DBL_Msk)				//HRC = 40MHz
		{
			SystemCoreClock = __HSI*2;
		}
		else											//HRC = 20MHz
		{
			SystemCoreClock = __HSI;
		}
	}
	else											//SYS  <= CLK
	{
		switch((SYS->CLKSEL & SYS_CLKSEL_CLK_Msk) >> SYS_CLKSEL_CLK_Pos)
		{
		case 0:
			SystemCoreClock = __LSI;
			break;
		
		case 1:
			if(SYS->PLLCR & SYS_PLLCR_INSEL_Msk)			//PLL_IN <= HRC
			{
				SystemCoreClock = __HSI;
			}
			else											//PLL_IN <= XTAL
			{
				SystemCoreClock = __HSE;
			}
			
			SystemCoreClock = SystemCoreClock / PLL_IN_DIV * PLL_FB_DIV * 4 / (2 << (2 - PLL_OUT_DIV));
			break;
		
		case 2:
			SystemCoreClock = __LSE;
			break;
		
		case 3:
			SystemCoreClock = __HSE;
			break;
		
		case 4:
			SystemCoreClock = __HSI;
			if(SYS->HRCCR & SYS_HRCCR_DBL_Msk)  SystemCoreClock *= 2;
			break;
		}
		
		if(SYS->CLKSEL & SYS_CLKSEL_CLK_DIVx_Msk)  SystemCoreClock /= 8;
	}
	
	CyclesPerUs = SystemCoreClock / 1000000;
}
void FPU_Enable(void);

/****************************************************************************************************************************************** 
* ????????: 
* ???????: The necessary initializaiton of systerm
* ??    ??: 
* ??    ??: 
* ???????: 
******************************************************************************************************************************************/
void SystemInit(void)
{		
	SYS->CLKEN0 |= (1 << SYS_CLKEN0_ANAC_Pos);
	
	Flash_Param_at_xMHz(150);
	
	switch(SYS_CLK)
	{
		case SYS_CLK_20MHz:
			switchTo20MHz();
			break;
		
		case SYS_CLK_2M5Hz:
			switchTo2M5Hz();
			break;
		
		case SYS_CLK_40MHz:
			switchTo40MHz();
			break;
		
		case SYS_CLK_5MHz:
			switchTo5MHz();
			break;
		
		case SYS_CLK_XTAL:
			switchToXTAL(0);
			break;
		
		case SYS_CLK_XTAL_DIV8:
			switchToXTAL(1);
			break;
		
		case SYS_CLK_PLL:
			switchToPLL(0);
			break;
		
		case SYS_CLK_PLL_DIV8:
			switchToPLL(1);
			break;
		
		case SYS_CLK_32KHz:
			switchTo32KHz();
			break;
		
		case SYS_CLK_XTAL_32K:
			switchToXTAL_32K();
			break;
	}
	
	SystemCoreClockUpdate();
	
	if(SystemCoreClock > 120000000)
	{
		Flash_Param_at_xMHz(150);
	}
	else if(SystemCoreClock > 80000000)
	{
		Flash_Param_at_xMHz(120);
	}
	else if(SystemCoreClock > 40000000)
	{
		Flash_Param_at_xMHz(80);
	}
	else if(SystemCoreClock > 30000000)
	{
		Flash_Param_at_xMHz(40);
	}
	else
	{
		Flash_Param_at_xMHz(30);
	}

	PORTM->PULLD &= ~(1 << PIN1);
	PORTM->PULLU &= ~((1 << PIN2) | (1 << PIN3));
	
	SYS->USBPHYCR &= ~SYS_USBPHYCR_OPMODE_Msk;
	SYS->USBPHYCR |= (1 << SYS_USBPHYCR_OPMODE_Pos);  	//Non-Driving, DP Pull-Up disable

	SCB->VTOR = 0x00000000 | CONFIG_KNL_OFFSET;

	FPU_Enable();
}


void FPU_Enable(void)
{
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= (0xF << 20);
#endif	
}


static void delay_3ms(void)
{
	uint32_t i;
	
	if(((SYS->CLKSEL & SYS_CLKSEL_SYS_Msk) == 0) &&
	   ((((SYS->CLKSEL & SYS_CLKSEL_CLK_Msk) >> SYS_CLKSEL_CLK_Pos) == 0) ||   //LSI 32KHz
	    (((SYS->CLKSEL & SYS_CLKSEL_CLK_Msk) >> SYS_CLKSEL_CLK_Pos) == 2)))	   //LSE 32KHz
	{
		for(i = 0; i < 20; i++) __NOP();
	}
	else
	{
		for(i = 0; i < 20000; i++) __NOP();
	}
}


void switchTo20MHz(void)
{
	SYS->HRCCR = (1 << SYS_HRCCR_ON_Pos) |
				 (0 << SYS_HRCCR_DBL_Pos);			//HRC = 20Hz
	
	delay_3ms();
	
	SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);		//SYS <= HRC
}

void switchTo2M5Hz(void)
{
	switchTo20MHz();
	
	SYS->CLKDIVx_ON = 1;
	
	SYS->CLKSEL &= ~SYS_CLKSEL_CLK_Msk;
	SYS->CLKSEL |= (4 << SYS_CLKSEL_CLK_Pos);		//CLK <= HRC

	SYS->CLKSEL |= (1 << SYS_CLKSEL_CLK_DIVx_Pos);
	
	delay_3ms();
	
	SYS->CLKSEL &=~(1 << SYS_CLKSEL_SYS_Pos);		//SYS <= HRC/8
}

void switchTo40MHz(void)
{
	SYS->HRCCR = (1 << SYS_HRCCR_ON_Pos) |
				 (1 << SYS_HRCCR_DBL_Pos);			//HRC = 40MHz
	
	delay_3ms();
	
	SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);		//SYS <= HRC
}

void switchTo5MHz(void)
{
	switchTo40MHz();
	
	SYS->CLKDIVx_ON = 1;
	
	SYS->CLKSEL &= ~SYS_CLKSEL_CLK_Msk;
	SYS->CLKSEL |= (4 << SYS_CLKSEL_CLK_Pos);		//CLK <= HRC

	SYS->CLKSEL |= (1 << SYS_CLKSEL_CLK_DIVx_Pos);
	
	delay_3ms();
	
	SYS->CLKSEL &=~(1 << SYS_CLKSEL_SYS_Pos);		//SYS <= HRC/8
}

void switchToXTAL(uint32_t div8)
{	
	switchTo20MHz();
	
	PORT_Init(PORTA, PIN3, PORTA_PIN3_XTAL_IN,  0);
	PORT_Init(PORTA, PIN4, PORTA_PIN4_XTAL_OUT, 0);
	SYS->XTALCR |= (1 << SYS_XTALCR_ON_Pos) | (15 << SYS_XTALCR_DRV_Pos) | (1 << SYS_XTALCR_DET_Pos);
	
	delay_3ms();
	delay_3ms();
	
	SYS->CLKDIVx_ON = 1;
	
	SYS->CLKSEL &= ~SYS_CLKSEL_CLK_Msk;
	SYS->CLKSEL |= (3 << SYS_CLKSEL_CLK_Pos);		//CLK <= XTAL

	if(div8) SYS->CLKSEL |= (1 << SYS_CLKSEL_CLK_DIVx_Pos);
	else     SYS->CLKSEL &=~(1 << SYS_CLKSEL_CLK_DIVx_Pos);
	
	SYS->CLKSEL &=~(1 << SYS_CLKSEL_SYS_Pos);		//SYS <= XTAL
}

void switchToPLL(uint32_t div8)
{
	switchTo20MHz();
	
	PLLInit();
	
	SYS->CLKDIVx_ON = 1;
	
	SYS->CLKSEL &= ~SYS_CLKSEL_CLK_Msk;
	SYS->CLKSEL |= (1 << SYS_CLKSEL_CLK_Pos);		//CLK <= PLL

	if(div8)  SYS->CLKSEL |= (1 << SYS_CLKSEL_CLK_DIVx_Pos);
	else      SYS->CLKSEL &=~(1 << SYS_CLKSEL_CLK_DIVx_Pos);
	
	SYS->CLKSEL &=~(1 << SYS_CLKSEL_SYS_Pos);		//SYS <= PLL
}

void switchTo32KHz(void)
{
	switchTo20MHz();
	
	SYS->LRCCR = (1 << SYS_LRCCR_ON_Pos);
	
	SYS->CLKDIVx_ON = 1;
	
	SYS->CLKSEL &= ~SYS_CLKSEL_CLK_Msk;
	SYS->CLKSEL |= (0 << SYS_CLKSEL_CLK_Pos);		//CLK <= LRC

	SYS->CLKSEL &=~(1 << SYS_CLKSEL_CLK_DIVx_Pos);
	
	delay_3ms();

	SYS->CLKSEL &=~(1 << SYS_CLKSEL_SYS_Pos);		//SYS <= LRC
}

void switchToXTAL_32K(void)
{
	uint32_t i;
	
	switchTo20MHz();
	
	SYS->XTALCR |= (1 << SYS_XTALCR_32KON_Pos) | (7 << SYS_XTALCR_32KDRV_Pos) | (1 << SYS_XTALCR_32KDET_Pos);
	for(i = 0; i < 1000; i++) __NOP();
	
	SYS->CLKDIVx_ON = 1;
	
	SYS->CLKSEL &= ~SYS_CLKSEL_CLK_Msk;
	SYS->CLKSEL |= (2 << SYS_CLKSEL_CLK_Pos);		//CLK <= XTAL_32K

	SYS->CLKSEL &=~(1 << SYS_CLKSEL_CLK_DIVx_Pos);
	
	delay_3ms();

	SYS->CLKSEL &=~(1 << SYS_CLKSEL_SYS_Pos);		//SYS <= XTAL_32K
}

void PLLInit(void)
{	
	if(SYS_PLL_SRC == SYS_CLK_20MHz)
	{
		SYS->HRCCR = (1 << SYS_HRCCR_ON_Pos) |
					 (0 << SYS_HRCCR_DBL_Pos);		//HRC = 20Hz
		
		delay_3ms();
		
		SYS->PLLCR |= (1 << SYS_PLLCR_INSEL_Pos);	//PLL_SRC <= HRC
	}
	else if(SYS_PLL_SRC == SYS_CLK_XTAL)
	{
		PORT_Init(PORTA, PIN3, PORTA_PIN3_XTAL_IN,  0);
		PORT_Init(PORTA, PIN4, PORTA_PIN4_XTAL_OUT, 0);
		SYS->XTALCR |= (1 << SYS_XTALCR_ON_Pos) | (15 << SYS_XTALCR_DRV_Pos) | (1 << SYS_XTALCR_DET_Pos);
		
		delay_3ms();
		delay_3ms();
		
		SYS->PLLCR &= ~(1 << SYS_PLLCR_INSEL_Pos);	//PLL_SRC <= XTAL
	}
	
	SYS->PLLDIV &= ~(SYS_PLLDIV_INDIV_Msk |
					 SYS_PLLDIV_FBDIV_Msk |
					 SYS_PLLDIV_OUTDIV_Msk);
	SYS->PLLDIV |= (PLL_IN_DIV << SYS_PLLDIV_INDIV_Pos) |
				   (PLL_FB_DIV << SYS_PLLDIV_FBDIV_Pos) |
				   (PLL_OUT_DIV<< SYS_PLLDIV_OUTDIV_Pos);
	
	SYS->PLLCR &= ~(1 << SYS_PLLCR_OFF_Pos);
	
	while(SYS->PLLLOCK == 0);		//???PLL????
	
	SYS->PLLCR |= (1 << SYS_PLLCR_OUTEN_Pos);
}
