/**
 * @file msg.h
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "x_types.h"

/*****************消息类型************************/
#define MSG_TOUCH 0
#define MSG_KEY 1
#define MSG_WIN 2
#define MSG_CURSOR 3
/*************************************************/

/********游标事件********/
#define MSG_CURSOR_MOVE 0 /*游标改变位置*/
/************************/

/***************MSG_KEY 消息ID********************/
#define MSG_KEY_LEFT 37
#define MSG_KEY_UP 38
#define MSG_KEY_RIGHT 39
#define MSG_KEY_DOWN 40

#define MSG_KEY_0 49
#define MSG_KEY_1 50
#define MSG_KEY_2 51
#define MSG_KEY_3 52
#define MSG_KEY_4 53
#define MSG_KEY_5 54
#define MSG_KEY_6 55
#define MSG_KEY_7 56
#define MSG_KEY_8 57
#define MSG_KEY_9 58
#define MSG_KEY_A 65
#define MSG_KEY_B 66
#define MSG_KEY_C 67
#define MSG_KEY_D 68
#define MSG_KEY_E 69
#define MSG_KEY_F 70
#define MSG_KEY_G 71
#define MSG_KEY_H 72
#define MSG_KEY_I 73
#define MSG_KEY_J 74
#define MSG_KEY_K 75
#define MSG_KEY_L 76
#define MSG_KEY_M 77
#define MSG_KEY_N 78
#define MSG_KEY_O 79
#define MSG_KEY_P 80
#define MSG_KEY_Q 81
#define MSG_KEY_R 82
#define MSG_KEY_S 83
#define MSG_KEY_T 84
#define MSG_KEY_U 85
#define MSG_KEY_V 86
#define MSG_KEY_W 87
#define MSG_KEY_X 88
#define MSG_KEY_Y 89
#define MSG_KEY_Z 90
/*************************************************/

/***************MSG_TOUCH 消息ID******************/
#define MSG_TOUCH_PRESS 0
#define MSG_TOUCH_MOVE 1
#define MSG_TOUCH_RELEASE 2
/*************************************************/

/******************窗口有关的事件*****************/
#define MSG_WIN_MOVE 0
#define MSG_WIN_INVAILD_UPDATE 1
#define MSG_WIN_CREATE 2
#define MSG_WIN_PAINT 3
#define MSG_WIN_DEL_WIDGE 4
/*************************************************/

typedef struct
{
	void *msgSrc;  /*消息源*/
	uint8_t msgType; /*消息类型*/
	uint8_t msgID;   /*消息ID*/
	union
	{
		uint8_t status; /*按键的状态*/
		uint32_t what;
		void *v;
		/*设置消息的宽高*/
		struct
		{
			/*坐标*/
			int16_t x;
			int16_t y;
			/*宽高*/
			uint16_t w;
			uint16_t h;
		} rect;
	} msgVal;
} *p_msg_t, msg_t;

uint8_t GUIMsgEventInit(void);
void GUIEventValid(void);
p_msg_t GUIGetMsg(void);
void GUIDelMsg(p_msg_t hMsg);

int8_t GUISendKeyMsg(uint8_t ID, uint8_t status);
int8_t GUISendTouchMsg(int ID, int16_t x, int16_t y);

int GUISendDrawMsg(void *hWin, uint8_t msgType, uint8_t msgID, int16_t x, int16_t y, uint16_t w, uint16_t h);
p_msg_t GUIGetDrawMsg(void);
void GUIDelDrawMsg(p_msg_t hMsg);

int GUISendMoveMsg(void *hWin, uint8_t msgType, uint8_t msgID, int16_t x, int16_t y);
p_msg_t GUIGetMoveMsg(void);
void GUIDelMoveMsg(p_msg_t hMsg);

int8_t GUISendWINDelMsg(void *msgSrc, void *delItem);
p_msg_t GUIGetWINDelMsg(void);
void GUIDelWinDelMsg(p_msg_t hMsg);

int8_t GUISendCursorMsg(uint16_t ID, int16_t x, int16_t y);
void GUIDelCursorMsg(p_msg_t hMsg);
p_msg_t GUIGetCursorMsg(void);
