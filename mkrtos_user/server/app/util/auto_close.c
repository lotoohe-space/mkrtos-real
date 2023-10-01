
#include "stm32_sys.h"
#include "sysinfo.h"
// #include "tim3.h"
#include "u_sys.h"
#include "auto_close.h"

u32 _start_tick[CTRL_IO_NUM] = {0};
u32 _io_status[CTRL_IO_NUM] = {0};
u8 _io_en[CTRL_IO_NUM] = {0};

_ctrl_func ctrl_func[CTRL_IO_NUM] = {0};
_ctrl_func ctrl_visual_func[CTRL_IO_NUM] = {0};
/**
 * 控制调用
 */
void io_ctrl(int inx, int state)
{
    if (state)
    {
        _io_status[inx] = state;
        _start_tick[inx] = sys_read_tick();
        _io_en[inx] = 1;
    }
    else
    {
        _io_en[inx] = 0;
        _io_status[inx] = state;
    }
    if (ctrl_func[inx])
    {
        ctrl_func[inx](inx, state);
    }
}
void io_ctrl_visual(int inx, int state)
{
    if (ctrl_visual_func[inx])
    {
        ctrl_visual_func[inx](inx, state);
    }
}
void io_ctrl_reg(int inx, _ctrl_func ctrl_f, _ctrl_func ctrl_visual_f)
{
    ctrl_func[inx] = ctrl_f;
    ctrl_visual_func[inx] = ctrl_visual_f;
}
void io_ctrl_loop(void)
{

    for (int i = 0; i < CTRL_IO_NUM; i++)
    {
        if (!_io_en[i])
        {
            continue;
        }
        if (
            sys_info.auto_ctrl_tick[i] != 0 &&
            sys_read_tick() - _start_tick[i] >= sys_info.auto_ctrl_tick[i])
        {
            io_ctrl(i, 0); // 关闭io
            io_ctrl_visual(i, 0);
        }
    }
}
