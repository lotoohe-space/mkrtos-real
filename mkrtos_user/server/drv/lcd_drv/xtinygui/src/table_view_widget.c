/**
 * @file table_view_widget.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "table_view_widget.h"
#include "x_malloc.h"
#include "xwindows.h"
PUBLIC p_table_widget_t TABLE_WIDGE_MARK_HEAD(Create)(int16_t x, int16_t y, uint16_t w, uint16_t h) {
	p_table_widget_t hObject;

	hObject = (p_table_widget_t)XMalloc(sizeof(table_widget_t));
	if (hObject == NULL) {
		return NULL;
	}
	GROUP_MARK_HEAD(Init)(&hObject->groupWidge, x, y, w, h);

	return hObject;
}
/*添加一个控件*/
PUBLIC uint8_t TABLE_WIDGE_MARK_HEAD(Add)(p_table_widget_t hBaseWidge, p_widget_base_t widge) {
	uint16_t widgeLength;
	if (hBaseWidge == NULL || widge == NULL) { return FALSE; }

	if (GROUP_MARK_HEAD(Add)((p_group_widget_t)hBaseWidge, widge) == FALSE) {
		return FALSE;
	}
	widgeLength = ((p_group_widget_t)hBaseWidge)->widgeLength - 1;

	/*刷新*/
	WindowsInvaildRect((p_widget_base_t)hBaseWidge, NULL);
	return TRUE;
}
