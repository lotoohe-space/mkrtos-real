#pragma once
#include <u_types.h>
#include <sys/types.h>

typedef struct file_desc
{
    pid_t pid;
    void (*close)(int fd);
    void *priv;
} file_desc_t;

int file_desc_init(void);
int file_desc_alloc(pid_t pid, void *priv, void (*close_fn)(int fd));
int file_desc_set_priv(int fd, void *priv);
file_desc_t *file_desc_get(int fd);
void *file_desc_free_unlock(int fd);
void *file_desc_free(int fd);
void file_desc_recycle(pid_t pid);
