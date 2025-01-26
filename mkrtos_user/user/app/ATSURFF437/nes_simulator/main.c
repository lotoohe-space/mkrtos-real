
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
#include "nes_main.h"
#include "mk_pin_drv.h"
#include "pca9555.h"
#include "joystick.h"
#include "appfs_open.h"
mk_display_cfg_t info;
int fd;
int pin_fd;
int nes_fd;
static addr_t nes_rom_addr;
static size_t new_rom_size;
void delay_ms(int ms)
{
    u_sleep_ms(ms);
}
static int new_rom_get(const char *path)
{
    int ret;
    appfs_ioctl_arg_t appfs_arg;

    nes_fd = open(path, O_RDONLY, 0777);
    if (nes_fd < 0)
    {
        return nes_fd;
    }
    ret = ioctl(nes_fd, APPFS_IOCTOL_GET_ACCESS_ADDR, &appfs_arg);
    if (ret < 0)
    {
        close(nes_fd);
        return ret;
    }
    nes_rom_addr = appfs_arg.addr;
    new_rom_size = appfs_arg.size;
    return 0;
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("nes rom is NULL.\n");
        return -1;
    }
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_nes");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_nes");
    if (new_rom_get(argv[1]) < 0)
    {
        printf("rom don't finded.\n");
        return -1;
    }
    // thread_run(-1, 4);
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
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, VMA_ADDR_PAGE_FAULT_SIM, 0),
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
    nes_load((uint8_t *)nes_rom_addr, (umword_t)(new_rom_size));
    return 0;
}
