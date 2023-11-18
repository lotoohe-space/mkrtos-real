#pragma once
#include <u_type.h>
typedef struct fd_map_entry fd_map_entry_t;

int fd_map_alloc(uint16_t svr_fd, uint16_t priv_fd);
int fd_map_update(int fd, fd_map_entry_t *new_entry);
int fd_map_free(int fd, fd_map_entry_t *ret_entry);
