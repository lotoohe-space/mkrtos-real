#include <sys/mman.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
void mm_test(void)
{
#define TEST_MEM_SIZE (1024)
    for (int i = 0; i < 1024; i++)
    {
        void *mem = mmap(0, TEST_MEM_SIZE, PROT_READ | PROT_WRITE | PROT_PFS, MAP_PRIVATE, -1, 0);
        if (mem == NULL)
        {
            printf("mmap failed.\n");
            return;
        }
        memset(mem, 0, TEST_MEM_SIZE);
        if (munmap(mem, TEST_MEM_SIZE) < 0)
        {
            printf("munmap failed.\n");
        }
    }
#undef TEST_MEM_SIZE
}
void mm_test1(void)
{
#define TEST_MEM_SIZE (1024 + 512)
    for (int i = 0; i < 1024; i++)
    {
        void *mem = mmap(0, TEST_MEM_SIZE, PROT_READ | PROT_WRITE | PROT_PFS, MAP_PRIVATE, -1, 0);
        if ((long)mem < 0)
        {
            printf("mmap failed.\n");
            return;
        }
        memset(mem, 0, TEST_MEM_SIZE);
        // if (munmap(mem, TEST_MEM_SIZE) < 0)
        // {
        //     printf("munmap failed.\n");
        // }
    }
#undef TEST_MEM_SIZE
}