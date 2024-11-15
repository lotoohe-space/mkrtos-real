
#include <assert.h>
#include <stdio.h>
#include <u_vmam.h>
#include <u_sleep.h>
#include <u_sys.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
void delay_ms(int ms)
{
    u_sleep_ms(ms);
}
typedef union mk_pin_cfg
{
    struct
    {
        uint16_t mode;
        uint16_t pin;
    };
    uint32_t cfg_raw;
} mk_pin_cfg_t;
enum mk_pin_mode
{
    MK_PIN_MODE_NONE = 1,
    MK_PIN_MODE_OUTPUT,
    MK_PIN_MODE_OUTPUT_OD,
    MK_PIN_MODE_INPUT,
    MK_PIN_MODE_INPUT_DOWN,
    MK_PIN_MODE_INPUT_UP,

    MK_PIN_MODE_IRQ_RISING,
    MK_PIN_MODE_IRQ_FALLING,
    MK_PIN_MODE_IRQ_EDGE,
    MK_PIN_MODE_IRQ_LOW,
    MK_PIN_MODE_IRQ_HIGH,
};
enum mk_pin_ioctl_op
{
    MK_PIN_SET_MODE,   //!< 设置模式
    MK_PIN_GET_MODE,   //!< 获取模式
    MK_PIN_SET_OP_PIN, //!< 设置要操作的引脚，标记读写的起始引脚
};
int main(int argc, char *argv[])
{
    printf("drv test init..\n");
    int fd = open("/pin", O_RDWR, 0777);
    int ret;

    if (fd < 0)
    {
        return fd;
    }

    ioctl(fd, MK_PIN_SET_MODE, ((mk_pin_cfg_t){
                      .mode = MK_PIN_MODE_OUTPUT,
                      .pin = 12,
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
}
