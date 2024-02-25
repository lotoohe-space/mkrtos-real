
#include <printf.h>
#include <assert.h>
#include <u_drv.h>
#include <u_prot.h>
#include <u_mm.h>
#include <u_sys.h>
#include <u_thread.h>
#include <pthread.h>
#include <u_rpc_svr.h>
#include <u_hd_man.h>
#include <net_drv_svr.h>
#include <u_share_mem.h>
#include <rpc_prot.h>
#include <ns_cli.h>
#include <printf.h>
#include "SWM341.h"
#include "OTM8009A.h"
#include <u_sleep.h>
#include <u_intr.h>
#include <string.h>
#include "dev_touch.h"

#include "msg.h"
#include "gui.h"
#include "x_widget.h"
#include "xwindows.h"
#include "button_widge.h"
#include "text_widge.h"
#include "bitmap.h"
#include "bitmap_widge.h"
#include "progress_widge.h"
#include "check_box_widge.h"
#include "switch_button_widge.h"
#include "radio_button_widge.h"
#include "list_widge.h"
#include "slide_widge.h"
#include "nes_main.h"
#include "snake.h"
#include "snake_gui.h"

void RGBLCDInit(void);
uint16_t *LCD_Buffer;
uint16_t *LCD_Buffer1;
uint16_t *LCD_Buffer2;

void lcd_draw_pixel(int x, int y, uint16_t color)
{
    LCD_Buffer[x + y * LCD_SCREEN_W] = color;
}

DMA2D_LayerSetting fgLayer, bgLayer, outLayer;

void DMA2D_GUIDrawRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uintColor color)
{
    assert(x + w <= LCD_SCREEN_W);
    assert(y + h <= LCD_SCREEN_H);
    outLayer.Address = (uint32_t)&LCD_Buffer[x + y * LCD_SCREEN_W];
    outLayer.LineCount = h;
    outLayer.LinePixel = w;
    outLayer.LineOffset = LCD_SCREEN_W - (w);
    outLayer.ColorMode = DMA2D_FMT_RGB565;
    DMA2D_PixelFill(&outLayer, color);
    while (DMA2D_IsBusy())
        __NOP();
}
void DMA2D_GUIDrawRect2Mem(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t wh,
                           uintColor color, uint16_t *data)
{
    assert(x + w <= LCD_SCREEN_W);
    assert(y + h <= LCD_SCREEN_H);
    outLayer.Address = (uint32_t)&data[x + y * wh];
    outLayer.LineCount = h;
    outLayer.LinePixel = w;
    outLayer.LineOffset = wh - (w);
    outLayer.ColorMode = DMA2D_FMT_RGB565;
    DMA2D_PixelFill(&outLayer, color);
    while (DMA2D_IsBusy())
        __NOP();
}
void DMA2D_GUIDrawBitmap(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t *bitmap)
{
    assert(x + w <= LCD_SCREEN_W);
    assert(y + h <= LCD_SCREEN_H);
    fgLayer.Address = (uint32_t)bitmap;
    fgLayer.LineOffset = 0;
    fgLayer.ColorMode = DMA2D_FMT_RGB565;

    outLayer.Address = (uint32_t)(&LCD_Buffer[x + y * LCD_SCREEN_W]);
    outLayer.LineCount = h;
    outLayer.LinePixel = w;
    outLayer.LineOffset = LCD_SCREEN_W - (w);
    DMA2D_PixelMove(&fgLayer, &outLayer);
    while (DMA2D_IsBusy())
        __NOP();
}
void DMA2D_GUICopy(xrect_t *src_rect, int src_wh, uint8_t *bitmap,
                   xrect_t *dst_rest, int dst_wh, uint8_t *dst_bitmap)
{
    assert(dst_rest->x + dst_rest->w <= LCD_SCREEN_W);
    assert(dst_rest->y + dst_rest->h <= LCD_SCREEN_H);
    fgLayer.Address = (uint32_t)(&bitmap[(src_rect->x + src_rect->y * src_wh) * 2]);
    fgLayer.LineCount = src_rect->h;
    fgLayer.LinePixel = src_rect->w;
    fgLayer.LineOffset = src_wh - (src_rect->w);
    fgLayer.ColorMode = DMA2D_FMT_RGB565;

    outLayer.Address = (uint32_t)(&dst_bitmap[(dst_rest->x + dst_rest->y * dst_wh) * 2]);
    outLayer.LineCount = dst_rest->h;
    outLayer.LinePixel = dst_rest->w;
    outLayer.LineOffset = dst_wh - (dst_rest->w);
    DMA2D_PixelMove(&fgLayer, &outLayer);
    while (DMA2D_IsBusy())
        __NOP();
}

p_win_t hWin;
p_win_t hWin2;
p_win_t hWin4;

p_xbutton_t hXButton;
p_text_widget_t hTEXT_WIDGE;
p_progress_widget_t testProgressWidge;
p_text_widget_t fpsTextWidge;
p_checkbox_widget_t testCheckBox;
p_bitmap_widget_t hBITMAP_WIDGE;

p_bitmap_widget_t hBITMAP_WIDGE1;
xbitmap_t xGirlBitmap;
uint16_t *nesImg;
void checkCallBack(void *Object, void *arg, uint8_t status)
{
    // if (status)
    // {
    // 	WidgetSetVisable((p_widget_base_t)hBITMAP_WIDGE1, TRUE);
    // }
    // else
    // {
    // 	WidgetSetVisable((p_widget_base_t)hBITMAP_WIDGE1, FALSE);
    // }
}
uint8_t WinProcessFunction(void *Object, void *arg, p_msg_t hMsg)
{
    switch (hMsg->msgType)
    {
    case MSG_WIN:
        if (hMsg->msgID == MSG_WIN_CREATE)
        {
#if 1

            /*窗口中添加一个文本框*/
            hTEXT_WIDGE = TextWidgetCreate(L"AB", 5, 5, 150, 100);
            hTEXT_WIDGE->hFont = &fontASCII100_100;
            WindowsAdd(Object, hTEXT_WIDGE);

            /*添加一个进度条*/
            testProgressWidge = PROGRESS_MARK_HEAD(Create)(5, 115, 100, 20, 100);
            WindowsAdd(Object, testProgressWidge);

            /*添加一个checkbox*/
            testCheckBox = CHECKBOX_MARK_HEAD(Create)(5, 140, 140, 18, "CheckBox");
            CHECKBOX_MARK_HEAD(SetClickBack)
            (testCheckBox, testCheckBox, checkCallBack);
            WindowsAdd(Object, testCheckBox);

            /*窗口中添加一个文本框*/

            // WindowsAdd(Object, SLIDE_MARK_HEAD(Create)(5, 180, 100, 20));
            p_xlist_widget_t hListWidge = ListWidgetCreate(160, 5, 240, 240);
            _OpenListAnimation(hListWidge);
            WidgetSetColor(&hListWidge->groupWidge.widgeBase, ARGB565_GEN(128, 0xff, 0x00, 0xff));

            WindowsAdd(Object, hListWidge);

            p_bitmap_widget_t *dy_bitmap;
            // InitBitmap(&pngBitmap, pngImg, 160, 120, 32);
            // BitmapSetFlag(&pngBitmap, BITMAP_DRAW_ARGB8888);
            dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
            dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
            dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
            dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
            dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
            dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
            dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
            dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
            dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
            dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
            dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
            dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
#endif
            /*内部添加一个窗口*/
            // hWin2 = WindowsCreate(L"Windows", 100, 100, 100, 100);
            // _SET_IS_DPY(hWin2);
            // WindowsSetColor(hWin2, ARGB565_GEN(128, 0, 0, 0xff));
            // WindowsAdd(Object, hWin2);
        }
        else if (hMsg->msgID == MSG_WIN_PAINT)
        {
#if 1
            ((p_widget_base_t)(Object))->pencil.DrawColor = RGB565_RED;
            ((p_widget_base_t)(Object))->pencil.LineWidth = 3;
            ((p_widget_base_t)(Object))->pencil.lineType = 0xf;
            DrawLine(Object, &(xpoint_t){.x = 250, .y = 400}, &(xpoint_t){.x = 400, .y = 400});
            DrawCircle(Object, &(xpoint_t){.x = 300, .y = 400}, 50);

            ((p_widget_base_t)(Object))->pencil.LineWidth = 1;
            ((p_widget_base_t)(Object))->pencil.brush_type = NORMAL_TYPE;
            ((p_widget_base_t)(Object))->pencil.DrawColor = RGB565_BLUE;
            DrawArc(Object, &(xpoint_t){.x = 350, .y = 400}, 0, 360, 47);

            ((p_widget_base_t)(Object))->pencil.DrawColor = RGB565_GREEN;
            DrawFillCircle(Object,
                           &(xpoint_t){.x = 200, .y = 400}, 50);

            DrawPie(Object,
                    &(xpoint_t){.x = 100, .y = 400},
                    50, // 扇形半径
                    30.0 * 3.14 / 180,
                    190.0 * 3.14 / 180 // 扇形弧度
            );
#endif
        }
        break;
    }
    return TRUE;
}

uint8_t WinProcessNESFunction(void *Object, void *arg, p_msg_t hMsg)
{
    switch (hMsg->msgType)
    {
    case MSG_WIN:
        if (hMsg->msgID == MSG_WIN_CREATE)
        {
            /*用来显示nes的图片*/
            InitBitmap(&xGirlBitmap, (uint8_t *)nesImg, 256, 240, 16);
            // xGirlBitmap.flag = 0x1;
            // xGirlBitmap.alpha = 100;
            hBITMAP_WIDGE1 = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
            // _CLR_IS_DPY(hBITMAP_WIDGE1);
            WindowsAdd(Object, hBITMAP_WIDGE1);
        }
        else if (hMsg->msgID == MSG_WIN_PAINT)
        {
            /*重绘*/
        }
        break;
    case MSG_KEY:
        switch (hMsg->msgID)
        {
        case MSG_KEY_UP:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(0, 4, 1);
            }
            else
            {
                NES_SetJoyPadValue(0, 4, 0);
            }
            break;
        case MSG_KEY_DOWN:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(0, 5, 1);
            }
            else
            {
                NES_SetJoyPadValue(0, 5, 0);
            }
            break;
        case MSG_KEY_LEFT:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(0, 6, 1);
            }
            else
            {
                NES_SetJoyPadValue(0, 6, 0);
            }
            break;
        case MSG_KEY_RIGHT:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(0, 7, 1);
            }
            else
            {
                NES_SetJoyPadValue(0, 7, 0);
            }
            break;
        case MSG_KEY_0:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(0, 3, 1);
            }
            else
            {
                NES_SetJoyPadValue(0, 3, 0);
            }
            break;
        case MSG_KEY_1:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(0, 2, 1);
            }
            else
            {
                NES_SetJoyPadValue(0, 2, 0);
            }
            break;
        case MSG_KEY_N:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(0, 0, 1);
            }
            else
            {
                NES_SetJoyPadValue(0, 0, 0);
            }
            break;
        case MSG_KEY_M:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(0, 1, 1);
            }
            else
            {
                NES_SetJoyPadValue(0, 1, 0);
            }
            break;

        case MSG_KEY_W:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(1, 4, 1);
            }
            else
            {
                NES_SetJoyPadValue(1, 4, 0);
            }
            break;
        case MSG_KEY_S:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(1, 5, 1);
            }
            else
            {
                NES_SetJoyPadValue(1, 5, 0);
            }
            break;
        case MSG_KEY_A:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(1, 6, 1);
            }
            else
            {
                NES_SetJoyPadValue(1, 6, 0);
            }
            break;
        case MSG_KEY_D:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(1, 7, 1);
            }
            else
            {
                NES_SetJoyPadValue(1, 7, 0);
            }
            break;
        case MSG_KEY_V:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(1, 0, 1);
            }
            else
            {
                NES_SetJoyPadValue(1, 0, 0);
            }
            break;
        case MSG_KEY_B:
            if (hMsg->msgVal.status)
            {
                NES_SetJoyPadValue(1, 1, 1);
            }
            else
            {
                NES_SetJoyPadValue(1, 1, 0);
            }
            break;
        }
        break;
    }
    return TRUE;
}
widget_base_t *create_page(void)
{
    p_group_widget_t group_widget = GroupWidgeCreate(0, 0, LCD_SCREEN_W, LCD_SCREEN_H);

    WidgetSetColor(&group_widget->widgeBase, ARGB565_GEN(128, rand() % 256, 0xaa, 0x00));
    fpsTextWidge = TextWidgetCreate("0fps", 0, 0, 100, 16);
    fpsTextWidge->hFont = &fontASCII16_16;
    GroupWidgeAdd(group_widget, fpsTextWidge);

    /*窗口中添加一个文本框*/
    hTEXT_WIDGE = TextWidgetCreate(L"AB", 5, 200, 150, 100);
    hTEXT_WIDGE->hFont = &fontASCII100_100;
    GroupWidgeAdd(group_widget, hTEXT_WIDGE);

    /*添加一个进度条*/
    testProgressWidge = PROGRESS_MARK_HEAD(Create)(5, 115, 100, 20, 100);
    GroupWidgeAdd(group_widget, testProgressWidge);

    /*添加一个checkbox*/
    testCheckBox = CHECKBOX_MARK_HEAD(Create)(5, 140, 140, 18, "CheckBox");
    CHECKBOX_MARK_HEAD(SetClickBack)
    (testCheckBox, testCheckBox, checkCallBack);
    GroupWidgeAdd(group_widget, testCheckBox);

    /*窗口中添加一个文本框*/

    // WindowsAdd(Object, SLIDE_MARK_HEAD(Create)(5, 180, 100, 20));
    p_xlist_widget_t hListWidge = ListWidgetCreate(160, 5, 240, 240);
    _OpenListAnimation(hListWidge);
    WidgetSetColor(&hListWidge->groupWidge.widgeBase, ARGB565_GEN(128, 0xff, 0x00, 0xff));

    GroupWidgeAdd(group_widget, hListWidge);

    p_bitmap_widget_t *dy_bitmap;
    // InitBitmap(&pngBitmap, pngImg, 160, 120, 32);
    // BitmapSetFlag(&pngBitmap, BITMAP_DRAW_ARGB8888);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);

    InitBitmap(&xGirlBitmap, (uint8_t *)nesImg, 256, 240, 16);
    hBITMAP_WIDGE1 = BitmapWidgetCreate(20, 500, 256, 240, &xGirlBitmap);
    GroupWidgeAdd(group_widget, hBITMAP_WIDGE1);

    return group_widget;
}
void gui_init(void)
{
    GUIInit();
    snake_main();

    // p_xlist_widget_t bgListView = ListWidgetCreate(0, 0, LCD_SCREEN_W, LCD_SCREEN_H);
    // _OpenListAnimation(bgListView);
    // // _SetOTN_H(bgListView);
    // WidgetSetColor(&bgListView->groupWidge.widgeBase, ARGB565_GEN(128, 0x00, 0xaa, 0xff));
    // WinListAdd(bgListView);

    // p_group_widget_t group_widget = GroupWidgeCreate(0, 0, LCD_SCREEN_W, LCD_SCREEN_H);

    // WidgetSetColor(&group_widget->widgeBase, ARGB565_GEN(128, rand() % 256, 0xaa, 0x00));
    fpsTextWidge = TextWidgetCreate("0fps", 0, 0, 100, 16);
    fpsTextWidge->hFont = &fontASCII16_16;
    WinListAdd(fpsTextWidge);

    /*窗口中添加一个文本框*/
    hTEXT_WIDGE = TextWidgetCreate(L"AB", 5, 200, 150, 100);
    hTEXT_WIDGE->hFont = &fontASCII100_100;
    WinListAdd(hTEXT_WIDGE);

    /*添加一个进度条*/
    testProgressWidge = PROGRESS_MARK_HEAD(Create)(5, 115, 100, 20, 100);
    WinListAdd(testProgressWidge);

    /*添加一个checkbox*/
    testCheckBox = CHECKBOX_MARK_HEAD(Create)(5, 140, 140, 18, "CheckBox");
    CHECKBOX_MARK_HEAD(SetClickBack)
    (testCheckBox, testCheckBox, checkCallBack);
    WinListAdd(testCheckBox);

    /*窗口中添加一个文本框*/

    // WindowsAdd(Object, SLIDE_MARK_HEAD(Create)(5, 180, 100, 20));
    p_xlist_widget_t hListWidge = ListWidgetCreate(160, 5, 240, 500);
    _OpenListAnimation(hListWidge);
    WidgetSetColor(&hListWidge->groupWidge.widgeBase, ARGB565_GEN(128, 0xff, 0x00, 0xff));

    WinListAdd(hListWidge);

    p_bitmap_widget_t *dy_bitmap;
    // InitBitmap(&pngBitmap, pngImg, 160, 120, 32);
    // BitmapSetFlag(&pngBitmap, BITMAP_DRAW_ARGB8888);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);
    dy_bitmap = BitmapWidgetCreate(0, 0, 256, 240, &xGirlBitmap);
    ListWidgetAdd(hListWidge, (p_widget_base_t)dy_bitmap);

    InitBitmap(&xGirlBitmap, (uint8_t *)nesImg, 256, 240, 16);
    hBITMAP_WIDGE1 = BitmapWidgetCreate(20, 500, 256, 240, &xGirlBitmap);
    WinListAdd(hBITMAP_WIDGE1);

    // ListWidgetAdd(bgListView, create_page());
    // hWin = WindowsCreate("Windows1", 0, 0, LCD_SCREEN_W, 400);
    // WindowsSetProcessCallBack(hWin, WinProcessFunction);
    // // _SET_IS_DPY(hWin);
    // // WindowsSetColor(hWin, ARGB565_GEN(128, 0xff, 0x00, 0xff));
    // WinListAdd(&hWin->groupWidge.widgeBase);

    /*NES模拟器*/
    //     hWin4 = WindowsCreate("NES", 0, 0, 256, 256 + 40);
    //     WindowsSetProcessCallBack(hWin4, WinProcessNESFunction);
    //     // _SET_IS_DPY(hWin4);
    //     WinListAdd(&hWin4->groupWidge.widgeBase);

    snake_gui_init();
}
/*Return true is the touchpad is pressed*/
static bool touchpad_is_pressed(void)
{
    /*Your code comes here*/
    return (0 == (Dev_TP.status & TP_PRES_DOWN)) ? false : true;
}
int touch_flag = 0;
int release_flag = 0;
#include <u_thread_util.h>
static uint8_t stack0[1024 + 512];
static uint8_t msg_buf[128];
extern void LCD_Handler(void);
void thread_nes(void)
{
    while (1)
    {
        NesFrameCycle();
        u_sleep_ms(1);
    }
}
static uint8_t FPSData[20];

int main(int argc, char *args[])
{
    uint8_t color = 0;
    u_sleep_ms(500);
    u_drv_init();
    LCD_Buffer = mm_alloc_page(MM_PROT, LCD_SCREEN_W * LCD_SCREEN_H * 2, 0);
    printf("LCD_Buffer1:0x%x\n", LCD_Buffer);
    // LCD_Buffer2 = mm_alloc_page(MM_PROT, LCD_SCREEN_W * LCD_SCREEN_H * 2, 0);
    // printf("LCD_Buffer1:0x%x\n", LCD_Buffer2);
    // LCD_Buffer = LCD_Buffer2;
    nesImg = mm_alloc_page(MM_PROT, 256 * 260 * sizeof(uint16_t), 0);
    printf("nesImg:0x%x\n", nesImg);

    gui_init();
    RGBLCDInit();
    LCD_Start(LCD);
    touch_ic_init();
    mk_printf("init lcd.\n");

    nes_main();
    DMA2D_InitStructure DMA2D_initStruct;

    DMA2D_initStruct.Interval = sys_read_clk() / 1000000;
    DMA2D_initStruct.IntEOTEn = 0;
    DMA2D_Init(&DMA2D_initStruct);

    // obj_handler_t obj_th;
    // u_thread_create(&obj_th, stack0 + 1024 + 512, msg_buf, thread_nes);
    thread_run(-1, 10);
    // u_thread_run(obj_th, 3);

    while (1)
    {
        volatile uint32_t st_tick;
        volatile uint32_t end_tick;
        st_tick = sys_read_tick();
        GUIEvent();
        // LCD_SetLayerPos(LCD, LCD_LAYER_1, 0, LCD_SCREEN_W - 1, 0, LCD_SCREEN_H - 1);
        NesFrameCycle();
        touch_handler();
        if (touchpad_is_pressed())
        {
            if (touch_flag == 0)
            {
                GUISendTouchMsg(MSG_TOUCH_PRESS, Dev_TP.x[0], Dev_TP.y[0]);
                touch_flag = 1;
                release_flag = 1;
            }
            else
            {
                GUISendTouchMsg(MSG_TOUCH_MOVE, Dev_TP.x[0], Dev_TP.y[0]);
                GUISendCursorMsg(MSG_CURSOR_MOVE, Dev_TP.x[0], Dev_TP.y[0]);
            }
        }
        else
        {
            touch_flag = 0;
            if (release_flag)
            {
                GUISendTouchMsg(MSG_TOUCH_RELEASE, Dev_TP.x[0], Dev_TP.y[0]);
                release_flag = 0;
            }
        }
        if (sys_read_tick() - st_tick < 1000 / 60)
        {
            u_sleep_ms(1000 / 60 - (sys_read_tick() - st_tick));
        }
        end_tick = sys_read_tick();
        if (end_tick - st_tick > 0)
        {
            sprintf(FPSData, "%dfps", (uint32_t)(1000 / ((sys_read_tick() - st_tick))));
            TextWidgetSetText(fpsTextWidge, FPSData);
        }
    }
    return 0;
}

void RGBLCDInit(void)
{
    uint32_t i;
    LCD_InitStructure LCD_initStruct;
    LCD_LayerInitStructure LCD_layerInitStruct;
    GPIO_Init(GPIOD, PIN0, 1, 0, 0, 0);
    GPIO_SetBit(GPIOD, PIN0);

    LCD_SPI_Init_OTM8009A();

    // PORT_Init(PORTB, PIN1,  PORTB_PIN1_LCD_B0,  0);
    // PORT_Init(PORTB, PIN11, PORTB_PIN11_LCD_B1, 0);
    // PORT_Init(PORTB, PIN13, PORTB_PIN13_LCD_B2, 0);
    PORT_Init(PORTB, PIN15, PORTB_PIN15_LCD_B3, 0);
    PORT_Init(PORTA, PIN2, PORTA_PIN2_LCD_B4, 0);
    PORT_Init(PORTA, PIN9, PORTA_PIN9_LCD_B5, 0);
    PORT_Init(PORTA, PIN10, PORTA_PIN10_LCD_B6, 0);
    PORT_Init(PORTA, PIN11, PORTA_PIN11_LCD_B7, 0);
    // PORT_Init(PORTA, PIN12, PORTA_PIN12_LCD_G0, 0);
    // PORT_Init(PORTA, PIN13, PORTA_PIN13_LCD_G1, 0);
    PORT_Init(PORTA, PIN14, PORTA_PIN14_LCD_G2, 0);
    PORT_Init(PORTA, PIN15, PORTA_PIN15_LCD_G3, 0);
    PORT_Init(PORTC, PIN0, PORTC_PIN0_LCD_G4, 0);
    PORT_Init(PORTC, PIN1, PORTC_PIN1_LCD_G5, 0);
    PORT_Init(PORTC, PIN2, PORTC_PIN2_LCD_G6, 0);
    PORT_Init(PORTC, PIN3, PORTC_PIN3_LCD_G7, 0);
    // PORT_Init(PORTC, PIN4,  PORTC_PIN4_LCD_R0,  0);
    // PORT_Init(PORTC, PIN5,  PORTC_PIN5_LCD_R1,  0);
    // PORT_Init(PORTC, PIN8,  PORTC_PIN8_LCD_R2,  0);
    PORT_Init(PORTC, PIN9, PORTC_PIN9_LCD_R3, 0);
    PORT_Init(PORTC, PIN10, PORTC_PIN10_LCD_R4, 0);
    PORT_Init(PORTC, PIN11, PORTC_PIN11_LCD_R5, 0);
    PORT_Init(PORTC, PIN12, PORTC_PIN12_LCD_R6, 0);
    PORT_Init(PORTC, PIN13, PORTC_PIN13_LCD_R7, 0);
    PORT_Init(PORTB, PIN2, PORTB_PIN2_LCD_VSYNC, 0);
    PORT_Init(PORTB, PIN3, PORTB_PIN3_LCD_HSYNC, 0);
    PORT_Init(PORTB, PIN4, PORTB_PIN4_LCD_DEN, 0);
    PORT_Init(PORTB, PIN5, PORTB_PIN5_LCD_DCLK, 0);

    LCD_initStruct.ClkDiv = 6;
    LCD_initStruct.Format = LCD_FMT_RGB565;
    LCD_initStruct.HnPixel = LCD_HDOT;
    LCD_initStruct.VnPixel = LCD_VDOT;
    LCD_initStruct.Hfp = 20;
    LCD_initStruct.Hbp = 20;
    LCD_initStruct.Vfp = 16;
    LCD_initStruct.Vbp = 16;
    LCD_initStruct.HsyncWidth = 2;
    LCD_initStruct.VsyncWidth = 10;
#if USE_FRAMEBUFFER_MODE
    LCD_initStruct.DataSource = (uint32_t)GUIGetBuffer();
#else
    LCD_initStruct.DataSource = (uint32_t)LCD_Buffer;
#endif
    LCD_initStruct.Background = 0xffff;
    LCD_initStruct.SampleEdge = LCD_SAMPLE_RISE; // LCD_SAMPLE_FALL;
    LCD_initStruct.IntEOTEn = 1;
    LCD_Init(LCD, &LCD_initStruct);

    // LCD_layerInitStruct.Alpha = 0xFF;
    // LCD_layerInitStruct.HStart = 0;
    // LCD_layerInitStruct.HStop = LCD_SCREEN_W - 1;
    // LCD_layerInitStruct.VStart = 0;
    // LCD_layerInitStruct.VStop = LCD_SCREEN_H - 1;
    // LCD_layerInitStruct.DataSource = (uint32_t)LCD_Buffer1;
    // LCD_LayerInit(LCD, LCD_LAYER_1, &LCD_layerInitStruct);

    // LCD_layerInitStruct.Alpha = 0xFF;
    // LCD_layerInitStruct.HStart = 0;
    // LCD_layerInitStruct.HStop = LCD_SCREEN_W - 1;
    // LCD_layerInitStruct.VStart = 0;
    // LCD_layerInitStruct.VStop = LCD_SCREEN_H - 1;
    // LCD_layerInitStruct.DataSource = (uint32_t)LCD_Buffer2;
    // LCD_LayerInit(LCD, LCD_LAYER_2, &LCD_layerInitStruct);
}
extern obj_handler_t lcd_irq_obj;

void LCD_Handler(void)
{
    while (1)
    {
        msg_tag_t tag = uirq_wait(lcd_irq_obj, 0);
        if (msg_tag_get_val(tag) >= 0)
        {
#if USE_MUTL_LAYER
            GUISwLayer();
            LCD->L[0].ADDR = (uint32_t)GUIGetBuffer();
#endif
            LCD_INTClr(LCD);
            LCD_Start(LCD);
        }
        uirq_ack(lcd_irq_obj, LCD_IRQn);
    }
}

extern obj_handler_t dma2d_irq_obj;

void DMA2D_Handler(void)
{
    while (1)
    {
        msg_tag_t tag = uirq_wait(dma2d_irq_obj, 0);
        if (msg_tag_get_val(tag) >= 0)
        {
            DMA2D_INTClr();
        }
        uirq_ack(dma2d_irq_obj, DMA2D_IRQn);
    }
}
