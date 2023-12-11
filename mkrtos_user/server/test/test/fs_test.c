

#include "fs_cli.h"
#include "cons_cli.h"
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>

void fs_test(void)
{
    {
        static char tmp[] = "\
        int a=2;\
        int b=2;\
        \
        int add(int a, int b)\
        {\
            return a+b;\
        }\
        int sub(int a, int b)\
        {\
            return a-b;\
        }\
        int _start(void)\
        {\
            sub(10, 2);\
            add(10, 2);\
            return a+b;\
        }\
        ";
        int fd = open("/mnt/1.c", O_CREAT | O_RDWR, 0777);
        assert(fd >= 0);
        int wlen = write(fd, tmp, sizeof(tmp) - 1);
        // assert(wlen == 4);
        // int ret = lseek(fd, 0, SEEK_SET);
        // assert(ret >= 0);
        // int rlen = read(fd, tmp, 4);
        // assert(rlen == 4);
        // assert(strcmp(tmp, "123") == 0);
        close(fd);
    }
    {
        char tmp[] = "int _start(void)\
        {\
            return 3;\
        }";
        int fd = open("/mnt/2.c", O_CREAT | O_RDWR, 0777);
        assert(fd >= 0);
        int i = 1000;

        while (i--)
            write(fd, tmp, sizeof(tmp) - 1);
        // assert(wlen == 4);
        // int ret = lseek(fd, 0, SEEK_SET);
        // assert(ret >= 0);
        // int rlen = read(fd, tmp, 4);
        // assert(rlen == 4);
        // assert(strcmp(tmp, "123") == 0);
        close(fd);
    }
    // {
    //     DIR *dir = opendir("/mnt/");
    //     assert(dir);
    //     struct dirent *d = readdir(dir);
    //     assert(d);
    //     printf("%s\n", d->d_name);
    //     closedir(dir);
    // }
    printf("%s ok.\n", __func__);
}
