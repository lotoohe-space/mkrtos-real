#pragma once

#include <types.h>
#include <task.h>
int elf_load(task_t *task, umword_t elf_data, size_t size, addr_t *entry_addr);
