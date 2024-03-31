#pragma once
#include <elf.h>
#include <mk_sys.h>

void elf_load_knl(mword_t elf_data, Elf64_Addr *entry);
