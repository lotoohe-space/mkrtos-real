#include <ff.h>
#include "u_log.h"
#include <stdio.h>
static FATFS fs;
static BYTE buff[512];
static MKFS_PARM defopt = {FM_ANY, 0, 0, 0};

int main(int args, char *argv[])
{
    FRESULT res = f_mount(&fs, "0:", 1);
    if (res != FR_OK)
    {
        res = f_mkfs("0:", &defopt, buff, 512); // 第三个参数可以设置成NULL，默认使用heap memory
        if (res != FR_OK)
        {
            printf("f_mkfs err.\n");
            while (1)
                ;
        }
        else
        {
            res = f_mount(&fs, "0:", 1);
            if (res != FR_OK)
            {
                printf("f_mount err.\n");
                while (1)
                    ;
            }
        }
    }
    printf("mount success\n");
    while (1)
        ;
    return -1;
}
