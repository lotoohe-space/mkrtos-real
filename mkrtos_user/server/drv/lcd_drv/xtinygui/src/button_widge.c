/**
 * @file button_widge.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "paint.h"
#include "paint_cut.h"
#include "button_widge.h"
#include "x_malloc.h"

/**
 * @brief 创建一个button
 *
 * @param text
 * @param x
 * @param y
 * @param w
 * @param h
 * @return PUBLIC
 */
PUBLIC p_xbutton_t ButtonWidgetCreate(const char *text, int16_t x, int16_t y, int16_t w, int16_t h)
{
	p_xbutton_t hXButton = XMalloc(sizeof(xbutton_t));

	if (hXButton == NULL)
	{
		return NULL;
	}
	TEXT_MARK_HEAD(Init)
	((p_text_widget_t)hXButton, text, x, y, w, h);

	_PToHWidgeBaseType(hXButton)->paintFun = (void *)ButtonWidgetPaint;
	_PToHWidgeBaseType(hXButton)->moveToFun = (void *)ButtonWidgetMoveTo;
	_PToHWidgeBaseType(hXButton)->widgeCallBackFun = (void *)ButtonWidgetCallBack;

	//_PToHTextWidgeType(hXButton)->hFont = (p_font_t)& fontGB231216_16;
	hXButton->flag = 0;

	hXButton->downBitmap = NULL;
	hXButton->upBitmap = NULL;

	hXButton->downColor = _DefaultButtonDownColor;
	hXButton->upColor = _DefaultButtonUpColor;

	_PToHTextWidgeType(hXButton)->textMode = TEXT_CENTER_ALIGN;

	return hXButton;
}
/**
 * @brief 设置按下的图片
 *
 * @param hObject
 * @param hBitmap
 * @return PUBLIC
 */
PUBLIC void ButtonWidgetSetDownBitmap(p_xbutton_t hObject, p_xbitmap_t hBitmap)
{
	if (hObject == NULL)
	{
		return;
	}
	hObject->downBitmap = hBitmap;
}
/**
 * @brief 设置弹起的图片
 *
 * @param hObject
 * @param hBitmap
 * @return PUBLIC
 */
PUBLIC void ButtonWidgetSetUpBitmap(p_xbutton_t hObject, p_xbitmap_t hBitmap)
{
	if (hObject == NULL)
	{
		return;
	}
	hObject->upBitmap = hBitmap;
}
/**
 * @brief 设置Buttton文字
 *
 * @param hObject
 * @param text
 * @return PUBLIC
 */
PUBLIC void ButtonWidgetSetText(p_xbutton_t hObject, const char *text)
{
	if (hObject == NULL)
	{
		return;
	}
	TextWidgetSetText((p_text_widget_t)hObject, text);
}
/**
 * @brief 设置点击回调
 *
 * @param hObject
 * @param arg
 * @param viewClickCallBack
 * @return PUBLIC
 */
PUBLIC void ButtonWidgetSetClickBack(p_xbutton_t hObject, void *arg, ViewClickCallBack viewClickCallBack)
{
	WidgetSetClickBack((p_widget_base_t)hObject, arg, viewClickCallBack);
}
/**
 * @brief 移动控件到指定位置
 *
 * @param hObject
 * @param x
 * @param y
 * @return PUBLIC
 */
PUBLIC void ButtonWidgetMoveTo(p_xbutton_t hObject, int16_t x, int16_t y)
{
	if (!hObject)
	{
		return;
	}
	WidgetMoveTo((p_widget_base_t)hObject, x, y);
}
/**
 * @brief 绘制窗口
 *
 * @param hObject
 * @return PRIVATE
 */
PRIVATE void ButtonWidgetPaint(void *hObject)
{
	p_xbutton_t hXButton;

	//	xrect_t xRECT;
	hXButton = hObject;
	if (!hObject)
	{
		return;
	}
	if (!DrawSetArea((p_widget_base_t)hXButton))
	{
		return;
	}
	// if (!IsGUINeedCut(hXButton)) { return ; }

	if (!_GetBtnStatus(hXButton))
	{
		/*按键松开*/
		xpoint_t startPoint;

		_PToHWidgeBaseType(hXButton)->pencil.DrawBkColor = _DefaultButtonUpColor;
		_PToHWidgeBaseType(hXButton)->pencil.DrawFrColor = _DefaultButtonFontColor;
		if (!_GetBtnMode(hXButton))
		{
			/*文本模式*/
			TextWidgetGetPOIByTextMode((p_text_widget_t)hXButton, &startPoint);
			/*根据模式确定起点*/
			DrawCutString(hXButton,
						  _PToHTextWidgeType(hXButton)->hFont,
						  &(_PToHWidgeBaseType(hXButton)->rect),
						  &startPoint,
						  _PToHTextWidgeType(hXButton)->text);
		}
		else
		{
			/*图片模式*/
			if (hXButton->upBitmap == NULL)
			{
				_PToHWidgeBaseType(hXButton)->pencil.DrawColor = _DefaultButtonUpColor;
				DrawCutRect(hXButton, _PToHXRECTType(hXButton));
			}
			else
			{
				DrawCutBitmap(hXButton, _PToHXRECTType(hXButton), hXButton->upBitmap);
			}
		}
	}
	else
	{
		/*按键按下*/
		xpoint_t startPoint;

		_PToHWidgeBaseType(hXButton)->pencil.DrawBkColor = _DefaultButtonDownColor;
		_PToHWidgeBaseType(hXButton)->pencil.DrawFrColor = _DefaultButtonFontColor;
		if (!_GetBtnMode(hXButton))
		{
			/*文本模式*/
			/*根据模式确定起点*/
			TextWidgetGetPOIByTextMode((p_text_widget_t)hXButton, &startPoint);
			DrawCutString(hXButton,
						  _PToHTextWidgeType(hXButton)->hFont,
						  &(_PToHWidgeBaseType(hXButton)->rect),
						  &startPoint,
						  _PToHTextWidgeType(hXButton)->text);
		}
		else
		{
			/*图片模式*/
			if (hXButton->upBitmap == NULL)
			{
				_PToHWidgeBaseType(hXButton)->pencil.DrawColor = _DefaultButtonDownColor;
				DrawCutRect(hXButton, _PToHXRECTType(hXButton));
			}
			else
			{
				DrawCutBitmap(hXButton, _PToHXRECTType(hXButton), hXButton->downBitmap);
			}
		}
	}
	/*恢复绘图区域*/
	DrawResetArea((p_widget_base_t)hXButton);
}
/**
 * @brief 事件回调
 *
 * @param hObject
 * @param hMsg
 * @return PRIVATE
 */
PRIVATE int8_t ButtonWidgetCallBack(void *hObject, p_msg_t hMsg)
{
	int8_t ret;
	p_xbutton_t hXButton = hObject;

	if (!hXButton || !hMsg)
	{
		return RES_ASSERT_ERR;
	}
	if (!_GetVisable(hXButton))
	{
		return RES_ASSERT_ERR;
	}

	ret = WidgetCallBack(hXButton, hMsg);
	if (ret == RES_OK)
	{
		if (_GetBtnStatus(hXButton))
		{
			_PToHWidgeBaseType(hXButton)->pencil.DrawColor = hXButton->downColor;
		}
		else
		{
			_PToHWidgeBaseType(hXButton)->pencil.DrawColor = hXButton->upColor;
		}
	}
	return ret;
}
