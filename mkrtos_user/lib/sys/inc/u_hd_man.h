#pragma once

#include "u_types.h"
#define HANDLER_INVALID ((umword_t)(-1))

obj_handler_t handler_alloc(void);
void handler_free(obj_handler_t hd_inx);
void handler_free_umap(obj_handler_t hd_inx);