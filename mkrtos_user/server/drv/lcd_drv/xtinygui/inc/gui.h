/**
 * @file gui.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "xwindows.h"
#include "x_types.h"
#include "paint.h"
#include "mem_dev.h"
#include "config.h"

/*********返回的事件说明********/
#define RES_ASSERT_ERR -1
#define RES_OK 0
#define RES_FAIL 1
#define RES_USED 10
/*******************************/

typedef struct
{
	/*继承HWIN 桌面也是一个窗口，不过没有窗头控件*/
	p_win_t desktopWin;
	/*最顶上的窗体*/
	p_win_t topWin;
	/*需要更新的区域*/
	xrect_t updateArea;
	/*更新区域的父控件*/
	p_widget_base_t updateParentWidge;

#if USE_MEM_DEV
	/*存储设备*/
	p_mem_dev_t hMemDev;
#endif
	uint8_t flag; /*0:bit 是否初次运行*/

} *p_xdesktop_t, xdesktop_t;
extern p_xdesktop_t hXDesktop;

/*首次运行*/
#define _SET_FIRST_RUN(a) _SET_BIT((((p_xdesktop_t)(a))->flag), 0)
#define _CLR_FIRST_RUN(a) _CLR_BIT((((p_xdesktop_t)(a))->flag), 0)
#define _GET_FIRST_RUN(a) _GET_BIT((((p_xdesktop_t)(a))->flag), 0)

/*获取一个控件的绘图区域*/
#define DESKTOP_DRAW_AREA(a) ((p_xrect_t)((&((a)->desktopWin->groupWidge.widgeBase.pencil))))

/*当前控件坐标转换为相对坐标*/
/*即当前坐标相对于父控件的偏移*/
#define CDE2OPPOSITE_X(a) (((p_xrect_t)(((p_widget_base_t)(a))->parentHWIN))->x - ((p_xrect_t)(a))->x)
#define CDE2OPPOSITE_Y(a) (((p_xrect_t)(((p_widget_base_t)(a))->parentHWIN))->y - ((p_xrect_t)(a))->y)

/*得到当前坐标相对于控件坐标的相对坐标*/
#define P_CDE2OPPOSITE_X(a, b) ((b) - ((p_xrect_t)((p_widget_base_t)(a)))->x)
#define P_CDE2OPPOSITE_Y(a, b) ((b) - ((p_xrect_t)((p_widget_base_t)(a)))->y)

/*相对坐标转换为绝对坐标*/
#define _OPPOSITE2CDE_X(a, b) (((p_xrect_t)(((p_widget_base_t)(a))->parentHWIN))->x - (b))
#define _OPPOSITE2CDE_Y(a, b) (((p_xrect_t)(((p_widget_base_t)(a))->parentHWIN))->y - (b))

/*得到控件的坐标与大小*/
#define WIDGE_X(a) (((p_xrect_t)(a))->x)
#define WIDGE_Y(a) (((p_xrect_t)(a))->y)
#define WIDGE_W(a) (((p_xrect_t)(a))->w)
#define WIDGE_H(a) (((p_xrect_t)(a))->h)

/*获得控件的父控件*/
#define WIDGE_PARENT(a) ((p_widget_base_t)(((p_widget_base_t)(a))->parentHWIN))

p_xdesktop_t GUIInit(void);
void GUIUpdate(p_widget_base_t hWidgeBase, p_xrect_t hXRect);
void GUIExec(void);
void GUIEvent(void);
void GUIInvailRect(xrect_t *rect);
uint32_t GUIGetTick(void);
int8_t WinListAdd(p_widget_base_t hWidgeBase);
p_widget_base_t WinGetTop(void);
void WinMoveTop(widget_base_t *hObject);

BOOL IsGUINeedCut(p_widget_base_t hWidgeBase);
void SetUpdateArea(p_widget_base_t hWidgeBase, p_xrect_t hXRect);

uint8_t DrawSetArea(p_widget_base_t hWidgeBase); /*设置绘图区域*/
void DrawResetArea(p_widget_base_t hWidgeBase);	 /*取消设置*/
#if USE_FRAMEBUFFER_MODE
#if USE_MUTL_LAYER
void *GUIGetBuffer(void);
void GUISwLayer(void);
#endif
#endif
