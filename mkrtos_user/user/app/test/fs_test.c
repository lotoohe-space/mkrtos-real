

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <u_types.h>
#include <string.h>
int fs_test(void)
{
    int fd;
    int ret;
    printf("%s start\n", __func__);

    fd = open("/mnt/1.txt", O_RDWR | O_CREAT, 0777);
    if (fd < 0)
    {
        printf("open faile:%d.\n", fd);
        return ret;
    }
    for (int i = 0; i < 66 * 1024; i++)
    {
        uint8_t wd = i;
        ret = write(fd, &wd, 1);
        if (ret < 0)
        {
            close(fd);
            printf("write fail:%d.\n", ret);
            return ret;
        }
    }
    close(fd);

    fd = open("/mnt/1.txt", O_RDWR, 0777);
    if (fd < 0)
    {
        printf("open faile:%d.\n", fd);
        return ret;
    }
    for (int i = 0; i < 66 * 1024; i++)
    {
        uint8_t wd = 0;
        ret = read(fd, &wd, 1);
        if (ret < 0)
        {
            close(fd);
            printf("write fail:%d.\n", ret);
            return ret;
        }
        if (wd != i % 256)
        {
            printf("verify data fail:%d %x");
            return -1;
        }
    }
    close(fd);
    printf("%s end\n", __func__);
    return 0;
}
static uint8_t buf[512];

int fs_test2(void)
{
    int fd;
    int ret;

    printf("%s start\n", __func__);
    for (int i = 0; i < 512; i++)
    {
        buf[i] = i;
    }

    fd = open("/mnt/1.txt", O_RDWR | O_CREAT, 0777);
    if (fd < 0)
    {
        printf("open faile:%d.\n", fd);
        return ret;
    }
    for (size_t i = 0; i < 128; i++)
    {
        ret = write(fd, buf, 33);
        if (ret < 0)
        {
            close(fd);
            printf("write fail:%d.\n", ret);
            return ret;
        }
    }

    close(fd);

    fd = open("/mnt/1.txt", O_RDWR, 0777);
    if (fd < 0)
    {
        printf("open faile:%d.\n", fd);
        return ret;
    }
    for (size_t i = 0; i < 32; i++)
    {
        ret = read(fd, buf, 33);
        if (ret < 0)
        {
            close(fd);
            printf("write fail:%d.\n", ret);
            return ret;
        }
        for (size_t j = 0; j < 33; j++)
        {
            if (buf[j] != j % 256)
            {
                close(fd);
                printf("verify data fail:%d %x");
                return -1;
            }
        }
        memset(buf, 0, 33);
    }
    close(fd);
    printf("%s end\n", __func__);
    return 0;
}

int fs_test3(void)
{
    FILE *fp;
    size_t ret;

    printf("%s start\n", __func__);

    for (int i = 0; i < 512; i++)
    {
        buf[i] = i;
    }

    fp = fopen("/mnt/2.txt", "wb+");
    if (fp == NULL)
    {
        printf("fopen failed.\n");
        return -1;
    }

    for (size_t i = 0; i < 100; i++)
    {
        ret = fwrite(buf, 1, 33, fp);
        if (ret != 33)
        {
            fclose(fp);
            printf("fwrite failed.\n");
            return -1;
        }
    }
    fclose(fp);
    fp = fopen("/mnt/2.txt", "wb+");
    if (fp == NULL)
    {
        printf("fopen failed.\n");
        return -1;
    }
    fseek(fp, 0, SEEK_SET);

    for (size_t i = 0; i < 100; i++)
    {
        ret = fread(buf, 1, 33, fp);
        if (ret != 33)
        {
            fclose(fp);
            printf("fread failed.\n");
            return -1;
        }
        for (size_t j = 0; j < 33; j++)
        {
            if (buf[j] != j % 256)
            {
                fclose(fp);
                printf("verify data fail:%d %x\n", j, buf[j]);
                return -1;
            }
        }
        memset(buf, 0, 33);
    }

    fclose(fp);
    printf("%s end\n", __func__);
    return 0;
}
