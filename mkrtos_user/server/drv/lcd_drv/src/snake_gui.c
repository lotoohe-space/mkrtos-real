#include <x_widget.h>
#include <xwindows.h>
#include <snake.h>
#include <GUI_timeout.h>
#include <gui.h>
static p_win_t snake_win;
static p_guitime_out_t snake_timeout;

static uint8_t WinProcessSnakeFunction(void *Object, void *arg, p_msg_t hMsg)
{
    switch (hMsg->msgType)
    {
    case MSG_WIN:
        if (hMsg->msgID == MSG_WIN_CREATE)
        {
        }
        else if (hMsg->msgID == MSG_WIN_PAINT)
        {
            /*重绘*/
            snake_gui_update(Object);
        }
        break;
    case MSG_KEY:
        switch (hMsg->msgID)
        {
        case MSG_KEY_UP:
            if (hMsg->msgVal.status)
            {
                snake_set_dir(3);
            }
            else
            {
            }
            break;
        case MSG_KEY_DOWN:
            if (hMsg->msgVal.status)
            {
                 snake_set_dir(0);
            }
            else
            {
            }
            break;
        case MSG_KEY_LEFT:
            if (hMsg->msgVal.status)
            {
                 snake_set_dir(1);
            }
            else
            {
            }
            break;
        case MSG_KEY_RIGHT:
            if (hMsg->msgVal.status)
            {
                 snake_set_dir(2);
            }
            else
            {
            }
            break;
        case MSG_KEY_0:
            if (hMsg->msgVal.status)
            {
            }
            else
            {
            }
            break;
        case MSG_KEY_1:
            if (hMsg->msgVal.status)
            {
            }
            else
            {
            }
            break;
        case MSG_KEY_N:
            if (hMsg->msgVal.status)
            {
            }
            else
            {
            }
            break;
        case MSG_KEY_M:
            if (hMsg->msgVal.status)
            {
            }
            else
            {
            }
            break;

        case MSG_KEY_W:
            if (hMsg->msgVal.status)
            {
            }
            else
            {
            }
            break;
        case MSG_KEY_S:
            if (hMsg->msgVal.status)
            {
            }
            else
            {
            }
            break;
        case MSG_KEY_A:
            if (hMsg->msgVal.status)
            {
            }
            else
            {
            }
            break;
        case MSG_KEY_D:
            if (hMsg->msgVal.status)
            {
            }
            else
            {
            }
            break;
        case MSG_KEY_V:
            if (hMsg->msgVal.status)
            {
            }
            else
            {
            }
            break;
        case MSG_KEY_B:
            if (hMsg->msgVal.status)
            {
            }
            else
            {
            }
            break;
        }
        break;
    }
    return TRUE;
}
static void GUITimeoutSnakeCallback(void *arg)
{
    snake_update();
    WindowsInvaildRect(arg, NULL);
    GUITimeoutOpen(snake_timeout);
}
void snake_gui_init(void)
{
    snake_win = WindowsCreate("Snake", 0, 0, 375, 300);
    WindowsSetProcessCallBack(snake_win, WinProcessSnakeFunction);
    // _SET_IS_DPY(hWin4);
    WinListAdd(&snake_win->groupWidge.widgeBase);
    snake_timeout = GUITimeoutCreate(500, snake_win, GUITimeoutSnakeCallback);
    GUITimeoutOpen(snake_timeout);
}
