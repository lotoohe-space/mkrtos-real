

#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
void dfs_test(void)
{
    char data[5];
    int fd = open("/dev/uart2", O_RDWR, 0x777);
    assert(fd >= 0);
    write(fd, "1234\n", 5);
    read(fd, data, 5);
    close(fd);
}