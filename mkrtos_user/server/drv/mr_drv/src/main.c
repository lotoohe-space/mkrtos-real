
#include "mrapi.h"
#include "device/spi.h"
#define SPI_DEVICE0_CS_NUMBER 10
#define SPI_DEVICE1_CS_NUMBER 20
struct mr_spi_device spi_device0, spi_device1;
int main(int argc, char *args[])
{
    printf("mr drv init...\n");

    mr_auto_init();
    /* 添加SPI设备 */
    mr_spi_device_add(&spi_device0, "spi10", SPI_DEVICE0_CS_NUMBER);
    mr_spi_device_add(&spi_device1, "spi11", SPI_DEVICE1_CS_NUMBER);

    /* 查找SPI设备 */
    mr_device_t spi0_device = mr_device_find("spi10");
    mr_device_t spi1_device = mr_device_find("spi11");

    /* 挂载总线 */
    mr_device_ioctl(spi0_device, MR_DEVICE_CTRL_CONNECT, "spi1");
    mr_device_ioctl(spi1_device, MR_DEVICE_CTRL_CONNECT, "spi1");

    /* 以可读可写的方式打开SPI设备 */
    mr_device_open(spi0_device, MR_DEVICE_OFLAG_RDWR);
    mr_device_open(spi1_device, MR_DEVICE_OFLAG_RDWR);

    /* 写入数据 */
    char buffer0[] = "hello";
    char buffer1[] = "world";
    mr_device_write(spi0_device, -1, buffer0, sizeof(buffer0) - 1);
    mr_device_write(spi1_device, -1, buffer1, sizeof(buffer1) - 1);

    /* 读取数据 */
    mr_device_read(spi0_device, -1, buffer0, sizeof(buffer0) - 1);
    mr_device_read(spi1_device, -1, buffer1, sizeof(buffer1) - 1);

    /* 关闭设备 */
    mr_device_close(spi0_device);
    mr_device_close(spi1_device);
    
    return 0;
}
