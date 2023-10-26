

#include "hmi_lcd_process.h"
#include "stm32_sys.h"
#include "sysinfo.h"
#include "led.h"
#include "relay.h"
#include "music_control.h"
#include "auto_close.h"
#include "u_sys.h"
#include "led.h"
#include <stdio.h>
#include <stdlib.h>

// 刷新时间
#define HIM_LCD_TIME_100MS 500
// LCD串口接收BUF
uint8_t cmd_buffer[CMD_MAX_SIZE]; // 指令缓存

/*!
 *  \brief  消息处理流程
 *  \param msg 待处理消息
 *  \param size 消息长度
 */
void ProcessMessage(PCTRL_MSG msg, uint16_t size)
{
    uint8_t cmd_type = msg->cmd_type;                // 指令类型
    uint8_t ctrl_msg = msg->ctrl_msg;                // 消息的类型
    uint8_t control_type = msg->control_type;        // 控件类型
    uint16_t screen_id = PTR2U16(&msg->screen_id);   // 画面ID
    uint16_t control_id = PTR2U16(&msg->control_id); // 控件ID
    uint32_t value = PTR2U32(msg->param);            // 数值
    switch (cmd_type)
    {
    case NOTIFY_TOUCH_PRESS:   // 触摸屏按下
    case NOTIFY_TOUCH_RELEASE: // 触摸屏松开
        NotifyTouchXY(cmd_buffer[1], PTR2U16(cmd_buffer + 2), PTR2U16(cmd_buffer + 4));
        break;
    case NOTIFY_WRITE_FLASH_OK: // 写FLASH成功
        NotifyWriteFlash(1);
        break;
    case NOTIFY_WRITE_FLASH_FAILD: // 写FLASH失败
        NotifyWriteFlash(0);
        break;
    case NOTIFY_READ_FLASH_OK:                        // 读取FLASH成功
        NotifyReadFlash(1, cmd_buffer + 2, size - 6); // 去除帧头帧尾
        break;
    case NOTIFY_READ_FLASH_FAILD: // 读取FLASH失败
        NotifyReadFlash(0, 0, 0);
        break;
    case NOTIFY_READ_RTC: // 读取RTC时间
        NotifyReadRTC(cmd_buffer[2], cmd_buffer[3], cmd_buffer[4], cmd_buffer[5], cmd_buffer[6], cmd_buffer[7], cmd_buffer[8]);
        break;
    case NOTIFY_CONTROL:
    {
        if (ctrl_msg == MSG_GET_CURRENT_SCREEN) // 画面ID变化通知
        {
            NotifyScreen(screen_id); // 画面切换调动的函数
        }
        else
        {
            switch (control_type)
            {
            case kCtrlButton: // 按钮控件
                NotifyButton(screen_id, control_id, msg->param[1]);
                break;
            case kCtrlText: // 文本控件
                NotifyText(screen_id, control_id, msg->param);
                break;
            case kCtrlProgress: // 进度条控件
                NotifyProgress(screen_id, control_id, value);
                break;
            case kCtrlSlider: // 滑动条控件
                NotifySlider(screen_id, control_id, value);
                break;
            case kCtrlMeter: // 仪表控件
                NotifyMeter(screen_id, control_id, value);
                break;
            case kCtrlMenu: // 菜单控件
                NotifyMenu(screen_id, control_id, msg->param[0], msg->param[1]);
                break;
            case kCtrlSelector: // 选择控件
                NotifySelector(screen_id, control_id, msg->param[0]);
                break;
            case kCtrlRTC: // 倒计时控件
                NotifyTimer(screen_id, control_id);
                break;
            default:
                break;
            }
        }
        break;
    }
    case NOTIFY_HandShake: // 握手通知
        NOTIFYHandShake();
        break;
    default:
        break;
    }
}

void LCD_SW_2PAGE()
{
    SetTextInt32(1, 2, sys_info.target_val, 1, 0);
    SetTextInt32(1, 15, sys_info.netID, 1, 0);
    SetTextInt32(1, 20, sys_info.devID, 1, 0);
    SetTextInt32(1, 32, sys_info.auto_ctrl_tick[0] / 1000 / 10, 1, 0);
    SetTextInt32(1, 33, sys_info.auto_ctrl_tick[1] / 1000 / 10, 1, 0);
    SetTextInt32(1, 35, sys_info.auto_ctrl_tick[2] / 1000 / 10, 1, 0);
    SetTextInt32(1, 34, sys_info.auto_ctrl_tick[3] / 1000 / 10, 1, 0);
    SetTextInt32(1, 11, sys_info.auto_ctrl_tick[4] / 1000 / 10, 1, 0);
    SetTextInt32(1, 14, sys_info.auto_ctrl_tick[5] / 1000 / 10, 1, 0);
}
/*!
 *  \brief  握手通知
 */
void NOTIFYHandShake()
{
}

/*!
 *  \brief  画面切换通知
 *  \details  当前画面改变时(或调用GetScreen)，执行此函数
 *  \param screen_id 当前画面ID
 */
void NotifyScreen(uint16_t screen_id)
{
}

/*!
 *  \brief  触摸坐标事件响应
 *  \param press 1按下触摸屏，3松开触摸屏
 *  \param x x坐标
 *  \param y y坐标
 */
void NotifyTouchXY(uint8_t press, uint16_t x, uint16_t y)
{
    // TODO: 添加用户代码
}

void SilderSetValue(u8 index, u16 value)
{
}

void RelaySetButtonValue(u8 index, u8 value)
{
}
static u32 timer_tick_last_ui_update = 0;

u8 up_down_flag[2] = {0};

int ctrl_vs(int inx, int state)
{
    switch (inx)
    {
    case 0:
        relay_ctrl(0, state);
        break;
    case 1:
        relay_ctrl(1, state);
        break;
    case 2:
        relay_ctrl(2, state);
        break;
    case 3:
        relay_ctrl(3, state);
        break;
    case 4:
        relay_ctrl(4, state);
        break;
    case 5:
        break;
    }
    return 0;
}
int vis_vs(int inx, int state)
{
    switch (inx)
    {
    case 0:
        SetButtonValue(0, 22, state);
        break;
    case 1:
        SetButtonValue(0, 23, state);
        break;
    case 2:
        SetButtonValue(0, 24, state);
        break;
    case 3:
        SetButtonValue(0, 36, state);
        break;
    case 4:
        SetButtonValue(0, 37, state);
        break;
    case 5:
        break;
    }
    return 0;
}
void LCD_init(void)
{
    for (int i = 0; i < CTRL_IO_NUM; i++)
    {
        io_ctrl_reg(i, ctrl_vs, vis_vs);
    }
}
/*!
 *  \brief  按钮控件通知
 *  \details  当按钮状态改变(或调用GetControlValue)时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param state 按钮状态：0弹起，1按下
 */
void NotifyButton(uint16_t screen_id, uint16_t control_id, uint8_t state)
{

    if (screen_id == 0)
    {
        switch (control_id)
        {
        case 22:
            io_ctrl(0, state);
            break;
        case 23:
            io_ctrl(1, state);
            break;
        case 24:
            io_ctrl(2, state);
            break;
        case 36:
            io_ctrl(3, state);
            break;
        case 37:
            io_ctrl(4, state);
            break;

        case 25: // 模式
            music_mode_next(0);
            break;
        case 26: // 上一曲
            if (state == 0)
            {
                if (up_down_flag[0] == 0)
                {
                    music_up(0);
                }
                else
                {
                    up_down_flag[0] = 0;
                }
            }
            else if (state == 2)
            {
                music_volume_add(0);
                up_down_flag[0] = 1;
            }
            break;
        case 27: // 暂停
            music_pause(0);
            break;
        case 28: // 下一曲
            if (state == 0)
            {
                if (up_down_flag[0] == 0)
                {
                    music_down(0);
                }
                else
                {
                    up_down_flag[0] = 0;
                }
            }
            else if (state == 2)
            {
                music_volume_sub(0);
                up_down_flag[0] = 1;
            }
            break;
        case 125: // 模式
            music_mode_next(1);
            break;
        case 126: // 上一曲
            if (state == 0)
            {
                if (up_down_flag[1] == 0)
                {
                    music_up(1);
                }
                else
                {
                    up_down_flag[1] = 0;
                }
            }
            else if (state == 2)
            {
                music_volume_add(1);
                up_down_flag[1] = 1;
            }
            break;
        case 127: // 暂停
            music_pause(1);
            break;
        case 128: // 下一曲
            if (state == 0)
            {
                if (up_down_flag[1] == 0)
                {
                    music_down(1);
                }
                else
                {
                    up_down_flag[1] = 0;
                }
            }
            else if (state == 2)
            {
                music_volume_sub(1);
                up_down_flag[1] = 1;
            }
            break;
        case 52:
            LCD_SW_2PAGE();
            break;
        }
    }
    else if (screen_id == 1)
    {
        switch (control_id)
        {
        case 16:

            break;
        }
    }
}

void NotifyText(uint16_t screen_id, uint16_t control_id, uint8_t *str)
{
    if (screen_id == 1)
    {
        switch (control_id)
        {
        case 2:
            sys_info.target_val = atoi((char *)str);
            break;
        case 32:
            sys_info.auto_ctrl_tick[0] = atoi((char *)str) * 10 * 1000;
            break;
        case 33:
            sys_info.auto_ctrl_tick[1] = atoi((char *)str) * 10 * 1000;
            break;
        case 35:
            sys_info.auto_ctrl_tick[2] = atoi((char *)str) * 10 * 1000;
            break;
        case 34:
            sys_info.auto_ctrl_tick[3] = atoi((char *)str) * 10 * 1000;
            break;
        case 11:
            sys_info.auto_ctrl_tick[4] = atoi((char *)str) * 10 * 1000;
            break;
        case 14:
            sys_info.auto_ctrl_tick[5] = atoi((char *)str) * 10 * 1000;
            break;
        case 22:
            bluetooth_name_set(0, (char *)str);
            break;
        case 15: // 网络id
            sys_info.netID = atoi((char *)str);
            break;
        case 20:
            sys_info.devID = atoi((char *)str);
            break;
        }
    }
    sys_info_save();
}

/*!
 *  \brief  进度条控件通知
 *  \details  调用GetControlValue时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param value 值
 */
void NotifyProgress(uint16_t screen_id, uint16_t control_id, uint32_t value)
{
    // TODO: 添加用户代码
}

/*!
 *  \brief  滑动条控件通知
 *  \details  当滑动条改变(或调用GetControlValue)时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param value 值
 */
void NotifySlider(uint16_t screen_id, uint16_t control_id, uint32_t value)
{
    if (screen_id == 0)
    {
        if (control_id == 2)
        { /*音量*/
            music_volume_set(0, value);
        }
        if (control_id == 129)
        { /*音量*/
            music_volume_set(1, value);
        }
    }
}

/*!
 *  \brief  仪表控件通知
 *  \details  调用GetControlValue时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param value 值
 */
void NotifyMeter(uint16_t screen_id, uint16_t control_id, uint32_t value)
{
    // TODO: 添加用户代码
}

/*!
 *  \brief  菜单控件通知
 *  \details  当菜单项按下或松开时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param item 菜单项索引
 *  \param state 按钮状态：0松开，1按下
 */
void NotifyMenu(uint16_t screen_id, uint16_t control_id, uint8_t item, uint8_t state)
{
    // TODO: 添加用户代码
}

/*!
 *  \brief  选择控件通知
 *  \details  当选择控件变化时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param item 当前选项
 */
void NotifySelector(uint16_t screen_id, uint16_t control_id, uint8_t item)
{
}

/*!
 *  \brief  定时器超时通知处理
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 */
void NotifyTimer(uint16_t screen_id, uint16_t control_id)
{
}

/*!
 *  \brief  读取用户FLASH状态返回
 *  \param status 0失败，1成功
 *  \param _data 返回数据
 *  \param length 数据长度
 */
void NotifyReadFlash(uint8_t status, uint8_t *_data, uint16_t length)
{
    // TODO: 添加用户代码
}

/*!
 *  \brief  写用户FLASH状态返回
 *  \param status 0失败，1成功
 */
void NotifyWriteFlash(uint8_t status)
{
    // TODO: 添加用户代码
}

/*!
 *  \brief  读取RTC时间，注意返回的是BCD码
 *  \param year 年（BCD）
 *  \param month 月（BCD）
 *  \param week 星期（BCD）
 *  \param day 日（BCD）
 *  \param hour 时（BCD）
 *  \param minute 分（BCD）
 *  \param second 秒（BCD）
 */
void NotifyReadRTC(uint8_t year, uint8_t month, uint8_t week, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    //    sec    =(0xff & (second>>4))*10 +(0xf & second);                                    //BCD码转十进制
    //    years   =(0xff & (year>>4))*10 +(0xf & year);
    //    months  =(0xff & (month>>4))*10 +(0xf & month);
    //    weeks   =(0xff & (week>>4))*10 +(0xf & week);
    //    days    =(0xff & (day>>4))*10 +(0xf & day);
    //    hours   =(0xff & (hour>>4))*10 +(0xf & hour);
    //    minutes =(0xff & (minute>>4))*10 +(0xf & minute);
}

/*!
 *  \brief  更新数据
 */
void UpdateUI(void)
{
    u16 show_val;
    u8 i = 0;
    int temp_val = 0;
    qsize size = 0;
    // 串口屏处理
    size = queue_find_cmd(cmd_buffer, CMD_MAX_SIZE); // 从缓冲区中获取一条指令
    if (size > 0)
    {
        ProcessMessage((PCTRL_MSG)cmd_buffer, size); // 指令处理
    }
    if (sys_read_tick() - timer_tick_last_ui_update <
        HIM_LCD_TIME_100MS)
    {
        return;
    }

    ////////////////////////////////////////////////////设置温度
    temp_val = sys_info.temp[0];
    temp_val = (((float)(temp_val + 40)) / 120.f) * 360;
    SetSliderValue(0, 7, temp_val);
    SetTextInt32(0, 20, sys_info.temp[0], 1, 0); // 设置文本值

    temp_val = sys_info.temp[1];
    temp_val = (((float)(temp_val + 40)) / 120.f) * 360;
    SetSliderValue(0, 5, temp_val);
    SetTextInt32(0, 19, (sys_info.temp[1]), 1, 0); // 设置文本值

    temp_val = sys_info.temp[2];
    temp_val = (((float)(temp_val + 40)) / 120.f) * 360;
    SetSliderValue(0, 4, temp_val);
    SetTextInt32(0, 18, (sys_info.temp[2]), 1, 0); // 设置文本值

    temp_val = sys_info.temp[3];
    temp_val = (((float)(temp_val + 40)) / 120.f) * 360;
    SetSliderValue(0, 8, temp_val);
    SetTextInt32(0, 21, (sys_info.temp[3]), 1, 0); // 设置文本值

    /////////////////////////////////////////////////////设置噪声
    temp_val = sys_info.noise[0];
    temp_val = (((float)(temp_val)) / 140.f) * 360;
    SetSliderValue(0, 1, temp_val);
    SetTextInt32(0, 29, sys_info.noise[0], 1, 0); // 设置文本值

    temp_val = sys_info.noise[1];
    temp_val = (((float)(temp_val)) / 140.f) * 360;
    SetSliderValue(0, 6, temp_val);
    SetTextInt32(0, 17, sys_info.noise[1], 1, 0); // 设置文本值

    temp_val = sys_info.noise[2];
    temp_val = (((float)(temp_val)) / 140.f) * 360;
    SetSliderValue(0, 32, temp_val);
    SetTextInt32(0, 34, sys_info.noise[2], 1, 0); // 设置文本值

    /// 分贝温度
    temp_val = sys_info.noise_temp[0];
    temp_val = (((float)(temp_val + 40)) / 120.f) * 360;
    SetSliderValue(0, 44, temp_val);
    SetTextInt32(0, 53, (sys_info.noise_temp[0]), 1, 0); // 设置文本值

    temp_val = sys_info.noise_temp[1];
    temp_val = (((float)(temp_val + 40)) / 120.f) * 360;
    SetSliderValue(0, 45, temp_val);
    SetTextInt32(0, 54, (sys_info.noise_temp[1]), 1, 0); // 设置文本值

    temp_val = sys_info.noise_temp[2];
    temp_val = (((float)(temp_val + 40)) / 120.f) * 360;
    SetSliderValue(0, 39, temp_val);
    SetTextInt32(0, 41, (sys_info.noise_temp[2]), 1, 0); // 设置文本值

    /*Zigbee温度*/
    uapp_sys_info_t *info = &sys_info;
    SetTextInt32(0, 64, info->zigbee_temp[0], 1, 0); // 设置文本值

    //	SetTextInt32(0, 100, ABS(sys_info.noise[0] - sys_info.noise[1]), 1, 0);

    timer_tick_last_ui_update = sys_read_tick();
    toogle_led_0();
}
