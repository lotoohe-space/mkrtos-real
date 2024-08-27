#include <elf.h>
#include <u_types.h>
#include <stdio.h>
#include <u_vmam.h>
#include <u_prot.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
static int elf_check(Elf64_Ehdr *ehdr)
{
    if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0)
    {
        return -1;
    }

    if (ehdr->e_type != ET_EXEC)
    {
        return -1;
    }
    /*TODO:check arch.*/

    return 0;
}

int elf_load(umword_t elf_data, size_t size, addr_t *entry_addr, obj_handler_t dst_task)
{
    int ret;
    Elf64_Ehdr *elf_header;

    assert(entry_addr);
    elf_header = (Elf64_Ehdr *)elf_data;
    ret = elf_check(elf_header);
    if (ret != 0)
    {
        return ret;
    }
    // int size = elf_header->e_phentsize;
    Elf64_Phdr *elf_phdr = (Elf64_Phdr *)(elf_header->e_phoff + elf_data);
    size_t mem_size = 0;
    addr_t st_addr = 0;
    addr_t end_addr = 0;

    *entry_addr = elf_header->e_entry;

    for (int i = 0; i < elf_header->e_phnum; i++, elf_phdr++)
    {
        if (elf_phdr->p_type == PT_LOAD)
        {
            printf("u[elf] paddr:0x%x vaddr:0x%x memsize:%d filesize:%d ofst:%d align:0x%x\n",
                   elf_phdr->p_vaddr, elf_phdr->p_paddr, elf_phdr->p_memsz, elf_phdr->p_filesz,
                   elf_phdr->p_offset, elf_phdr->p_align);

            if (i == 0)
            {
                st_addr = elf_phdr->p_vaddr;
            }

            end_addr = ALIGN(elf_phdr->p_vaddr + elf_phdr->p_memsz, elf_phdr->p_align);
        }
    }
    mem_size = end_addr - st_addr;
    void *mem;
    msg_tag_t tag;
    
    printf("%s:%d.\n", __func__, __LINE__);

    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, 0), mem_size, 0, (addr_t *)(&mem));
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    printf("%s:%d mem:0x%p size:0x%x.\n", __func__, __LINE__, mem, mem_size);

    memset(mem, 0, mem_size);
    printf("%s:%d.\n", __func__, __LINE__);

    mword_t offset = 0;

    elf_phdr = (Elf64_Phdr *)(elf_header->e_phoff + elf_data);
    for (int i = 0; i < elf_header->e_phnum; i++, elf_phdr++)
    {
        if (elf_phdr->p_type == PT_LOAD)
        {
            memcpy((char *)mem + (elf_phdr->p_vaddr - st_addr),
                   (void *)(elf_data + elf_phdr->p_offset),
                   elf_phdr->p_filesz);
            offset += ALIGN(elf_phdr->p_memsz, elf_phdr->p_align);
        }
    }
    tag = u_vmam_grant(VMA_PROT, dst_task, (addr_t)mem, st_addr, mem_size);
    if (msg_tag_get_val(tag) < 0)
    {
        u_vmam_free(VMA_PROT, (addr_t)mem, mem_size);
        return msg_tag_get_val(tag);
    }
    return ret;
}

