

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
    {
        DIR *dir = opendir("/mnt/");
        assert(dir);
        struct dirent *d = readdir(dir);
        assert(d);
        printf("%s\n", d->d_name);
        closedir(dir);
    }
    printf("%s ok.\n", __func__);
}
