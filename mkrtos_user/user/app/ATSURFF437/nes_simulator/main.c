
#include <stdio.h>
#include <u_vmam.h>
#include <at32f435_437_conf.h>
#include <at_surf_f437_board_lcd.h>
#include <u_sleep.h>
#include <u_sys.h>
#include <u_thread.h>
#ifndef INFONES
#include <NES_Simulator/nes_main.h>
#else
#include <InfoNES.h>
#endif
void delay_ms(int ms)
{
    u_sleep_ms(ms);
}
#ifdef INFONES
#define BGR565TORGB565(a) (((a) >> 11) | ((a) & (0x3f << 5)) | (a & 0x1f) << 11)
void nes_lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height, width;
    uint16_t i, j;

    width = ex - sx + 1;
    height = ey - sy + 1;

    lcd_windows_set(sx, sy, ex, ey);

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            lcd_data_16bit_write(BGR565TORGB565(color[i * width + j]));
        }
    }
}
#endif
void NES_LCD_DisplayLine(int y_axes, uint16_t *Disaplyline_buffer)
{
    lcd_color_fill(0, y_axes, 256, y_axes, Disaplyline_buffer);
}
static int drv_iomem_init(void)
{
    addr_t addr;
    msg_tag_t tag;
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, 0),
                       XMC_MEM_BASE - PERIPH_BASE, PERIPH_BASE, &addr);
    if (msg_tag_get_val(tag) < 0)
    {
        printf("periph mem alloc failed..\n");
        return -1;
    }
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, 0),
                       0x4000000, XMC_MEM_BASE, &addr);
    if (msg_tag_get_val(tag) < 0)
    {
        printf("periph mem alloc failed..\n");
        return -1;
    }
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, 0),
                       0x10000, XMC_REG_BASE, &addr);
    if (msg_tag_get_val(tag) < 0)
    {
        printf("periph mem alloc failed..\n");
        return -1;
    }
    return 0;
}
int main(int argc, char *argv[])
{
    printf("lcd drv init..\n");
    if (drv_iomem_init() < 0)
    {
        return -1;
    }
    lcd_init(LCD_DISPLAY_VERTICAL);

    lcd_clear(BLACK);
#if 0
    thread_run(-1, 3);
#endif
    while (1)
    {
#ifndef INFONES
        nes_main();
#else
        if (InfoNES_Load(NULL) == 0)
        {
            FrameSkip = 2;
            InfoNES_Main();
        }
#endif
    }
}
