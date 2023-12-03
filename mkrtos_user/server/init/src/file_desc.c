#include "file_desc.h"

file_desc_t *fd_alloc(ns_node_t *node)
{
    for (int i = 0; i < FILE_DESC_NR; i++)
    {
        if (fd_list[i].node == NULL)
        {
            fd_list[i].node = node;
            fd_list[i].node_iter = NULL;
            return &fd_list[i];
        }
    }
    return NULL;
}
file_desc_t *fd_get(int fd)
{
    if (fd < 0 || fd >= FILE_DESC_NR)
    {
        return NULL;
    }
    return &fd_list[fd];
}
void fd_free(int fd)
{
    if (fd < 0 || fd >= FILE_DESC_NR)
    {
        return;
    }
    fd_list[fd].node = NULL;
    fd_list[fd].node_iter = NULL;
}
