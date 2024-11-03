
#include <stdio.h>
#include <u_vmam.h>
#include <at32f435_437_conf.h>
#include <at_surf_f437_board_lcd.h>
#include <u_sleep.h>
#include <u_sys.h>
#include <u_thread.h>
// #include <NES_Simulator/nes_main.h>
#include <InfoNES.h>
void delay_ms(int ms)
{
    u_sleep_ms(ms);
}
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
#if 1
    thread_run(-1, 3);
#endif
    while (1)
    {
        // nes_main();
        if (InfoNES_Load(NULL) == 0)
        {
            FrameSkip = 2;
            InfoNES_Main();
        }
    }
}
