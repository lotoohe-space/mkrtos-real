/********************************************************************************
 * @File name: MD_RTU_Serial.c
 * @Author: zspace
 * @Emial: 1358745329@qq.com
 * @Version: 1.0
 * @Date: 2020-4-10
 * @Description: Modbus RTU Serial related modules
 * Open source address: https://github.com/lotoohe-space/XTinyModbus
 ********************************************************************************/

/*********************************HEAD FILE************************************/
#include "MDM_RTU_Serial.h"
#include "MD_RTU_Tool.h"
#include "wk2xx_hw.h"
#include "uart5.h"
#include "rs485.h"
#include <assert.h>
/*User-related header files*/

/*********************************END******************************************/
#define WK_UART_NUM1 4
/*********************************GLOBAL VARIABLE************************************/
PModbusBase pModbusMBase = NULL; /*Modbus of the current serial port*/
/*********************************END******************************************/

/*********************************FUNCTION DECLARATION************************************/

// void MDMTimeHandler100US(void);
/*********************************END******************************************/

/*******************************************************
 *
 * Function name :MDMInitSerial
 * Description        	:Hardware initialization function, you can initialize the serial port here
 * Parameter         	:
 *        @obj        	Host object pointer
 *        @baud    		Baud rate
 *        @dataBits    Data bit
 *        @stopBit    	Stop bit
 *        @parity    	Parity bit
 * Return          		:None
 **********************************************************/
void MDMInitSerial(void *obj, uint32 baud, uint8 dataBits, uint8 stopBit, uint8 parity)
{
	pModbusMBase = obj;
	if (obj == NULL)
	{
		return;
	}

	pModbusMBase->mdRTUSendBytesFunction = MDMSerialSendBytes;
	pModbusMBase->mdRTURecSendConv = MDMSerialSWRecv_Send;

	/*Hardware initialization*/
	assert(baud == 9600);
	Wk_SetBaud(WK_UART_NUM1, B9600);
	init_uart5(B9600);
}
/*******************************************************
 *
 * Function name :MDMTimeHandler100US
 * Description        :Call this function in the timer
 * Parameter         :None
 * Return          :None
 **********************************************************/
void MDMTimeHandler100US(uint32 times)
{
	if (pModbusMBase == NULL)
	{
		return;
	}
	pModbusMBase->mdRTUTimeHandlerFunction(pModbusMBase, times);
}
/*******************************************************
 *
 * Function name :MDMSerialRecvByte
 * Description        :Bsp layer serial port interrupt receiving call this function
 * Parameter         :
 *        @byte    Byte received
 * Return          : None
 **********************************************************/
void MDMSerialRecvByte(uint8 byte)
{
	if (pModbusMBase == NULL)
	{
		return;
	}
	pModbusMBase->mdRTURecByteFunction(pModbusMBase, byte);
}
/*******************************************************
 *
 * Function name :MDMSerialRecvByte
 * Description        :Switch to receive or send
 * Parameter         :
 *        @mode    TRUE send, FALSE receive
 * Return          : None
 **********************************************************/
void MDMSerialSWRecv_Send(uint8 mode)
{
	/*Send and receive conversion*/
	/*Fill in the converted code below*/
	RS4850_RW_CONV = mode;
	RS4851_RW_CONV = mode;
	/*Different hardware may require a little delay after setting conversion*/
}
/*******************************************************
 *
 * Function name :MDMSerialSendBytes
 * Description        :Send function
 * Parameter         :
 *        @bytes    Data sent
 *        @num    How many bytes to send
 * Return          : None
 **********************************************************/
void MDMSerialSendBytes(uint8 *bytes, uint16 num)
{

	uart5_send_bytes(bytes, num);

	/*在下面调用bsp的发送函数*/
	wk_TxChars(WK_UART_NUM1, num, bytes);
	// 等待发送完成
	wk_wait_tx_done(WK_UART_NUM1);
}
