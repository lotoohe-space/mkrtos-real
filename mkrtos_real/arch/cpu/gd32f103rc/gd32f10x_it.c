/*!
    \file    gd32f10x_it.c
    \brief   interrupt service routines

    \version 2014-12-26, V1.0.0, firmware for GD32F10x
    \version 2017-06-20, V2.0.0, firmware for GD32F10x
    \version 2018-07-31, V2.1.0, firmware for GD32F10x
    \version 2020-09-30, V2.2.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f10x_it.h"
#include <mkrtos/sched.h>
//#include "systick.h"

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
	unsigned char *mfsr=(void*)0xe000ed28;
	unsigned char *bfsr=(void*)0xe000ed29;
	unsigned short *ufsr=(void*)0xe000ed2a;
	unsigned int *hard_fsr=(void*)0xe000ed2c;
	unsigned char *dfsr=(void*)0xe000ed30;
	kprint("----------------------%s----------------\r\n",__FUNCTION__);
	kprint("mfsr 0x%x\r\n",*mfsr);
	kprint("bfsr 0x%x\r\n",*bfsr);
	kprint("ufsr 0x%x\r\n",*ufsr);
	kprint("hard_fsr 0x%x\r\n",*hard_fsr);
	kprint("dfsr 0x%x\r\n",*dfsr);
	kprint("----------------------------------------\r\n");
    kprint("kill pid is %d.\n", get_current_task()->pid);
	/* if Hard Fault exception occurs, go to infinite loop */
	do_exit(-1);
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{

	uint32_t *mmfar = (uint32_t *)(0xE000ED34);

	kprint("R/W mem falut, addr:0x%x.\n", *mmfar);
	kprint("closing current task.\n");
	do_exit(-1);
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1){
    }
}

///*!
//    \brief      this function handles SVC exception
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//void SVC_Handler(void)
//{
//}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

///*!
//    \brief      this function handles PendSV exception
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//void PendSV_Handler(void)
//{
//}
//
///*!
//    \brief      this function handles SysTick exception
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//void SysTick_Handler(void)
//{
//    delay_decrement();
//}
