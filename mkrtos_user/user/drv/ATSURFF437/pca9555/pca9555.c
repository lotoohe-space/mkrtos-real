
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <u_types.h>
#include <mk_i2c_drv.h>
#include <mk_pca9555_drv.h>
#include <errno.h>
#include <stdio.h>
#include "pca9555.h"
#include "u_sleep.h"
/**
 * @brief pca9555 init.
 *
 * @param io9555
 * @return int
 */
int pca9555_init(pca9555_t *io9555)
{
    assert(io9555);

    int ret;
again:
    io9555->i2c_fd = open(io9555->i2c_name, O_RDWR, 0777);

    if (io9555->i2c_fd < 0)
    {
        u_sleep_ms(50);
        goto again;
    }
    ret = lseek(io9555->i2c_fd, (0 << 16) | (io9555->i2c_addr), SEEK_SET);
    if (ret < 0)
    {
        printf("%s:%d ret:%d\n", __func__, __LINE__, ret);
        return ret;
    }

    ret = pca9555_input_read(io9555);
    if (ret < 0)
    {
        printf("%s:%d ret:%d\n", __func__, __LINE__, ret);
        close(io9555->i2c_fd);
        return ret;
    }
    return ret;
}

int pca9555_set_mode(pca9555_t *io9555, enum pca9555_io_mode mode, uint16_t val)
{
    assert(io9555);
    int ret;
    if (mode == PCA9555_SET_INPUT_MODE)
    {
        io9555->imode |= val;
    }
    else
    {
        io9555->imode &= ~val;
    }
    const uint8_t write_init_data[] = {
        PCA_REG_CTRL,
        io9555->imode & 0xff,
        (io9555->imode >> 8) & 0xff,
    };
    ret = write(io9555->i2c_fd, write_init_data, sizeof(write_init_data));
    if (ret != sizeof(write_init_data))
    {
        printf("error:%s:%d ret:%d\n", __func__, __LINE__, ret);
        // close(i2c_fd);
        return -1;
    }

    return 0;
}
int pca9555_output_write(pca9555_t *io9555, uint16_t val)
{
    int ret;

    io9555->odata = val;
    const uint8_t write_init_data[] = {
        PCA_REG_OUTPUT,
        io9555->odata & 0xff,
        (io9555->odata >> 8) & 0xff,
    };
    ret = write(io9555->i2c_fd, write_init_data, sizeof(write_init_data));
    if (ret != sizeof(write_init_data))
    {
        printf("error:%s:%d ret:%d\n", __func__, __LINE__, ret);
        return -1;
    }
    return 0;
}

int pca9555_input_read(pca9555_t *io9555)
{
    int ret;

    ret = lseek(io9555->i2c_fd, (RT_I2C_NO_STOP << 16) | (io9555->i2c_addr), SEEK_SET);
    if (ret < 0)
    {
        printf("error:%s:%d ret:%d\n", __func__, __LINE__, ret);
        return ret;
    }
    const uint8_t write_init_data[] = {PCA_REG_INPUT};
    ret = write(io9555->i2c_fd, write_init_data, sizeof(write_init_data));
    if (ret != sizeof(write_init_data))
    {
        printf("error:%s:%d ret:%d\n", __func__, __LINE__, ret);
        return -1;
    }
    ret = lseek(io9555->i2c_fd, (io9555->i2c_addr), SEEK_SET);
    if (ret < 0)
    {
        printf("error:%s:%d ret:%d\n", __func__, __LINE__, ret);
        return ret;
    }
    ret = read(io9555->i2c_fd, &io9555->idata, sizeof(io9555->idata));
    if (ret < 0)
    {
        printf("error:%s:%d ret:%d\n", __func__, __LINE__, ret);
        return -1;
    }
#if 0
    printf("0x%x\n", io9555->idata);
#endif
    return ret;
}