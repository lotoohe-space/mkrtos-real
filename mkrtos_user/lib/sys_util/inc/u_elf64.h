#pragma once

#include <u_types.h>

int elf_load(umword_t elf_data, size_t size, addr_t *entry_addr, obj_handler_t dst_task);
