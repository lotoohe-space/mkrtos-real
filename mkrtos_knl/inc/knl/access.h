#pragma once

#include <types.h>

bool_t is_rw_access(task_t *tg_task, void *addr, size_t size, bool_t ignore_null);
