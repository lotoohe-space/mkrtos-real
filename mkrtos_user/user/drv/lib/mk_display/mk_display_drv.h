#pragma once

#include <u_types.h>
#include "mk_drv.h"
#include "mk_dev.h"

typedef struct mk_display
{
    /*None*/
} mk_display_t;

enum mk_display_mode
{
    MK_DISPLAY_NO_BUFFER, //!< 无buffer模式
    MK_DISPLAY_BUFFER,    //!< buffer模式
};

typedef struct mk_display_cfg
{
    addr_t display_addr;       //!< 显示缓存的地址，物理地址
    addr_t display_cmd_addr;   //!< 命令地址
    enum mk_display_mode mode; //!< 驱动显示模式
} mk_display_cfg_t;

typedef struct mk_display_win
{
    int x;
    int y;
    int w;
    int h;
} mk_display_win_t;

typedef struct mk_display_ops
{
    int (*display_get_info)(mk_display_t *drv, mk_display_cfg_t *cfg);
    int (*display_set_win)(mk_display_t *drv, mk_display_win_t *dis_win);
} mk_display_ops_t;

typedef struct mk_display_dev
{
    mk_display_t display;
    mk_display_ops_t *ops;
} mk_display_dev_t;

enum mk_display_ioctl
{
    MK_DISPLAY_GET_INFO, //!< 直接获取显示的物理缓存
    MK_DISPLAY_SET_WIN,  //!< 设置显示窗口
};

int mk_display_register(mk_dev_t *dev, mk_display_dev_t *pin_dev);
