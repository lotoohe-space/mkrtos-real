/**
 * @file MDM_RTU_APP.c
 * @author ATShining (1358745329@qq.com)
 * @brief https://github.com/lotoohe-space/XTinyModbus
 * @version 0.1
 * @date 2023-10-03
 *
 * @copyright Copyright (c) 2023
 *
 */

/*********************************HEAD FILE************************************/
#include "MDM_RTU_APP.h"
#include "MDM_RTU_Fun.h"
#include "MDM_RTU_Serial.h"
#include "MDM_RTU_User_Fun.h"
#include "MDM_RTU_RW_Man.h"
#include "u_sys.h"
#include "sysinfo.h"
/*********************************END******************************************/
#define USE_RS485_DB 1
/*********************************全局变量************************************/
uint16 regDataMaster0[32] = {0};
MapTableItem mapTableItemMaster0 = {
	.modbusAddr = 0x0000,		  /*MODBUS中的地址*/
	.modbusData = regDataMaster0, /*映射的内存单元*/
	.modbusDataSize = 32,		  /*映射的大小*/
	.addrType = HOLD_REGS_TYPE,	  /*映射的类型*/
	.devAddr = 6,				  /*被哪个从机使用*/
};
uint16 regDataMaster1[32] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
MapTableItem mapTableItemMaster1 = {
	.modbusAddr = 0x0000,		  /*MODBUS中的地址*/
	.modbusData = regDataMaster1, /*映射的内存单元*/
	.modbusDataSize = 32,		  /*映射的大小*/
	.addrType = HOLD_REGS_TYPE,	  /*映射的类型*/
	.devAddr = 7,				  /*被哪个从机使用*/
};
uint16 regDataMaster2[32] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
MapTableItem mapTableItemMaster2 = {
	.modbusAddr = 0x0000,		  /*MODBUS中的地址*/
	.modbusData = regDataMaster2, /*映射的内存单元*/
	.modbusDataSize = 32,		  /*映射的大小*/
	.addrType = HOLD_REGS_TYPE,	  /*映射的类型*/
	.devAddr = 8,				  /*被哪个从机使用*/
};

Modbus_RTU modbus_RTU = {0};
Modbus_RTU_CB modbusRWRTUCB = {0};
Modbus_RTU_CB modbusRWRTUCB1 = {0};
Modbus_RTU_CB modbusRWRTUCB2 = {0};
Modbus_RTU_CB modbusRWRTUCB3 = {0};
Modbus_RTU_CB modbusRWRTUCB4 = {0};
static uint8_t recv_buf[257];
/*********************************结束******************************************/

/*******************************************************
 *
 * Function name :MDM_RTU_APPInit
 * Description        :主机用户APP初始化函数
 * Parameter         :无
 * Return          : TRUE success , FALSE fail
 **********************************************************/
BOOL MDM_RTU_APPInit(void)
{
	if (MDM_RTU_Init(&modbus_RTU, MDMInitSerial, 9600, 8, 1, 0) != ERR_NONE)
	{
		return FALSE;
	}
	MDM_RTU_QueueInit(&modbus_RTU,
					  recv_buf,
					  sizeof(recv_buf));

	if (MDM_RTU_AddMapItem(&modbus_RTU, &mapTableItemMaster0) == FALSE)
	{
		return FALSE;
	}
	if (MDM_RTU_AddMapItem(&modbus_RTU, &mapTableItemMaster1) == FALSE)
	{
		return FALSE;
	}
	if (MDM_RTU_AddMapItem(&modbus_RTU, &mapTableItemMaster2) == FALSE)
	{
		return FALSE;
	}

	/*RW控制块*/
	MDM_RTU_CB_Init(&modbusRWRTUCB, &modbus_RTU, 3000, 3000, 1);
	MDM_RTU_CB_Init(&modbusRWRTUCB1, &modbus_RTU, 3000, 3000, 1);
	MDM_RTU_CB_Init(&modbusRWRTUCB2, &modbus_RTU, 3000, 3000, 1);
	MDM_RTU_CB_Init(&modbusRWRTUCB3, &modbus_RTU, 3000, 3000, 1);
	MDM_RTU_CB_Init(&modbusRWRTUCB4, &modbus_RTU, 3000, 3000, 1);
	return TRUE;
}
// uint16 temp = ~(0x5555);
// uint16 temp2 = 0x5555;
// uint16 temp1 = 1234;
// uint16 data1[] = {1, 2, 3, 4, 5, 6};
#define MD_NB_MODE_TEST 1
/*用户读取数据*/
static void MDM_RTUUserRead(void)
{
	uint16 resTemp;
#if USE_RS485_DB
#if MD_NB_MODE_TEST
	MDError res;
	res = MDM_RTU_NB_ReadHoldReg(&modbusRWRTUCB, 6, 0, 11);
	if (res != ERR_IDLE)
	{
		if (res != ERR_RW_FIN)
		{ /*出现错误*/
			if (res == ERR_RW_OV_TIME_ERR)
			{ /*超时了*/
				/*使能重传*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB);
			}
		}
		else
		{

			/*读成功*/
			MDM_RTU_ReadRegs(modbusRWRTUCB.pModbus_RTU, 0x0000, 1, &resTemp, HOLD_REGS_TYPE, 6);
			sys_info.noise[0] = (float)(resTemp) / 10.0f;

			MDM_RTU_ReadRegs(modbusRWRTUCB.pModbus_RTU, 10, 1, &resTemp, HOLD_REGS_TYPE, 6);
			sys_info.noise_temp[0] = (float)(resTemp);
		}
	}
	res = MDM_RTU_NB_ReadHoldReg(&modbusRWRTUCB1, 7, 0, 11);
	if (res != ERR_IDLE)
	{
		if (res != ERR_RW_FIN)
		{ /*出现错误*/
			if (res == ERR_RW_OV_TIME_ERR)
			{ /*超时了*/
				/*使能重传*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB1);
			}
		}
		else
		{
			/*读成功*/
			MDM_RTU_ReadRegs(modbusRWRTUCB1.pModbus_RTU, 0x0000, 1, &resTemp, HOLD_REGS_TYPE, 7);
			sys_info.noise[1] = (float)(resTemp) / 10.0f;

			MDM_RTU_ReadRegs(modbusRWRTUCB1.pModbus_RTU, 10, 1, &resTemp, HOLD_REGS_TYPE, 7);
			sys_info.noise_temp[1] = (float)(resTemp);
		}
	}
	res = MDM_RTU_NB_ReadHoldReg(&modbusRWRTUCB2, 8, 0, 11);
	if (res != ERR_IDLE)
	{
		if (res != ERR_RW_FIN)
		{ /*出现错误*/
			if (res == ERR_RW_OV_TIME_ERR)
			{ /*超时了*/
				/*使能重传*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB2);
			}
		}
		else
		{
			/*读成功*/
			MDM_RTU_ReadRegs(modbusRWRTUCB2.pModbus_RTU, 0x0000, 1, &resTemp, HOLD_REGS_TYPE, 8);
			sys_info.noise[2] = (float)(resTemp) / 10.0f;

			MDM_RTU_ReadRegs(modbusRWRTUCB2.pModbus_RTU, 10, 1, &resTemp, HOLD_REGS_TYPE, 8);
			sys_info.noise_temp[2] = (float)(resTemp);
		}
	}
#else

	if (MDM_RTU_ReadCoil(&modbusRWRTUCB, 0x1, 0x0000, 16) == ERR_RW_FIN)
	{
		MDM_RTU_ReadBits(modbusRWRTUCB.pModbus_RTU, 0x0000, 16, (uint8 *)&resTemp, COILS_TYPE, 0x1);
		resTemp = resTemp;
	}
#endif
#endif
}

// static void MDM_RTUUserWrite(void)
// {
// 	MDError res;
// #if MD_NB_MODE_TEST
// 	res = MDM_RTU_NB_WriteCoils(&modbusRWRTUCB1, 0x1, 0, 16, (uint8 *)(&temp));
// 	if (res != ERR_IDLE)
// 	{
// 		if (res != ERR_RW_FIN)
// 		{ /*出现错误*/
// 			if (res == ERR_RW_OV_TIME_ERR)
// 			{ /*超时了*/
// 				/*使能重传*/
// 				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB1);
// 			}
// 		}
// 	}
// 	res = MDM_RTU_NB_WriteCoils(&modbusRWRTUCB4, 0x1, 0, 16, (uint8 *)(&temp2));
// 	if (res != ERR_IDLE)
// 	{
// 		if (res != ERR_RW_FIN)
// 		{ /*出现错误*/
// 			if (res == ERR_RW_OV_TIME_ERR)
// 			{ /*超时了*/
// 				/*使能重传*/
// 				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB4);
// 			}
// 		}
// 	}
// #else
// 	MDM_RTU_WriteCoils(&modbusRWRTUCB1, 0x1, 0, 16, (uint8 *)(&temp));
// 	MDM_RTU_WriteCoils(&modbusRWRTUCB4, 0x1, 0, 16, (uint8 *)(&temp2));

// 	// MDM_RTU_WriteRegs(&modbusRWRTUCB4,0x1,0,6,data1);
// #endif
// }
/*用户数据的读写*/
static void MDM_RTU_UserUpdate(void)
{
	MDM_RTUUserRead();
	// MDM_RTUUserWrite();
}

/*Loop call*/
void MDM_RTU_Loop(void)
{
	MDMTimeHandler100US(sys_read_tick() * 10);
	MDM_RTU_UserUpdate();
}
