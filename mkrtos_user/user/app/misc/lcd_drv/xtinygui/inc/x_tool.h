/**
 * @file x_tool.h
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "msg.h"
#include "paint.h"
#include <string.h>
#include <assert.h>

#define _GET_BIT(a, b) (((a) >> (b)) & 0x1) //!< 获取位
#define _SET_BIT(a, b) (a) |= (1 << (b))    //!< 设置位
#define _CLR_BIT(a, b) (a) &= (~(1 << (b))) //!< 清除位

#define MAX(a, b) ((a) > (b) ? (a) : (b))   //!< 最大值
#define MIN(a, b) ((a) < (b) ? (a) : (b))   //!< 最小值

/**
 * @brief 判断两矩形是否相交
 *
 * @param x1
 * @param y1
 * @param w1
 * @param h1
 * @param x2
 * @param y2
 * @param w2
 * @param h2
 * @return int8_t
 */
static inline int8_t isCollsionWithRect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    // 矩形A位于矩形B的右侧
    if (x1 >= x2 && x1 >= x2 + w2)
    {
        return FALSE;
        // 矩形A位于矩形B的左侧
    }
    else if (x1 <= x2 && x1 + w1 <= x2)
    {
        return FALSE;
        // 矩形A位于矩形B的下侧
    }
    else if (y1 >= y2 && y1 >= y2 + h2)
    {
        return FALSE;
        // 矩形A位于矩形B的上侧
    }
    else if (y1 <= y2 && y1 + h1 <= y2)
    {
        return FALSE;
    }
    // 不相交都不满足，那就是相交了
    return TRUE;
}

/**
 * @brief 获得两个矩形的重叠部分
 *
 * @param r1
 * @param r2
 * @param res
 * @return BOOL
 */
static inline BOOL GetOverLapRect(p_xrect_t r1, p_xrect_t r2, p_xrect_t res)
{
    assert(r1);
    assert(r2);
    assert(res);

    res->x = MAX(r1->x, r2->x);
    res->y = MAX(r1->y, r2->y);
    res->w = MIN(r1->x + r1->w, r2->x + r2->w);
    res->h = MIN(r1->y + r1->h, r2->y + r2->h);
    if ((res->x > res->w) || (res->y > res->h))
    {
        res->x = r1->x;
        res->y = r1->y;
        res->w = 0;
        res->h = 0;
        return FALSE;
    }
    else
    {
        res->w -= res->x;
        res->h -= res->y;
    }
    return TRUE;
}
// 某个范围内内的垂直线是否与下方的横线相交
#define _isRangeLinesCollsionLineD(x1, y1, w1, x2, y2, w2) ((y2 > y1) && (((x2 >= x1 && x2 < (x1 + w1))) || ((x2 + w2) >= x1) && ((x2 + w1) < (x1 + w1)))) // isRangeLinesCollsionLineD(x1,y1,w1,x2,y2,w2)
// 横线是否与矩形相交
#define _isLineHCollsionRect(x0, y, w, x1, y1, w1, h1) (!(!((y) >= (y1) && (y) < ((y1) + (h1))) || (((x0) + (w)-1) < (x1)) || ((x0) > ((x1) + (w1)-1)))) // isLineHCollsionRect( x0,  y,  w,  x1,  y1,  w1,  h1)
// 检查两个矩形是否碰撞
#define _IsDrawCheckArea(x1, y1, w1, h1, x2, y2, w2, h2) isCollsionWithRect(x1, y1, w1, h1, x2, y2, w2, h2)
// 点是否在矩形内
#define _IsDrawCheckPoint(x, y, x0, y0, w0, h0) (((x) >= (x0)) && ((x) < ((x0) + (w0))) && ((y) >= (y0)) && ((y) < ((y0) + (h0)))) // isInside( x,  y, x0, y0, w0, h0)
#define _IsDrawCheckPointR(xp, yp, a) _IsDrawCheckPoint((xp), (yp), ((a)->x), ((a)->y), ((a)->w), ((a)->h))

/*获取gbk或者gb2312的长度*/
uint32_t GBK_Strlen(const char *str);
uint32_t UNI_Strlen(uint16_t *str);
const char *GBK_CharGet(const char *str, uint16_t index);

// 矩形复制函数
#define XRECT_COPY(a, b) memcpy((a), (b), sizeof(xrect_t))
