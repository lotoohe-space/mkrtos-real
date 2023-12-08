#pragma once
#include <u_types.h>

typedef struct fd_map_entry
{
    uint32_t svr_fd;
    uint32_t priv_fd;
    uint8_t type;
    uint8_t flags;
} fd_map_entry_t;

enum fd_type
{
    FD_TTY,
    FD_FS,
};

int fd_map_alloc(uint32_t svr_fd, uint32_t priv_fd, enum fd_type type);
int fd_map_update(int fd, fd_map_entry_t *new_entry);
int fd_map_free(int fd, fd_map_entry_t *ret_entry);
int fd_map_get(int fd, fd_map_entry_t *new_entry);
