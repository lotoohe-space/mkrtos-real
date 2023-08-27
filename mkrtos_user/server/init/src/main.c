
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include <assert.h>
#include <stdio.h>
void ulog_test(void)
{
    ulog_write_str(LOG_PROT, "Init task running..\n");
    ulog_write_str(LOG_PROT, "todo..\n");
}
void mm_test(void)
{
    void *mem = mm_alloc_page(MM_PROT, 1, REGION_RWX);
    assert(mem);
    memset(mem, 0, 512);
    void *mem1 = mm_alloc_page(MM_PROT, 1, REGION_RWX);
    assert(mem1);
    memset(mem1, 0, 512);

    // mm_free_page(MM_PROT, mem1, 1);
    // mm_free_page(MM_PROT, mem, 1);
    // memset(mem, 0, 512);
    // memset(mem1, 0, 512);
}
void mpu_test(void)
{

#if 0
    // mpu保护测试
    int *test = ((int *)0x8000000);
    *test = 1;
    printf("test is %d\n", *test);
#endif
}
void printf_test(void)
{
    printf("print test0.\n");
    printf("print test1.\n");
    printf("print test2.\n");
    float a = 1.1;
    float b = 1.2;
    float c = a + b;
    c = c;
    printf("%c %d %lf\n", 'a', 1234, 1.1);
}
int main(int argc, char *args[])
{
    mm_test();
    mpu_test();
    ulog_test();
    printf_test();
    while (1)
        ;
    return 0;
}
