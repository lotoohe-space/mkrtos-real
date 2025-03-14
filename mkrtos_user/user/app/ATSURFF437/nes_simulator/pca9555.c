
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <u_types.h>
#include <stdio.h>
#include <unistd.h>
#include "pca9555.h"
#include "joystick.h"
#include "u_sleep.h"
static int pca9555_fd;
int pca9555_init(void)
{
again:
    pca9555_fd = open("/pca9555", O_RDWR, 0777);
    if (pca9555_fd < 0)
    {
        u_sleep_ms(50);
        // printf("%s:%d ret:%d\n", __func__, __LINE__, pca9555_fd);
        // return pca9555_fd;
        goto again;
    }
    int ret;

    ret = ioctl(pca9555_fd, PCA9555_SET_INPUT_MODE, JOY_LEFT_PIN | JOY_RIGHT_PIN | JOY_UP_PIN | JOY_DOWN_PIN | JOY_ENTER_PIN | BUZZ_PIN);
    if (ret < 0)
    {
        printf("%s:%d ret:%d\n", __func__, __LINE__, ret);
        return ret;
    }
    return 0;
}
int pca9555_io_input_read(void)
{
    uint16_t idata;
    int ret;

    ret = read(pca9555_fd, &idata, 2);
    if (ret < 0)
    {
        return ret;
    }
    return idata;
}