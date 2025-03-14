/**
 * @file paint_cut.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "paint_cut.h"
#include "x_malloc.h"
#include "gui.h"
#include "list.h"
#include <string.h>
extern void fill_rect(int x, int y, int w, int h, int color);

xrect_t xResRect;
/*和那个矩形产生了碰撞*/
p_xrect_t lastRightRect = NULL;
list_t rectCutList = {0};		 /*剪裁矩形*/
static p_list_t rectItem = NULL; /*临时用*/
static xrect_t bgRect;			 /*被剪裁的矩形*/
static xpoint_t startPoint;		 /*初始扫描起点*/
static BOOL isEnd = 0;			 /*剪裁是否结束*/

BOOL RectCutIsEnd(void)
{
	return isEnd;
}

/**
 * @brief 添加剪裁矩形，每次添加到尾部
 *
 * @param addRectList
 * @return p_list_t
 */
p_list_t RectCutAddRectList(p_list_t addRectList)
{
	p_list_t tempRectCutList;
	tempRectCutList = &rectCutList;

	while (tempRectCutList->next)
	{
		tempRectCutList = tempRectCutList->next;
	}
	tempRectCutList->next = addRectList;

	return tempRectCutList;
}
/**
 * @brief 指定的List处断开
 *
 * @param splitRectList
 */
void RectCutSplitRectList(p_list_t splitRectList)
{
	splitRectList->next = NULL;
}

/**
 * @brief 初始化剪裁
 *
 * @return int8_t
 */
int8_t RectCutInit(void)
{

	return TRUE;
}

/**
 * @brief 开始剪裁
 *
 * @param hXRECT
 * @return int8_t
 */
void *RectCutStart(p_xrect_t hXRECT)
{
	rectItem = rectCutList.next;
	bgRect = *hXRECT;
	startPoint.x = bgRect.x;
	startPoint.y = bgRect.y;

	isEnd = FALSE;

	return NULL;
}

static p_xrect_t RectCutGetNext(void)
{
	p_list_t tempList = rectItem;

	if (rectItem != NULL)
	{
		rectItem = rectItem->next;

		return &container_of(tempList, widget_base_t, widget_node)->rect;
	}
	return NULL;
}
/*矩形剪裁结束*/
void RectCutEnd(void)
{
	rectItem = NULL;
	// if (splitItem == NULL) {return;}
	// splitItem->next = NULL;
}

/*得到下一个剪裁矩形*/
p_xrect_t RectCutFind(void)
{
	p_xrect_t hRect;
	xpoint_t pointEnd;
	//	p_list_t tempRectCutlist;

	int16_t temp_w = 0;
	int16_t val = bgRect.y + bgRect.h;
	// int val1 = bgRect.y + bgRect.h;

	pointEnd.x = bgRect.w + bgRect.x;
	pointEnd.y = bgRect.y + bgRect.h;

	lastRightRect = NULL;

	rectItem = rectCutList.next;
	while ((hRect = RectCutGetNext()) != NULL)
	{
		if (!_GetVisable(hRect) || _GET_IS_DPY(hRect))
		{
			continue;
		}

		/*************查找参考线下面离得最近的一条边*************/
		/*优化，不应该只找离得最近的一条边，应该是找右边最近的矩形的上下最近的一条边*/
		if (hRect->y > startPoint.y)
		{ /*上边*/
			/*在被剪裁矩形的纵向限定范围内*/
			if (hRect->y > bgRect.y && hRect->y < bgRect.y + bgRect.h)
			{
				if (hRect->y - startPoint.y < val)
				{ /*找最近的一条*/
					val = hRect->y - startPoint.y;
					pointEnd.y = hRect->y;
					// fill_rect(startPoint.x, startPoint.y, 5, 5, rand() % 65535);
				}
			}
		}
		else if (hRect->y + hRect->h > startPoint.y)
		{ /*下边*/
			/*在被剪裁矩形的纵向限定范围内*/
			if (hRect->y + hRect->h > bgRect.y && hRect->y + hRect->h < bgRect.y + bgRect.h)
			{
				if ((hRect->y + hRect->h - startPoint.y) < val)
				{ /*找最近的一条*/
					val = (hRect->y + hRect->h - startPoint.y);

					pointEnd.y = hRect->y + hRect->h;
					// fill_rect(startPoint.x, startPoint.y, 5, 5, rand() % 65535);
				}
			}

			/************找右边离起点x最近的线************/
			if (startPoint.x < hRect->x)
			{ /*在右边的线*/
				if (pointEnd.x > hRect->x)
				{ /*离得最近的一条*/
					pointEnd.x = hRect->x;
					lastRightRect = hRect;
					temp_w = hRect->x + hRect->w;
				}
			}
			if (lastRightRect != NULL)
			{
				if (_IsDrawCheckArea(lastRightRect->x, lastRightRect->y, lastRightRect->w, lastRightRect->h,
									 hRect->x, hRect->y, hRect->w, hRect->h))
				{
					if (hRect->x + hRect->w > lastRightRect->x + lastRightRect->w)
					{
						temp_w = hRect->x + hRect->w;
					}
					else
					{
						temp_w = lastRightRect->x + lastRightRect->w;
					}
					lastRightRect = hRect;
				}
			}

			/********************************************/
		}
		/*****************************************************/

		/*找起点*/
		if (_IsDrawCheckPoint(startPoint.x, startPoint.y,
							  hRect->x, hRect->y, hRect->w, hRect->h))
		{

			/*起点碰撞，向右移动*/
			startPoint.x = hRect->x + hRect->w;
			if (startPoint.x >= bgRect.w + bgRect.x)
			{
				/*起点大于等于终点并且终点y大于等于剪裁矩形则退出*/
				if (startPoint.x >= pointEnd.x && pointEnd.y >= bgRect.y + bgRect.h)
				{

					p_list_t tempItem;
					int tempPointEndY;
					tempPointEndY = pointEnd.y;
					tempItem = rectItem;
					/*pointEnd.y可能还没有达到与参考位置最近，故继续遍历*/
					while (hRect = RectCutGetNext())
					{
						if (!_GetVisable(hRect) || _GET_IS_DPY(hRect))
						{
							continue;
						}
						if (hRect->y > startPoint.y)
						{ /*上边*/
							/*在被剪裁矩形的纵向限定范围内*/
							if (hRect->y > bgRect.y && hRect->y < bgRect.y + bgRect.h)
							{
								if (hRect->y - startPoint.y < val)
								{ /*找最近的一条*/
									val = hRect->y - startPoint.y;
									pointEnd.y = hRect->y;
								}
							}
						}
						else if (hRect->y + hRect->h > startPoint.y)
						{ /*下边*/
							/*在被剪裁矩形的纵向限定范围内*/
							if (hRect->y + hRect->h > bgRect.y && hRect->y + hRect->h < bgRect.y + bgRect.h)
							{
								if ((hRect->y + hRect->h - startPoint.y) < val)
								{ /*找最近的一条*/
									val = (hRect->y + hRect->h - startPoint.y);
									pointEnd.y = hRect->y + hRect->h;
								}
							}
						}
					}
					rectItem = tempItem;
					/*如果没有变化，则表示找不到了，退出*/
					if (tempPointEndY == pointEnd.y)
					{
						return NULL;
					}
				}
				/*右移超出边界，x回到起点*/
				startPoint.x = bgRect.x;
				/*Y回到找到的最近的一条边*/
				p_list_t tempItem;
				int tempPointEndY;
				tempPointEndY = pointEnd.y;
				tempItem = rectItem;
				/*pointEnd.y可能还没有达到与参考位置最近，故继续遍历*/
				while (hRect = RectCutGetNext())
				{
					if (!_GetVisable(hRect) || _GET_IS_DPY(hRect))
					{
						continue;
					}
					if (hRect->y > startPoint.y)
					{ /*上边*/
						/*在被剪裁矩形的纵向限定范围内*/
						if (hRect->y > bgRect.y && hRect->y < bgRect.y + bgRect.h)
						{
							if (hRect->y - startPoint.y < val)
							{ /*找最近的一条*/
								val = hRect->y - startPoint.y;
								pointEnd.y = hRect->y;
							}
						}
					}
					else if (hRect->y + hRect->h > startPoint.y)
					{ /*下边*/
						/*在被剪裁矩形的纵向限定范围内*/
						if (hRect->y + hRect->h > bgRect.y && hRect->y + hRect->h < bgRect.y + bgRect.h)
						{
							if ((hRect->y + hRect->h - startPoint.y) < val)
							{ /*找最近的一条*/
								val = (hRect->y + hRect->h - startPoint.y);
								pointEnd.y = hRect->y + hRect->h;
							}
						}
					}
				}
				// rectItem = tempItem;
				startPoint.y = pointEnd.y;

				/*更新完成后，剪裁的起点发生了变化，重新退回剪裁域*/
				rectItem = rectCutList.next;

				/*复位到原始起点*/
				pointEnd.x = bgRect.w + bgRect.x;
				pointEnd.y = bgRect.y + bgRect.h;

				temp_w = 0;

				val = bgRect.y + bgRect.h;

				lastRightRect = NULL;
			}
			// fill_rect(startPoint.x, startPoint.y, 5, 5, rand() % 65535);
			/*退回，因为可能之前的矩形与现在的碰撞，但是之前的起点并未发现*/
			rectItem = rectCutList.next;
		}
	}

	xResRect.x = startPoint.x;
	xResRect.y = startPoint.y;
	xResRect.w = pointEnd.x - startPoint.x;
	xResRect.h = pointEnd.y - startPoint.y;

	// lastY = pointEnd.y;
	if (pointEnd.x >= bgRect.x + bgRect.w || temp_w >= bgRect.x + bgRect.w)
	{
		startPoint.y = pointEnd.y;
		startPoint.x = bgRect.x;

		// fill_rect(startPoint.x, startPoint.y, 5, 5, rand() % 65535);

		if (pointEnd.y >= bgRect.h + bgRect.y)
		{
			isEnd = TRUE;
		}
	}
	else
	{
		startPoint.x = pointEnd.x;
		if (startPoint.x >= bgRect.x + bgRect.w)
		{
			startPoint.y = pointEnd.y;
			startPoint.x = bgRect.x;
		}

		// fill_rect(startPoint.x, startPoint.y, 5, 5, rand() % 65535);
	}
	// fill_rect(startPoint.x, startPoint.y, 5, 5, rand() % 65535);
	return &xResRect;
}