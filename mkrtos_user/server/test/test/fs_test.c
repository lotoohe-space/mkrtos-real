

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
    int i = 1000;
    while (i--)
    {
        char tmp[4] = "123";
        int fd = open("/mnt/1.txt", O_CREAT | O_RDWR, 0777);
        assert(fd >= 0);
        int wlen = write(fd, tmp, 4);
        assert(wlen == 4);
        int ret = lseek(fd, 0, SEEK_SET);
        assert(ret >= 0);
        int rlen = read(fd, tmp, 4);
        assert(rlen == 4);
        assert(strcmp(tmp, "123") == 0);
        close(fd);
    }
    printf("%s ok.\n", __func__);
}
