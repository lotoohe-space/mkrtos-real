
#include <types.h>
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint64_t Elf64_Xword;
#define EI_NIDENT (16)

#pragma pack(1)
typedef struct
{
    unsigned char e_ident[EI_NIDENT]; /* 一个字节数组用来确认文件是否是一个ELF文件 */
    Elf64_Half e_type;                /* 描述文件是,可执行文件elf=2,重定位so=3 */
    Elf64_Half e_machine;             /* 目标主机架构 */
    Elf64_Word e_version;             /* ELF文件格式的版本 */
    Elf64_Addr e_entry;               /* 入口点虚拟地址 */
    Elf64_Off e_phoff;                /* 程序头文件偏移 */
    Elf64_Off e_shoff;                /* 节头表文件偏移 */
    Elf64_Word e_flags;               /* ELF文件标志 */
    Elf64_Half e_ehsize;              /* ELF头大小 */
    Elf64_Half e_phentsize;           /* 程序头大小 */
    Elf64_Half e_phnum;               /* 程序头表计数 */
    Elf64_Half e_shentsize;           /* 节头表大小 */
    Elf64_Half e_shnum;               /* 节头表计数 */
    Elf64_Half e_shstrndx;            /* 字符串表索引节头 */
} Elf64_Ehdr;
#pragma pack(1)
typedef struct
{
    Elf64_Word p_type;    /* Segment type */
    Elf64_Word p_flags;   /* Segment flags */
    Elf64_Off p_offset;   /* Segment file offset */
    Elf64_Addr p_vaddr;   /* Segment virtual address */
    Elf64_Addr p_paddr;   /* Segment physical address */
    Elf64_Xword p_filesz; /* Segment size in file */
    Elf64_Xword p_memsz;  /* Segment size in memory */
    Elf64_Xword p_align;  /* Segment alignment */
} Elf64_Phdr;
#define PT_NULL 0                  /* Program header table entry unused */
#define PT_LOAD 1                  /* Loadable program segment */
#define PT_DYNAMIC 2               /* Dynamic linking information */
#define PT_INTERP 3                /* Program interpreter */
#define PT_NOTE 4                  /* Auxiliary information */
#define PT_SHLIB 5                 /* Reserved */
#define PT_PHDR 6                  /* Entry for header table itself */
#define PT_TLS 7                   /* Thread-local storage segment */
#define PT_NUM 8                   /* Number of defined types */
#define PT_LOOS 0x60000000         /* Start of OS-specific */
#define PT_GNU_EH_FRAME 0x6474e550 /* GCC .eh_frame_hdr segment */
#define PT_GNU_STACK 0x6474e551    /* Indicates stack executability */
#define PT_GNU_RELRO 0x6474e552    /* Read-only after relocation */
#define PT_LOSUNW 0x6ffffffa
#define PT_SUNWBSS 0x6ffffffa   /* Sun Specific segment */
#define PT_SUNWSTACK 0x6ffffffb /* Stack segment */
#define PT_HISUNW 0x6fffffff
#define PT_HIOS 0x6fffffff   /* End of OS-specific */
#define PT_LOPROC 0x70000000 /* Start of processor-specific */
#define PT_HIPROC 0x7fffffff /* End of processor-specific */

#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4
#define ET_NUM 5
#define ET_LOOS 0xfe00
#define ET_HIOS 0xfeff
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff

#define ELFMAG "\177ELF"
#define SELFMAG 4
#include <string.h>
#include <printk.h>
int elf_check(Elf64_Ehdr *ehdr)
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

int elf_load(umword_t elf_data)
{
    int ret;
    Elf64_Ehdr *elf_header;

    elf_header = (Elf64_Ehdr *)elf_data;

    ret = elf_check(elf_header);
    if (ret != 0)
    {
        return ret;
    }
    int size = elf_header->e_phentsize;
    Elf64_Phdr *elf_phdr = (Elf64_Phdr *)(elf_header->e_phoff + elf_data);

    for (int i = 0; i < elf_header->e_phnum; i++, elf_phdr++)
    {
        if (elf_phdr->p_type == PT_LOAD)
        {
            printk("[] paddr:0x%x vaddr:0x%x memsize:%d filesize:%d\n",
                   elf_phdr->p_vaddr, elf_phdr->p_paddr, elf_phdr->p_memsz, elf_phdr->p_filesz);
            // 申请内存，拷贝数据，然后转移给ini_task
            // memcpy((char *)elf_phdr->p_paddr, (char *)(elf_data + elf_phdr->p_offset),
            //        elf_phdr->p_filesz);
        }
    }
}
