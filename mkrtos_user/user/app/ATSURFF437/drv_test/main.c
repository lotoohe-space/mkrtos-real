
#include <assert.h>
#include <stdio.h>
#include <u_vmam.h>
#include <u_sleep.h>
#include <u_sys.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "mk_pin_drv.h"
#include "mk_display_drv.h"
#include <u_vmam.h>
#define PIN_TEST 0
void delay_ms(int ms)
{
    u_sleep_ms(ms);
}
static int_fast16_t pin_test(void)
{
    int fd = open("/pin", O_RDWR, 0777);
    int ret;

    if (fd < 0)
    {
        return fd;
    }

    ioctl(fd, MK_PIN_SET_MODE, ((mk_pin_cfg_t){
                                    .mode = MK_PIN_MODE_OUTPUT,
                                    .pin = 12,
                                    .cfg = 0,
                                })
                                   .cfg_raw);
    ret = ioctl(fd, MK_PIN_GET_MODE, ((mk_pin_cfg_t){
                                          .pin = 12,
                                      })
                                         .cfg_raw);
    printf("ret = %d\n", ret);
    assert(MK_PIN_MODE_OUTPUT == ret);
    uint8_t val = 1;
    lseek(fd, 12, SEEK_SET);
    ret = write(fd, &val, 1);
    assert(ret == 1);
    val = 0;
    ret = read(fd, &val, 1);
    assert(ret == 1);
    assert(val == 1);
    while (1)
    {
        val = 1;
        // delay_ms(500);
        lseek(fd, 12, SEEK_SET);
        write(fd, &val, 1);
        val = 0;
        // delay_ms(500);
        lseek(fd, 12, SEEK_SET);
        write(fd, &val, 1);
    }
    return 0;
}
static int display_test(void)
{
    mk_display_cfg_t info;
    int fd = open("/display", O_RDWR, 0777);
    int ret;

    if (fd < 0)
    {
        return fd;
    }
    ioctl(fd, MK_DISPLAY_SET_WIN, &((mk_display_win_t){
                                      .x = 0,
                                      .y = 0,
                                      .w = 240,
                                      .h = 320,
                                  }));
    ioctl(fd, MK_DISPLAY_GET_INFO, &info);
    printf("display addr:0x%x\n", info.display_addr);
    msg_tag_t tag;
    addr_t addr;
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, 0),
                       512, info.display_addr, &addr);
    if (msg_tag_get_val(tag) < 0)
    {
        printf("periph mem alloc failed..\n");
        return -1;
    }
    int j = 0;
    while (1)
    {
        for (int i = 0; i < 230; i++)
        {
            for (int i = 0; i < 320; i++)
            {
                *(volatile uint16_t *)(info.display_addr) = j++;
            }
        }
    }
    return 0;
}
int main(int argc, char *argv[])
{
    printf("drv test init..\n");
#if PIN_TEST
    pin_test();
#endif
    display_test();
    while (1)
    {
        u_sleep_ms(10000000);
    }
}
