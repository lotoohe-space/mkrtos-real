#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ns.h"
#include "nsfs.h"
#include <assert.h>

int nsfs_mkdir_test(void)
{
    int ret;

    ret = fs_ns_mkdir("/bin/tst/");
    assert(ret < 0);
    ret = fs_ns_mkdir("/bin/");
    assert(ret >= 0);
    ret = fs_ns_mkdir("/bin2");
    assert(ret >= 0);
    ret = fs_ns_mkdir("/bin/tst/");
    assert(ret >= 0);
    ret = fs_ns_remove("/bin/tst");
    assert(ret < 0);
    ret = fs_ns_rmdir("/bin/tst");
    assert(ret >= 0);
    return 0;
}

int nsfs_scan_dir_test(void)
{
#define TEST_CN 10
    int ret;
    ret = fs_ns_mkdir("/bin/");
    assert(ret >= 0);

    for (int i = 0; i < TEST_CN; i++)
    {
        char dir_name[32];

        snprintf(dir_name, sizeof(dir_name), "/bin/tst%d", i);
        ret = fs_ns_mkdir(dir_name);
        assert(ret >= 0);
    }
    int fd;

    fd = fs_ns_open("/bin/", O_RDWR, 0777);
    assert(fd >= 0);
    struct dirent dir_info;
    for (int i = 0; i < (TEST_CN + 1); i++)
    {
        ret = fs_ns_readdir(fd, &dir_info);
        if (i < TEST_CN)
        {
            assert(ret >= 0);
            printf("readdir info :%s\n", dir_info.d_name);
            fflush(stdout);
        }
        else
        {
            assert(ret < 0);
        }
    }
    ret = fs_ns_lseek(fd, 2, SEEK_SET);
    printf("old seek: %d\n", ret);
    assert(ret >= 0);
    for (int i = 0; i < (TEST_CN - 2 + 1); i++)
    {
        ret = fs_ns_readdir(fd, &dir_info);
        if (i < TEST_CN - 2)
        {
            assert(ret >= 0);
            printf("readdir info :%s\n", dir_info.d_name);
            fflush(stdout);
        }
        else
        {
            assert(ret < 0);
        }
    }

    ret = fs_ns_close(fd);
    assert(ret >= 0);
    ret = fs_ns_readdir(fd, &dir_info);
    assert(ret < 0);

    for (int i = 0; i < TEST_CN; i++)
    {
        char dir_name[32];
        snprintf(dir_name, sizeof(dir_name), "/bin/tst%d", i);
        ret = fs_ns_rmdir(dir_name);
        assert(ret >= 0);
    }
    ret = fs_ns_rmdir("/bin");
    assert(ret >= 0);
#undef TEST_CN
    return 0;
}
int nsfs_mksvr_node(void)
{
    int ret;
    struct dirent dir;

    ret = fs_ns_mkdir("/bin/");
    assert(ret >= 0);
    ret = ns_mknode("/bin/tst1", 10, NODE_TYPE_SVR);
    assert(ret >= 0);
    int fd;

    fd = fs_ns_open("/bin/tst1", O_RDWR, 0777);
    assert(fd < 0);
    fd = fs_ns_open("/bin/", O_RDWR, 0777);
    assert(fd >= 0);
    ret = fs_ns_readdir(fd, &dir);
    assert(ret >= 0);
    printf("readdir info :%s\n", dir.d_name);
    ret = fs_ns_close(fd);
    assert(ret >= 0);

    ret = fs_ns_remove("/bin/tst1/");
    assert(ret >= 0);
    ret = fs_ns_rmdir("/bin/");
    assert(ret >= 0);
    return 0;
}
int nsfs_query_svr_node(void)
{
    int ret;
    ret = fs_ns_mkdir("/bin/");
    assert(ret >= 0);
    ns_node_t *node;
    ns_node_t *pnode;
    int cur_inx;
    int p_inx;

    node = ns_node_find_full_file("/bin/", &ret, &cur_inx);
    assert(node == NULL && ret < 0);

#define SVR_NODE "/svr_tst"
    ret = ns_mknode(SVR_NODE, 10, NODE_TYPE_SVR);
    assert(ret >= 0);

    node = ns_node_find_full_file(SVR_NODE, &ret, &cur_inx);
    assert(node != NULL && ret >= 0);
    assert(cur_inx == strlen(SVR_NODE));

    node = ns_node_find(&pnode, SVR_NODE "/1/2/3", &ret, &cur_inx, &p_inx);
    assert(node != NULL && ret >= 0);
    assert(cur_inx == strlen(SVR_NODE) + 1);

    ret = fs_ns_mkdir(SVR_NODE "/1");
    assert(ret < 0);
#undef SVR_NODE
    ret = fs_ns_remove("/svr_tst");
    assert(ret >= 0);
#define SVR_NODE "/bin/svr_tst"
    ret = ns_mknode(SVR_NODE, 10, NODE_TYPE_SVR);
    assert(ret >= 0);
    node = ns_node_find_full_file(SVR_NODE, &ret, &cur_inx);
    assert(node != NULL && ret >= 0);
    assert(cur_inx == strlen(SVR_NODE));
    node = ns_node_find_svr_file(SVR_NODE "/1/2/3", &ret, &cur_inx);
    assert(node != NULL && ret >= 0);
    assert(cur_inx == strlen(SVR_NODE) + 1);

    ret = fs_ns_rmdir("/bin/");
    assert(ret < 0);

    ret = fs_ns_remove(SVR_NODE);
    assert(ret >= 0);
    ret = fs_ns_rmdir("/bin/");
    assert(ret >= 0);
    return 0;
}
int nsfs_stat_test(void)
{
    int ret;
    struct stat fst;
    int fd;

    ret = fs_ns_mkdir("/tstdir/");
    assert(ret >= 0);
    fd = fs_ns_open("/tstdir/", O_RDWR, 0777);
    assert(fd >= 0);
    ret = fs_ns_stat(fd, &fst);
    assert(ret >= 0);
    ret = fs_ns_close(fd);
    assert(ret >= 0);
    return 0;
}
int main(int argc, char *argv[])
{
    nsfs_mkdir_test();
    nsfs_scan_dir_test();
    nsfs_stat_test();
    nsfs_mksvr_node();
    nsfs_query_svr_node();
    return 0;
}