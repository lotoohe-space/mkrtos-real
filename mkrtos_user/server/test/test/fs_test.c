

#include "fs_cli.h"
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <stddef.h>
void fs_test(void)
{
    char tmp[4] = "123";
    int fd = fs_open("/test", O_CREAT | O_RDWR, 0777);
    assert(fd >= 0);
    int wlen = fs_write(fd, tmp, 4);
    assert(wlen == 4);
    int ret = fs_lseek(fd, 0, SEEK_SET);
    assert(ret >= 0);
    int rlen = fs_read(fd, tmp, 4);
    assert(rlen == 4);
    assert(strcmp(tmp, "123") == 0);
    fs_close(fd);
}
