/**
 * @file GUI_timeout.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "GUI_timeout.h"
#include "x_malloc.h"
#include "gui.h"

#define GUI_TIMEOUT_MAX_COUNT 10
static guitime_out_t GUITimeoutList[GUI_TIMEOUT_MAX_COUNT];

PRIVATE p_guitime_out_t GUITimeoutNew(void)
{
	uint16_t i = 0;
	for (i = 0; i < GUI_TIMEOUT_MAX_COUNT; i++)
	{
		if (!_GetTimeoutUse(&GUITimeoutList[i]))
		{
			_SetTimeoutUse(&GUITimeoutList[i]);
			return &GUITimeoutList[i];
		}
	}
	return NULL;
}
PUBLIC void GUITimeoutFree(p_guitime_out_t hGUITimeout)
{
	if (hGUITimeout == NULL)
	{
		return;
	}
	_ClrTimeoutUse(hGUITimeout);
}

p_guitime_out_t GUITimeoutCreate(uint16_t timeoutPeriod, void *arg, GUITimeoutCallback timoutCallBackFun)
{
	p_guitime_out_t hGUITimeOut = GUITimeoutNew();
	if (hGUITimeOut == NULL)
	{
		return NULL;
	}

	hGUITimeOut->timeoutTick = GUIGetTick();
	hGUITimeOut->timeoutPeriod = timeoutPeriod;
	hGUITimeOut->timoutCallBackFun = timoutCallBackFun;
	hGUITimeOut->arg = arg;
	_OpenTimeout(hGUITimeOut);
	return hGUITimeOut;
}
void GUITimeoutOpen(p_guitime_out_t hGUITimeOut)
{
	if (hGUITimeOut == NULL)
	{
		return;
	}
	hGUITimeOut->timeoutTick = GUIGetTick();
	_OpenTimeout(hGUITimeOut);
}
void GUITimeoutClose(p_guitime_out_t hGUITimeOut)
{
	if (hGUITimeOut == NULL)
	{
		return;
	}
	_CloseTimeout(hGUITimeOut);
}
void GUITimeoutProcess(void)
{
	uint16_t i = 0;
	for (i = 0; i < GUI_TIMEOUT_MAX_COUNT; i++)
	{
		if (_GetTimeoutUse(&GUITimeoutList[i]) && _GetTimeout(&GUITimeoutList[i]))
		{
			if (GUIGetTick() - GUITimeoutList[i].timeoutTick >= GUITimeoutList[i].timeoutPeriod)
			{
				_CloseTimeout(&GUITimeoutList[i]);
				GUITimeoutList[i].timoutCallBackFun(GUITimeoutList[i].arg);
			}
		}
	}
}
