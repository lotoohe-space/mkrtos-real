
#include "namespace.h"
#include "u_rpc.h"
#include "ns_types.h"
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
int fs_svr_open(const char *path, int flags, int mode);
int fs_svr_readdir(int fd, dirent_t *dir);
void fs_svr_close(int fd);
int fs_svr_unlink(char *path);
int ns_reg(const char *path, obj_handler_t hd, enum node_type type);

void ns_test(void)
{
    int ret;
    ret = ns_reg("/", 0, 0);
    assert(ret < 0);
    ret = ns_reg("/test", 0, 0);
    assert(ret >= 0);
    ret = ns_reg("/test", 0, 0);
    assert(ret < 0);
    ret = ns_reg("/test1", 0, 0);
    assert(ret >= 0);
    ret = ns_reg("/test2", 0, 0);
    assert(ret >= 0);

    int fd;

    {
        fd = fs_svr_open("/c", O_CREAT, 0);
        assert(fd >= 0);
        fs_svr_close(fd);
    }
    {
        fd = fs_svr_open("/", 0, 0);
        assert(fd >= 0);
        dirent_t dir;
        while (fs_svr_readdir(fd, &dir) >= 0)
        {
            printf("%s\n", dir.d_name);
        }
        fs_svr_close(fd);
    }

    ret = fs_svr_unlink("/test");
    assert(ret >= 0);
    ret = fs_svr_unlink("/test1");
    assert(ret >= 0);
    ret = fs_svr_unlink("/test2");
    assert(ret >= 0);
    ret = fs_svr_unlink("/c");
    assert(ret >= 0);
    {
        fd = fs_svr_open("/", 0, 0);
        assert(fd >= 0);
        dirent_t dir;
        while (fs_svr_readdir(fd, &dir) >= 0)
        {
            printf("%s\n", dir.d_name);
        }
        fs_svr_close(fd);
    }
}
