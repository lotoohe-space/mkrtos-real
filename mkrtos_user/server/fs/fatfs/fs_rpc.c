#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "fs_svr.h"
#include <stdio.h>
#include "ff.h"
static fs_t fs;

void fs_svr_init(obj_handler_t ipc)
{
    fs_init(&fs);
    fs.ipc = ipc;
}
typedef struct file_desc
{
    FIL fp;
} file_desc_t;

#define FILE_DESC_NR 16
static file_desc_t files[FILE_DESC_NR];

file_desc_t *alloc_file(int *fd)
{
    for (int i = 0; i < FILE_DESC_NR; i++)
    {
        if (files[i].fp.obj.fs == NULL)
        {
            *fd = i;
            return &files[i];
        }
    }
    return NULL;
}
void free_file(int fd)
{
    files[fd].fp.obj.fs = NULL;
}
int fs_svr_open(const char *path, int flags, int mode)
{
    printf("open %s.\n", path);
    int fd;
    file_desc_t *file = alloc_file(&fd);

    if (!file)
    {
        return -ENOMEM;
    }
    FRESULT ret = f_open(&file->fp, path, mode);

    if (ret != FR_OK)
    {
        printf("open fail..\n");
        free_file(fd);
    }

    return -ret;
}

void fs_svr_loop(void)
{
    rpc_loop(fs.ipc, &fs.svr);
}