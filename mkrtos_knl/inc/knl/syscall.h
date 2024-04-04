#pragma once

#include "types.h"
#include <arch.h>

typedef pf_t entry_frame_t;

typedef void (*syscall_entry_func)(entry_frame_t *entry);
void syscall_entry(entry_frame_t *entry);
