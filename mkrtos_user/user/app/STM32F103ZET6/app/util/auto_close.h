#pragma once

#include "sysinfo.h"

typedef int (*_ctrl_func)(int inx, int state);

void io_ctrl_reg(int inx, _ctrl_func ctrl_f, _ctrl_func ctrl_visual_f);
void io_ctrl_loop(void);
void io_ctrl(int inx, int state);
void io_ctrl_visual(int inx, int state);
