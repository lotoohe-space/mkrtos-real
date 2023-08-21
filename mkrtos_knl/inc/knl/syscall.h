#pragma once

#include "types.h"

typedef struct entry_frame
{
    mword_t r[8];
} entry_frame_t;

typedef int (*syscall_entry_func)(entry_frame_t entry);
int syscall_entry(entry_frame_t entry);
