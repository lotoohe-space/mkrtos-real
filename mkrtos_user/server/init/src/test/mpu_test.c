#include "u_types.h"

void mpu_test(void)
{

#if 0
    // mpu保护测试
    int *test = ((int *)0x8000000);
    *test = 1;
    printf("test is %d\n", *test);
#endif
}