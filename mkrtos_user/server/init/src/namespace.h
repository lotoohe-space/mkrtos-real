#pragma once
#include "u_types.h"
#include "u_prot.h"

void namespace_init(void);
int namespace_register(const char *path, obj_handler_t hd);
int namespace_query(const char *path, obj_handler_t *hd);
int namespace_pre_alloc_map_fd(void);
void namespace_loop(void);
