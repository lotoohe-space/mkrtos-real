#pragma once
#include <ns_types.h>

#define FILE_DESC_NR 32

typedef struct file_desc
{
    ns_node_t *node;
    ns_node_t *node_iter;
} file_desc_t;

static file_desc_t fd_list[FILE_DESC_NR];
file_desc_t *fd_alloc(ns_node_t *node);
file_desc_t *fd_get(int fd);
void fd_free(int fd);
