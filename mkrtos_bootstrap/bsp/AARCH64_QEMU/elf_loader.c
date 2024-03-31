
#include <elf.h>
#include <string.h>

void elf_load_knl(mword_t elf_data, Elf64_Addr *entry)
{
    Elf64_Ehdr *elf_header;

    elf_header = (Elf64_Ehdr *)elf_data;
    *entry = elf_header->e_entry;
    int size = elf_header->e_phentsize;
    Elf64_Phdr *elf_phdr = (Elf64_Phdr *)(elf_header->e_phoff + elf_data);

    for (int i = 0; i < elf_header->e_phnum; i++, elf_phdr++)
    {
        if (elf_phdr->p_type == PT_LOAD)
        {
            memcpy((char *)elf_phdr->p_paddr, (char *)(elf_data + elf_phdr->p_offset), elf_phdr->p_filesz);
        }
    }
}
