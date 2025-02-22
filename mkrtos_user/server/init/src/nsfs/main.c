#include <stdlib.h>
#include <stdio.h>
#include "nsfs.h"
#include <assert.h>

int nsfs_test(void)
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
    assert(ret >= 0 );
    return 0;
}

int main(int argc, char *argv[])
{
    nsfs_test();
    return 0;
}