

#include <u_types.h>
#include <errno.h>
#include <libfdt.h>
#include <mk_dev.h>
#include <stdio.h>
#include <assert.h>
#include "mk_drv.h"
#include "mk_snd_drv.h"
#include "mk_dtb_parse.h"
#include <u_prot.h>
#include <u_vmam.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "mk_snd_drv_impl.h"
#include "at_surf_f437_board_audio.h"

typedef struct snd_priv_data
{
} snd_priv_data_t;

static snd_priv_data_t snd_priv_data;

static int snd_bus_configure(mk_snd_t *drv)
{
    assert(drv);
    snd_priv_data_t *priv_data = drv->priv_data;

    return 0;
}
static int snd_bus_read(mk_snd_t *drv, uint8_t *data, int len)
{

    return -1;
}
static int snd_bus_write(mk_snd_t *drv, uint8_t *data, int len)
{

    return -1;
}

static mk_snd_ops_t snd_ops = {
    .snd_configure = snd_bus_configure,
    .snd_read = snd_bus_read,
    .snd_write = snd_bus_write,
};
static mk_snd_dev_t snd_dev = {
    .ops = &snd_ops,
    .snd.priv_data = &snd_priv_data,
};

static int snd_probe(mk_dev_t *dev)
{
    int ret;

    /* ioremap */
    ret = dev_regs_map(dev, dev->dtb);
    if (ret < 0)
    {
        printf("snd dev regs map failed.\n");
        return -ENOMEM;
    }
    if (audio_init() != SUCCESS)
    {
        printf("audio init failed.\n");
        return -EIO;
    }
    printf("snd init success.\r\n");
    /* 注册snd设备 */
    // mk_snd_register(dev, &snd_dev);
    return 0;
}

static mk_drv_compatible_t drv_compatilbe[] =
    {
        {"at32f43x,snd"},
        {NULL},
};

static mk_drv_t snd_drv =
    {
        .compatilbe = drv_compatilbe,
        .probe = snd_probe,
        .data = NULL,
};
void drv_snd_init(void)
{
    mk_drv_register(&snd_drv);
}
