

#include <u_types.h>
#include <errno.h>
#include <libfdt.h>
#include <mk_dev.h>
#include <stdio.h>
#include <assert.h>
#include "mk_drv.h"
#include "mk_display_drv.h"
#include "mk_dtb_parse.h"
#include <u_prot.h>
#include <u_vmam.h>
#include "at32f435_437_gpio.h"
#include "at32f435_437_crm.h"
#include <at_surf_f437_board_lcd.h>

static int display_get_info(mk_display_t *drv, mk_display_cfg_t *cfg)
{
    assert(drv);
    assert(cfg);

    cfg->mode = MK_DISPLAY_NO_BUFFER;
    cfg->display_addr = XMC_LCD_DATA;
    cfg->display_cmd_addr = XMC_LCD_COMMAND;
    return 0;
}
static int display_set_win(mk_display_t *drv, mk_display_win_t *dis_win)
{
    assert(drv);
    assert(dis_win);

    lcd_windows_set(dis_win->x, dis_win->y, dis_win->x + dis_win->w - 1, dis_win->y + dis_win->h - 1);
    return 0;
}

static mk_display_ops_t pin_ops = {
    .display_get_info = display_get_info,
    .display_set_win = display_set_win,
};
static mk_display_dev_t pin_dev = {
    .ops = &pin_ops,
};

static int display_probe(mk_dev_t *dev)
{
    int ret;

    /* ioremap */
    ret = dev_regs_map(dev, dev->dtb);
    if (ret < 0)
    {
        printf("display dev regs map failed.\n");
    }
    lcd_init(LCD_DISPLAY_VERTICAL);
    /* 注册display设备 */
    mk_display_register(dev, &pin_dev);
    return 0;
}

static mk_drv_compatible_t drv_compatilbe[] =
    {
        {"at32f43x,lcd"},
        {NULL},
};

static mk_drv_t display_drv =
    {
        .compatilbe = drv_compatilbe,
        .probe = display_probe,
        .data = NULL,
};
void drv_display_init(void)
{
    mk_drv_register(&display_drv);
}
