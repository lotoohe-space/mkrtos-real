/**
 * @file msg.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "msg.h"
#include "gui.h"
#include "x_queue.h"
#include "x_malloc.h"

/*队列最大长度*/
#define MSG_EVENT_MAX_VAL 64

/*事件队列*/
xqueue_t eventMsg;
/*重绘队列*/
xqueue_t drawMsg;
/*窗口移动队列*/
xqueue_t moveMsg;
/*删除*/
xqueue_t winDelMsg;
/*鼠标移动*/
xqueue_t cursorMsg;

/*
 * GUI初始化
 */
uint8_t GUIMsgEventInit(void)
{
	uint8_t res;
	res = InitQueue(&eventMsg, MSG_EVENT_MAX_VAL);
	if (!res)
	{
		return FALSE;
	}

	res = InitQueue(&drawMsg, MSG_EVENT_MAX_VAL);
	if (!res)
	{
		DestroyQueue(&eventMsg);
		return FALSE;
	}
	res = InitQueue(&moveMsg, MSG_EVENT_MAX_VAL);
	if (!res)
	{
		DestroyQueue(&eventMsg);
		DestroyQueue(&drawMsg);
		return FALSE;
	}
	res = InitQueue(&winDelMsg, MSG_EVENT_MAX_VAL);
	if (!res)
	{
		DestroyQueue(&eventMsg);
		DestroyQueue(&drawMsg);
		DestroyQueue(&moveMsg);
		return FALSE;
	}
	res = InitQueue(&cursorMsg, MSG_EVENT_MAX_VAL);
	if (!res)
	{
		DestroyQueue(&eventMsg);
		DestroyQueue(&drawMsg);
		DestroyQueue(&moveMsg);
		DestroyQueue(&winDelMsg);
		return FALSE;
	}
	eventMsg.valid = FALSE;
	drawMsg.valid = FALSE;
	moveMsg.valid = FALSE;
	winDelMsg.valid = FALSE;
	cursorMsg.valid = FALSE;

	return TRUE;
}
void GUIEventValid(void)
{
	eventMsg.valid = TRUE;
	drawMsg.valid = TRUE;
	moveMsg.valid = TRUE;
	winDelMsg.valid = TRUE;
	cursorMsg.valid = TRUE;
}
/*获取消息*/
p_msg_t GUIGetMsg(void)
{
	QueueDateType e;
	if (deQueue(&eventMsg, &e))
	{
		return (p_msg_t)e;
	}
	return NULL;
}
void GUIDelMsg(p_msg_t hMsg)
{
	XFree(hMsg);
}

int8_t GUISendKeyMsg(uint8_t ID, uint8_t status)
{
	p_msg_t hMsg;
	if (eventMsg.valid == FALSE)
	{
		return FALSE;
	}
	hMsg = XMalloc(sizeof(msg_t));
	if (hMsg == NULL)
	{
		return FALSE;
	}

	hMsg->msgSrc = NULL;
	hMsg->msgType = MSG_KEY;
	hMsg->msgID = ID;
	hMsg->msgVal.status = status;
	// hMsg->msgVal.what = keyVal;

	enQueue(&eventMsg, hMsg);
	// GUIPostEvent(&Msg);
	return TRUE;
}

/*发送消息到队列*/
int8_t GUISendTouchMsg(int ID, int16_t x, int16_t y)
{
	p_msg_t hMsg;
	if (eventMsg.valid == FALSE)
	{
		return FALSE;
	}
	hMsg = XMalloc(sizeof(msg_t));
	if (hMsg == NULL)
	{
		return FALSE;
	}

	hMsg->msgSrc = NULL;
	hMsg->msgType = MSG_TOUCH;
	hMsg->msgID = ID;
	hMsg->msgVal.rect.x = x;
	hMsg->msgVal.rect.y = y;

	enQueue(&eventMsg, hMsg);
	// GUIPostEvent(&Msg);
	return TRUE;
}
int GUISendDrawMsg(void *hWin, uint8_t msgType, uint8_t msgID, int16_t x, int16_t y, uint16_t w, uint16_t h
				   //	,int16_t x1,int16_t y1,uint16_t w1,uint16_t h1
)
{
	p_msg_t hMsg;
	// if (hWin == NULL) { return FALSE; }
	if (drawMsg.valid == FALSE)
	{
		return FALSE;
	}
	hMsg = XMalloc(sizeof(msg_t));
	if (hMsg == NULL)
	{
		return FALSE;
	}

	hMsg->msgSrc = hWin;
	hMsg->msgType = msgType;
	hMsg->msgID = msgID;
	hMsg->msgVal.rect.x = x;
	hMsg->msgVal.rect.y = y;
	hMsg->msgVal.rect.w = w;
	hMsg->msgVal.rect.h = h;
	/*hMsg->msgVal1.rect.x = x1;
	hMsg->msgVal1.rect.y = y1;
	hMsg->msgVal1.rect.w = w1;
	hMsg->msgVal1.rect.h = h1;*/
	enQueue(&drawMsg, hMsg);

	// GUIPostEvent(&Msg);
	return TRUE;
}

/*获取消息*/
p_msg_t GUIGetDrawMsg(void)
{
	QueueDateType e;
	if (deQueue(&drawMsg, &e))
	{
		return (p_msg_t)e;
	}

	return NULL;
}
void GUIDelDrawMsg(p_msg_t hMsg)
{
	XFree(hMsg);
}

int GUISendMoveMsg(void *hWin, uint8_t msgType, uint8_t msgID, int16_t x, int16_t y)
{
	p_msg_t hMsg;
	// if (hWin == NULL) { return FALSE; }
	if (moveMsg.valid == FALSE)
	{
		return FALSE;
	}
	hMsg = XMalloc(sizeof(msg_t));
	if (hMsg == NULL)
	{
		return FALSE;
	}

	hMsg->msgSrc = hWin;
	hMsg->msgType = msgType;
	hMsg->msgID = msgID;
	hMsg->msgVal.rect.x = x;
	hMsg->msgVal.rect.y = y;

	enQueue(&moveMsg, hMsg);

	return TRUE;
}
p_msg_t GUIGetMoveMsg(void)
{
	QueueDateType e, e1;
next:
	if (deQueue(&moveMsg, &e))
	{
		if (getTailQueue(&moveMsg, &e1) == FALSE)
		{
			return (p_msg_t)e;
		}
		if (((p_msg_t)e)->msgSrc == ((p_msg_t)e1)->msgSrc)
		{
			goto next;
		}
		else
		{
			return (p_msg_t)e;
		}
	}

	return NULL;
}
void GUIDelMoveMsg(p_msg_t hMsg)
{
	XFree(hMsg);
}
int8_t GUISendWINDelMsg(void *msgSrc, void *delItem)
{
	p_msg_t hMsg;
	if (winDelMsg.valid == FALSE)
	{
		return FALSE;
	}
	hMsg = XMalloc(sizeof(msg_t));
	if (hMsg == NULL)
	{
		return FALSE;
	}
	hMsg->msgSrc = msgSrc;
	hMsg->msgType = MSG_WIN;
	hMsg->msgID = MSG_WIN_DEL_WIDGE;
	hMsg->msgVal.v = delItem;

	enQueue(&winDelMsg, hMsg);
	return TRUE;
}
/*获取消息*/
p_msg_t GUIGetWINDelMsg(void)
{
	QueueDateType e;
	if (deQueue(&winDelMsg, &e))
	{
		return (p_msg_t)e;
	}

	return NULL;
}
void GUIDelWinDelMsg(p_msg_t hMsg)
{
	XFree(hMsg);
}

int8_t GUISendCursorMsg(uint16_t ID, int16_t x, int16_t y)
{
	p_msg_t hMsg;
	if (cursorMsg.valid == FALSE)
	{
		return FALSE;
	}
	hMsg = XMalloc(sizeof(msg_t));
	if (hMsg == NULL)
	{
		return FALSE;
	}

	hMsg->msgSrc = NULL;
	hMsg->msgType = MSG_CURSOR;
	hMsg->msgID = ID;
	hMsg->msgVal.rect.x = x;
	hMsg->msgVal.rect.y = y;
	// hMsg->msgVal.what = keyVal;

	enQueue(&cursorMsg, hMsg);
	// GUIPostEvent(&Msg);
	return TRUE;
}
void GUIDelCursorMsg(p_msg_t hMsg)
{
	XFree(hMsg);
}
/*获取消息*/
p_msg_t GUIGetCursorMsg(void)
{
	QueueDateType e;
	if (deQueue(&cursorMsg, &e))
	{
		return (p_msg_t)e;
	}

	return NULL;
}