#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

// 生成 B 指令的二进制码
uint16_t generate_b_instruction(int32_t ofst)
{
    // 计算偏移量
    int32_t offset = (ofst - 4) / 2;

    // 检查偏移量是否在合法范围内（11 位有符号数）
    if (offset < -1024 || offset > 1023)
    {
        printf("Error: Offset out of range for B instruction.\n");
        return 0;
    }

    // 将偏移量限制为 11 位
    offset &= 0x7FF;

    // 生成 B 指令的二进制码
    uint16_t instruction = 0xE000 | offset; // 0xE000 是 B 指令的操作码
    return instruction;
}

void copy_phdrs_and_load_segments(const char *input_file, const char *output_file)
{
    int fd;
    struct stat st;
    Elf32_Ehdr *ehdr;
    Elf32_Phdr *phdr;
    void *map_start;
    int i;

    // 打开输入文件
    fd = open(input_file, O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // 获取文件大小
    if (fstat(fd, &st) < 0)
    {
        perror("fstat");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // 将文件映射到内存
    map_start = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    ehdr = (Elf32_Ehdr *)map_start;
    phdr = (Elf32_Phdr *)(map_start + ehdr->e_phoff);

    // 打开输出文件
    FILE *out_fp = fopen(output_file, "wb");
    if (!out_fp)
    {
        perror("fopen");
        munmap(map_start, st.st_size);
        close(fd);
        exit(EXIT_FAILURE);
    }

    // 2. 遍历程序头表，找到所有 LOAD 段并拷贝到输出文件
    for (i = 0; i < ehdr->e_phnum; i++)
    {
        void *p_mem = calloc(1, phdr[i].p_memsz);

        if (p_mem == NULL)
        {
            fprintf(stderr, "malloc failed for segment %d\n", i);
            exit(-1);
        }
        memcpy(p_mem, (void *)map_start + phdr[i].p_offset, phdr[i].p_filesz);

        if (phdr[i].p_type == PT_LOAD)
        {
            fseek(out_fp, phdr[i].p_vaddr, SEEK_SET);

            // 将 LOAD 段写入输出文件
            if (fwrite(p_mem, 1, phdr[i].p_memsz, out_fp) != phdr[i].p_memsz)
            {
                perror("fwrite (LOAD segment)");
                fclose(out_fp);
                munmap(map_start, st.st_size);
                close(fd);
                exit(EXIT_FAILURE);
            }
            printf("Loaded segment at offset %zu with size 0x%x\n", phdr[i].p_offset, phdr[i].p_memsz);
        }
        else if (phdr[i].p_type == PT_DYNAMIC)
        {
            fseek(out_fp, phdr[i].p_vaddr, SEEK_SET);
            // 将 DYNAMIC 段写入输出文件
            if (fwrite(p_mem, 1, phdr[i].p_memsz, out_fp) != phdr[i].p_memsz)
            {
                perror("fwrite (DYNAMIC segment)");
                fclose(out_fp);
                munmap(map_start, st.st_size);
                close(fd);
                exit(EXIT_FAILURE);
            }
            printf("dynamic segment at offset %zu with size 0x%x\n", phdr[i].p_offset, phdr[i].p_memsz);
        }
        free(p_mem);
    }
    // 重写phdr
    fseek(out_fp, ehdr->e_phoff, SEEK_SET);
    int st_i = 0;
    for (int i = 0; i < ehdr->e_phnum; i++)
    {
        if (phdr[i].p_type == PT_LOAD || phdr[i].p_type == PT_DYNAMIC)
        {
            if (st_i != 0)
            {
                // if (st_i == 1)
                // {
                //     phdr[i].p_offset = phdr[i].p_vaddr;
                // }
                // phdr[i].p_offset = (phdr[i - 1].p_memsz + phdr[i - 1].p_offset + (phdr[i].p_align - 1)) & (~(phdr[i].p_align - 1));
                // phdr[i].p_offset = phdr[i - 1].p_memsz + phdr[i - 1].p_offset;
                phdr[i].p_offset = phdr[i].p_vaddr;
            }
            else
            {
                phdr[i].p_flags |= PF_X;
            }
            // phdr[i].p_align = 4;
            st_i++;
        }
        else
        {
            phdr[i].p_type = PT_NULL;
        }
    }
    fwrite(phdr, ehdr->e_phentsize * ehdr->e_phnum, 1, out_fp);
    // 重写ehdr
    fseek(out_fp, 0, SEEK_SET);
    ehdr->e_shnum = 0;
    ehdr->e_shoff = 0;
    ehdr->e_shentsize = 0;
    ehdr->e_shstrndx = 0;
    ehdr->e_entry = 0x1;
    fwrite(ehdr, sizeof(*ehdr), 1, out_fp);
    fseek(out_fp, 0, SEEK_SET);
    uint16_t jump_to = generate_b_instruction(0xd8);
    fwrite(&jump_to, 2, 1, out_fp);

    // 清理资源
    fclose(out_fp);
    munmap(map_start, st.st_size);
    close(fd);
}

int main(int argc, char *argv[])
{
    // if (argc != 3)
    // {
    //     fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
    //     exit(EXIT_FAILURE);
    // }
    argv[1] = "/home/zhangzheng/projects/mkrtos-real/build/output/libc.so";
    argv[2] = "libc.bin";
    copy_phdrs_and_load_segments(argv[1], argv[2]);

    printf("Program headers and LOAD segments copied to %s\n", argv[2]);

    return 0;
}