
#include <stdio.h>
#include <u_vmam.h>
#include <u_sleep.h>
#include <u_sys.h>
#include <u_thread.h>
#include <u_util.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <u_task.h>
#include <string.h>
#include "mk_display_drv.h"
mk_display_cfg_t info;
int fd;
int pin_fd;
void delay_ms(int ms)
{
    u_sleep_ms(ms);
}
// #define BGR565TORGB565(a) (((a) >> 11) | ((a) & (0x3f << 5)) | (a & 0x1f) << 11)

// void nes_lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
// {
//     uint16_t height, width;
//     uint16_t i, j;

//     width = ex - sx + 1;
//     height = ey - sy + 1;
// #if 0
//     lcd_windows_set(sx, sy, ex, ey);

//     for (i = 0; i < height; i++)
//     {
//         for (j = 0; j < width; j++)
//         {
//             lcd_data_16bit_write(BGR565TORGB565(color[i * width + j]));
//         }
//     }
// #else
//     memcpy(dis_buff[dis_show_line++], color, sizeof(uint16_t) * 256);
//     if (dis_show_line >= 120)
//     {
//         ioctl(fd, MK_DISPLAY_SET_WIN, &((mk_display_win_t){
//                                           .x = 0,
//                                           .y = dis_offset,
//                                           .w = 256,
//                                           .h = 120,
//                                       }));
//         for (int j = 0; j < 120; j++)
//         {
//             for (int i = 0; i < 256; i++)
//             {
//                 *(volatile uint16_t *)(info.display_addr) = BGR565TORGB565(dis_buff[j][i]);
//             }
//         }
//         if (dis_offset == 0)
//         {
//             dis_offset = 120;
//         }
//         else
//         {
//             dis_offset = 0;
//         }
//         dis_show_line = 0;
//     }
// #endif
// }

// static uint32_t last_tick;
// static uint32_t frame_flag;
// void NES_LCD_DisplayLine(int y_axes, uint16_t *Disaplyline_buffer)
// {
//     memcpy(dis_buff[dis_show_line++], Disaplyline_buffer, sizeof(uint16_t) * 256);
//     if (dis_show_line >= 120)
//     {
//         ioctl(fd, MK_DISPLAY_SET_WIN, &((mk_display_win_t){
//                                           .x = 0,
//                                           .y = dis_offset,
//                                           .w = 256,
//                                           .h = 120,
//                                       }));
//         for (int j = 0; j < 120; j++)
//         {
//             for (int i = 0; i < 256; i++)
//             {
//                 *(volatile uint16_t *)(info.display_addr) = dis_buff[j][i];
//             }
//         }
//         if (dis_offset == 0)
//         {
//             frame_flag = !frame_flag;
//             if (frame_flag == 0)
//             {
//                 last_tick = sys_read_tick();
//             }
//             else
//             {
//                 printf("fps:%d\n", (1000 / ((sys_read_tick() - last_tick) * (5))) * 3);
//             }
//             dis_offset = 120;
//         }
//         else
//         {
//             dis_offset = 0;
//         }
//         dis_show_line = 0;
//     }
// }
#if 0
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
#endif
#include "nes_main.h"
#include "mk_pin_drv.h"
#include "pca9555.h"
#include "joystick.h"

extern const char nes_rom[];
extern const char nes_rom_end[];
int main(int argc, char *argv[])
{
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_nes");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_nes");
    thread_run(-1, 4);
    printf("nes init..\n");
#if 0
    if (drv_iomem_init() < 0)
    {
        return -1;
    }
    lcd_init(LCD_DISPLAY_VERTICAL);

    lcd_clear(BLACK);
#endif
    int ret;

    ret = pca9555_init();
    if (ret < 0)
    {
        printf("pca9555 init failed.\n");
    }
again:
    fd = open("/display", O_RDWR, 0777);

    if (fd < 0)
    {
        u_sleep_ms(50);
        goto again;
    }
    ioctl(fd, MK_DISPLAY_GET_INFO, &info);
    printf("dis_addr:0x%x\n", info.display_addr);

    pin_fd = open("/pin", O_RDWR, 0777);

    if (fd < 0)
    {
        printf("pin open fail. ref:%d\n", pin_fd);
        return -1;
    }
    ret = ioctl(pin_fd, MK_PIN_SET_MODE, ((mk_pin_cfg_t){
                                              .mode = MK_PIN_MODE_INPUT_DOWN,
                                              .pin = 0,
                                              .cfg = 0,
                                          })
                                             .cfg_raw);
    if (ret < 0)
    {
        printf("pin set fail. ref:%d\n", pin_fd);
        return -1;
    }
    ret = ioctl(pin_fd, MK_PIN_SET_MODE, ((mk_pin_cfg_t){
                                              .mode = MK_PIN_MODE_INPUT_DOWN,
                                              .pin = (2 * 16) + 13,
                                              .cfg = 0,
                                          })
                                             .cfg_raw);
    if (ret < 0)
    {
        printf("pin set fail. ref:%d\n", pin_fd);
        return -1;
    }
    msg_tag_t tag;
    addr_t addr;
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, 0),
                       512, info.display_addr, &addr);
    if (msg_tag_get_val(tag) < 0)
    {
        printf("periph mem alloc failed..\n");
        return -1;
    }
    ioctl(fd, MK_DISPLAY_SET_WIN, &((mk_display_win_t){
                                      .x = 0,
                                      .y = 0,
                                      .w = 256,
                                      .h = 240,
                                  }));
    nes_load((uint8_t *)nes_rom, (umword_t)(nes_rom_end - nes_rom));
    return 0;
}
